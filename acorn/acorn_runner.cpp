#include "acorn_runner.h"
#include "core/os/memory.h"
#include "core/os/os.h"
#include "core/print_string.h"
#include "core/io/file_access_pack.h"
#include "core/io/resource_loader.h"
#include "scene/main/viewport.h"
#include "scene/resources/dynamic_font.h"
#include "scene/resources/packed_scene.h"
#include "thirdparty/minhook/include/MinHook.h"

bool AcornRunner::initialized;
HMODULE AcornRunner::m_DLL;

bool(*m_AcornInit)();
bool(*m_AcornUpdate)(float delta);
void(*m_AcornCleanup)(float delta);

using FUNC_OS_get_singleton = OS *(*)();
FUNC_OS_get_singleton OS_get_singleton;

OS *HOOK_OS_get_singleton() {
	return OS::get_singleton();
}

using FUNC_FileAccess_create_for_path = FileAccess *(*)(const String &);
FUNC_FileAccess_create_for_path FileAccess_create_for_path;

FileAccess *HOOK_FileAccess_create_for_path(const String &p_path) {
	return FileAccess::create_for_path(p_path);
}

using FUNC_ResourceLoader_load = RES (*)(const String &, const String &, bool, Error *);
FUNC_ResourceLoader_load ResourceLoader_load;

RES HOOK_ResourceLoader_load(const String& p_path, const String& p_type_hint = "", bool p_no_cache = false, Error* r_error = NULL)
{
	return ResourceLoader::load(p_path, p_type_hint, p_no_cache, r_error);
}

using FUNC_String_ctor = String *(*)(String &, const char *);
FUNC_String_ctor String_ctor;

String *&HOOK_String_ctor(String *in, const char *p_str) {
	*(uintptr_t *)in = 0;

	if (*p_str == '\x00') {
		*in = String();
		return in;
	}

	*in = String(std::string(p_str).c_str());
	return in;
}

using FUNC_DynamicFont_ctor = DynamicFont*(*)(DynamicFont *);
FUNC_DynamicFont_ctor DynamicFont_ctor;

DynamicFont* HOOK_DynamicFont_ctor(DynamicFont &in) {
	return new ("") DynamicFont;
}

using FUNC_Object_postinitialize = void (*)(Object *);
FUNC_Object_postinitialize Object_postinitialize;

void HOOK_Object_postinitialize(Object *ctx) {
	postinitialize_handler(ctx);
}

using FUNC_DynamicFont_set_font_data = void (*)(DynamicFont *, const Ref<DynamicFontData> &);
FUNC_DynamicFont_set_font_data DynamicFont_set_font_data;

void HOOK_DynamicFont_set_font_data(DynamicFont *ctx, const Ref<DynamicFontData> &p_data) {
	ctx->set_font_data(p_data);
}

using FUNC_Reference_init_ref = bool (*)(Reference *);
FUNC_Reference_init_ref Reference_init_ref;

bool HOOK_Reference_init_ref(Reference *ctx) {
	return ctx->init_ref();
}

using FUNC_Reference_reference = bool (*)(Reference *);
FUNC_Reference_reference Reference_reference;

bool HOOK_Reference_reference(Reference *ctx) {
	return ctx->reference();
}

using FUNC_Reference_unreference = bool (*)(Reference *);
FUNC_Reference_unreference Reference_unreference;

bool HOOK_Reference_unreference(Reference *ctx) {
	return ctx->unreference();
}

using FUNC_PackedScene_instance = Node *(*)(PackedScene *, PackedScene::GenEditState);
FUNC_PackedScene_instance PackedScene_instance;

Node *HOOK_PackedScene_instance(PackedScene *ctx, PackedScene::GenEditState p_edit_state) {
	return ctx->instance(p_edit_state);
}

using FUNC_Node_get_node = Node *(*)(Node *, NodePath *, char);
FUNC_Node_get_node Node_get_node;

Node *HOOK_Node_get_node(Node *ctx, const NodePath &p_path, char) {
	return ctx->get_node(p_path);
}

using FUNC_NodePath_ctor = Node *(*)(Node *, NodePath *);
FUNC_NodePath_ctor NodePath_ctor;

NodePath *HOOK_NodePath_ctor(NodePath *ctx, String &p_path) {
	*ctx = NodePath(p_path);
	return ctx;
}

using FUNC_print_line = void (*)(String);
FUNC_print_line _print_line;

void HOOK_print_line(String p_string) {
	print_line(p_string);
}

using FUNC_unk = uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t);
FUNC_unk _unk;

uintptr_t HOOK_unk(uintptr_t a1, uintptr_t a2, uintptr_t a3) {
	return a2;
}

using FUNC_get_sound_system = void* (*)();
FUNC_get_sound_system get_sound_system;

void* HOOK_get_sound_system() {
	void *sound_system = (void*)((uintptr_t)AcornRunner::m_DLL + 0x6669A60);

	if (*(uintptr_t *)sound_system) return sound_system;

	((void *(*)(void *))((uintptr_t)AcornRunner::m_DLL + 0x17B6100))(sound_system);

	return sound_system;
}

using FUNC_Viewport_get_size = Size2 (*)(Viewport *);
FUNC_Viewport_get_size Viewport_get_size;

Size2 HOOK_Viewport_get_size(Viewport *ctx) {
	return ctx->get_size();
}

using FUNC_Object_call = Variant (*)(Object *, const StringName &, const Variant **, int , Variant::CallError &);
FUNC_Object_call Object_call;

Variant HOOK_Object_call(Object* ctx, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	return ctx->call(p_method, p_args, p_argcount, r_error);
}

using FUNC_Node_add_child = void (*)(Node *, Node*, bool);
FUNC_Node_add_child Node_add_child;

void HOOK_Node_add_child(Node *ctx, Node* p_child, bool p_legible_unique_name) {
	ctx->add_child(p_child, p_legible_unique_name);
}

using FUNC_Node_find_parent = Node* (*)(Node *, const String&);
FUNC_Node_find_parent Node_find_parent;

Node* HOOK_Node_find_parent(Node *ctx, const String& p_mask) {
	return ctx->find_parent(p_mask);
}

using FUNC_predelete_handler = void (*)(Object *);
FUNC_predelete_handler _predelete_handler;

void HOOK_predelete_handler(Object *p_object) {
	predelete_handler(p_object);
}

using FUNC_CowData_Vector2_resize = Error (*)(CowData<Vector2> *, int&);
FUNC_CowData_Vector2_resize CowData_Vector2_resize;

Error HOOK_CowData_Vector2_resize(CowData<Vector2> *ctx, int p_size) {
	return ctx->resize(p_size);
}

using FUNC_CowData_Color_resize = Error (*)(CowData<Color> *, int&);
FUNC_CowData_Color_resize CowData_Color_resize;

Error HOOK_CowData_Color_resize(CowData<Color> *ctx, int p_size) {
	return ctx->resize(p_size);
}

bool AcornRunner::apply_hooks() 
{
	MH_Initialize();

	OS_get_singleton = (FUNC_OS_get_singleton)((uintptr_t)m_DLL + 0x1B6F20);
	if (MH_CreateHook(OS_get_singleton, &HOOK_OS_get_singleton, nullptr) != MH_OK) {
		print_error("Failed to create hook for OS::get_singleton!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(OS_get_singleton) != MH_OK) {
		print_error("Failed to enable hook for OS::get_singleton!");
		m_DLL = nullptr;
		return false;
	}

	FileAccess_create_for_path = (FUNC_FileAccess_create_for_path)((uintptr_t)m_DLL + 0x2182C0);
	if (MH_CreateHook(FileAccess_create_for_path, &HOOK_FileAccess_create_for_path, nullptr) != MH_OK) {
		print_error("Failed to create hook for FileAccess::create_for_path!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(FileAccess_create_for_path) != MH_OK) {
		print_error("Failed to enable hook for FileAccess::create_for_path!");
		m_DLL = nullptr;
		return false;
	}

	ResourceLoader_load = (FUNC_ResourceLoader_load)((uintptr_t)m_DLL + 0x27DC80);
	if (MH_CreateHook(ResourceLoader_load, &HOOK_ResourceLoader_load, nullptr) != MH_OK) {
		print_error("Failed to create hook for ResourceLoader::load!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(ResourceLoader_load) != MH_OK) {
		print_error("Failed to enable hook for ResourceLoader::load!");
		m_DLL = nullptr;
		return false;
	}

	String_ctor = (FUNC_String_ctor)((uintptr_t)m_DLL + 0x1AD1E0);
	if (MH_CreateHook(String_ctor, &HOOK_String_ctor, nullptr) != MH_OK) {
		print_error("Failed to create hook for String::String!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(String_ctor) != MH_OK) {
		print_error("Failed to enable hook for String::String!");
		m_DLL = nullptr;
		return false;
	}

	DynamicFont_ctor = (FUNC_DynamicFont_ctor)((uintptr_t)m_DLL + 0x9B90E0);
	if (MH_CreateHook(DynamicFont_ctor, &HOOK_DynamicFont_ctor, nullptr) != MH_OK) {
		print_error("Failed to create hook for DynamicFont::DynamicFont!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(DynamicFont_ctor) != MH_OK) {
		print_error("Failed to enable hook for DynamicFont::DynamicFont!");
		m_DLL = nullptr;
		return false;
	}

	Object_postinitialize = (FUNC_Object_postinitialize)((uintptr_t)m_DLL + 0x1C0D00);
	if (MH_CreateHook(Object_postinitialize, &HOOK_Object_postinitialize, nullptr) != MH_OK) {
		print_error("Failed to create hook for Object::_postinitialize!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Object_postinitialize) != MH_OK) {
		print_error("Failed to enable hook for Object::_postinitialize!");
		m_DLL = nullptr;
		return false;
	}

	DynamicFont_set_font_data = (FUNC_DynamicFont_set_font_data)((uintptr_t)m_DLL + 0x9B7660);
	if (MH_CreateHook(DynamicFont_set_font_data, &HOOK_DynamicFont_set_font_data, nullptr) != MH_OK) {
		print_error("Failed to create hook for DynamicFont::set_font_data!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(DynamicFont_set_font_data) != MH_OK) {
		print_error("Failed to enable hook for DynamicFont::set_font_data!");
		m_DLL = nullptr;
		return false;
	}

	Reference_init_ref = (FUNC_Reference_init_ref)((uintptr_t)m_DLL + 0x1E1EE0);
	if (MH_CreateHook(Reference_init_ref, &HOOK_Reference_init_ref, nullptr) != MH_OK) {
		print_error("Failed to create hook for Reference::init_ref!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Reference_init_ref) != MH_OK) {
		print_error("Failed to enable hook for Reference::init_ref!");
		m_DLL = nullptr;
		return false;
	}

	Reference_reference = (FUNC_Reference_reference)((uintptr_t)m_DLL + 0x1E1F30);
	if (MH_CreateHook(Reference_reference, &HOOK_Reference_reference, nullptr) != MH_OK) {
		print_error("Failed to create hook for Reference::reference!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Reference_reference) != MH_OK) {
		print_error("Failed to enable hook for Reference::reference!");
		m_DLL = nullptr;
		return false;
	}

	Reference_unreference = (FUNC_Reference_unreference)((uintptr_t)m_DLL + 0x1E1FD0);
	if (MH_CreateHook(Reference_unreference, &HOOK_Reference_unreference, nullptr) != MH_OK) {
		print_error("Failed to create hook for Reference::unreference!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Reference_unreference) != MH_OK) {
		print_error("Failed to enable hook for Reference::unreference!");
		m_DLL = nullptr;
		return false;
	}

	PackedScene_instance = (FUNC_PackedScene_instance)((uintptr_t)m_DLL + 0x62B360);
	if (MH_CreateHook(PackedScene_instance, &HOOK_PackedScene_instance, nullptr) != MH_OK) {
		print_error("Failed to create hook for PackedScene::instance!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(PackedScene_instance) != MH_OK) {
		print_error("Failed to enable hook for PackedScene::instance!");
		m_DLL = nullptr;
		return false;
	}

	Node_get_node = (FUNC_Node_get_node)((uintptr_t)m_DLL + 0x58DAC0);
	if (MH_CreateHook(Node_get_node, &HOOK_Node_get_node, nullptr) != MH_OK) {
		print_error("Failed to create hook for Node::get_node!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Node_get_node) != MH_OK) {
		print_error("Failed to enable hook for Node::get_node!");
		m_DLL = nullptr;
		return false;
	}

	NodePath_ctor = (FUNC_NodePath_ctor)((uintptr_t)m_DLL + 0x3C71E0);
	if (MH_CreateHook(NodePath_ctor, &HOOK_NodePath_ctor, nullptr) != MH_OK) {
		print_error("Failed to create hook for NodePath::NodePath!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(NodePath_ctor) != MH_OK) {
		print_error("Failed to enable hook for NodePath::NodePath!");
		m_DLL = nullptr;
		return false;
	}

	_print_line = (FUNC_print_line)((uintptr_t)m_DLL + 0x215D60);
	if (MH_CreateHook(_print_line, &HOOK_print_line, nullptr) != MH_OK) {
		print_error("Failed to create hook for print_line!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(_print_line) != MH_OK) {
		print_error("Failed to enable hook for print_line!");
		m_DLL = nullptr;
		return false;
	}

	_unk = (FUNC_unk)((uintptr_t)m_DLL + 0x1BAD20);
	if (MH_CreateHook(_unk, &HOOK_unk, nullptr) != MH_OK) {
		print_error("Failed to create hook for unk!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(_unk) != MH_OK) {
		print_error("Failed to enable hook for unk!");
		m_DLL = nullptr;
		return false;
	}

	get_sound_system = (FUNC_get_sound_system)((uintptr_t)m_DLL + 0x1789DA0);
	if (MH_CreateHook(get_sound_system, &HOOK_get_sound_system, nullptr) != MH_OK) {
		print_error("Failed to create hook for get_sound_system!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(get_sound_system) != MH_OK) {
		print_error("Failed to enable hook for get_sound_system!");
		m_DLL = nullptr;
		return false;
	}

	Viewport_get_size = (FUNC_Viewport_get_size)((uintptr_t)m_DLL + 0x5BA160);
	if (MH_CreateHook(Viewport_get_size, &HOOK_Viewport_get_size, nullptr) != MH_OK) {
		print_error("Failed to create hook for Viewport::get_size!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Viewport_get_size) != MH_OK) {
		print_error("Failed to enable hook for Viewport::get_size!");
		m_DLL = nullptr;
		return false;
	}

	Object_call = (FUNC_Object_call)((uintptr_t)m_DLL + 0x1C5F40);
	if (MH_CreateHook(Object_call, &HOOK_Object_call, nullptr) != MH_OK) {
		print_error("Failed to create hook for Object::call!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Object_call) != MH_OK) {
		print_error("Failed to enable hook for Object::call!");
		m_DLL = nullptr;
		return false;
	}

	Node_add_child = (FUNC_Node_add_child)((uintptr_t)m_DLL + 0x58D1E0);
	if (MH_CreateHook(Node_add_child, &HOOK_Node_add_child, nullptr) != MH_OK) {
		print_error("Failed to create hook for Node::add_child!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Node_add_child) != MH_OK) {
		print_error("Failed to enable hook for Node::add_child!");
		m_DLL = nullptr;
		return false;
	}

	Node_find_parent = (FUNC_Node_find_parent)((uintptr_t)m_DLL + 0x58DCE0);
	if (MH_CreateHook(Node_find_parent, &HOOK_Node_find_parent, nullptr) != MH_OK) {
		print_error("Failed to create hook for Node::find_parent!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(Node_find_parent) != MH_OK) {
		print_error("Failed to enable hook for Node::find_parent!");
		m_DLL = nullptr;
		return false;
	}

	_predelete_handler = (FUNC_predelete_handler)((uintptr_t)m_DLL + 0x1C0CA0);
	if (MH_CreateHook(_predelete_handler, &HOOK_predelete_handler, nullptr) != MH_OK) {
		print_error("Failed to create hook for predelete_handler!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(_predelete_handler) != MH_OK) {
		print_error("Failed to enable hook for predelete_handler!");
		m_DLL = nullptr;
		return false;
	}

	CowData_Vector2_resize = (FUNC_CowData_Vector2_resize)((uintptr_t)m_DLL + 0x249B80);
	if (MH_CreateHook(CowData_Vector2_resize, &HOOK_CowData_Vector2_resize, nullptr) != MH_OK) {
		print_error("Failed to create hook for CowData<Vector2>::resize!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(CowData_Vector2_resize) != MH_OK) {
		print_error("Failed to enable hook for CowData<Vector2>::resize!");
		m_DLL = nullptr;
		return false;
	}

	CowData_Color_resize = (FUNC_CowData_Color_resize)((uintptr_t)m_DLL + 0x249890);
	if (MH_CreateHook(CowData_Color_resize, &HOOK_CowData_Color_resize, nullptr) != MH_OK) {
		print_error("Failed to create hook for CowData<Color>::resize!");
		m_DLL = nullptr;
		return false;
	}
	if (MH_EnableHook(CowData_Color_resize) != MH_OK) {
		print_error("Failed to enable hook for CowData<Color>::resize!");
		m_DLL = nullptr;
		return false;
	}

	return true;
}

void UnprotectPageIfNeeded(void *pagePointer) {
	MEMORY_BASIC_INFORMATION memoryInfo;
	VirtualQuery(pagePointer, &memoryInfo, sizeof(memoryInfo));
	if ((memoryInfo.Protect & PAGE_READONLY) > 0) {
		VirtualProtect(pagePointer, 1, PAGE_READWRITE, &memoryInfo.Protect);
	}
}

void ResolveDllImports(unsigned char* codeBase, PIMAGE_IMPORT_DESCRIPTOR importDesc)
{
	for (; importDesc->Name; importDesc++) {
		uintptr_t *thunkRef;
		FARPROC *funcRef;
		if (importDesc->OriginalFirstThunk) {
			thunkRef = (uintptr_t *)(codeBase + importDesc->OriginalFirstThunk);
			funcRef = (FARPROC *)(codeBase + importDesc->FirstThunk);
		} else {
			// no hint table
			thunkRef = (uintptr_t *)(codeBase + importDesc->FirstThunk);
			funcRef = (FARPROC *)(codeBase + importDesc->FirstThunk);
		}
		const char *libraryName = (LPCSTR)(codeBase + importDesc->Name);

		HMODULE libraryHandle = GetModuleHandleA(libraryName);
		if (libraryHandle == nullptr) {
			std::string libraryNameString = libraryName;
			// try to load library only once
			libraryHandle = LoadLibraryA(libraryName);
		}

		// iterate all thunk import entries and resolve them
		for (; *thunkRef; thunkRef++, funcRef++) {
			const char *importDescriptor;
			if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
				importDescriptor = (LPCSTR)IMAGE_ORDINAL(*thunkRef);
			} else {
				auto thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + (*thunkRef));
				importDescriptor = (LPCSTR)&thunkData->Name;
			}
			UnprotectPageIfNeeded(funcRef);
			if (libraryHandle != nullptr) {
				// we have a library reference for a given import, so use GetProcAddress
				*funcRef = GetProcAddress(libraryHandle, importDescriptor);
			}
		}
	}
}

void set_data(void *addr, PBYTE data, int size) {
	DWORD old;

	bool success = VirtualProtect(addr, size, PAGE_READWRITE, &old);

	if (success) {
		memcpy(addr, data, size);

		VirtualProtect(addr, size, old, &old);
	}
}

void AcornRunner::_init() {
	m_DLL = LoadLibraryEx("Rainbow.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES);

	if (!m_DLL) return;

	auto dosHeader = (PIMAGE_DOS_HEADER)m_DLL;
	auto pNTHeader = (PIMAGE_NT_HEADERS)((LONGLONG)dosHeader + dosHeader->e_lfanew);
	auto importsDir = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (importsDir->Size) {
		auto baseImp = (PIMAGE_IMPORT_DESCRIPTOR)((unsigned char *)m_DLL + importsDir->VirtualAddress);
		UnprotectPageIfNeeded(baseImp);
		ResolveDllImports((unsigned char *)m_DLL, baseImp);
	}

	if (!apply_hooks()) return;

	// CRT/initializer stuff

	const uintptr_t initCrtRva = 0x28A3AD4;
	const uintptr_t initCrtAddr = ((uintptr_t)m_DLL + initCrtRva);
	if (!((char (*)(int))initCrtAddr)(1))
	{
		print_error("Failed to init C Runtime for Rainbow.dll!");
		m_DLL = nullptr;
		return;
	}

	const uintptr_t crtAcquireLockRva = 0x28A390C;
	const uintptr_t crtAcquireLockAddr = ((uintptr_t)m_DLL + crtAcquireLockRva);
	auto lock = ((char (*)())crtAcquireLockAddr)();

	*(int*)((uintptr_t)m_DLL + 0x709B5B8) = 2;

	const uintptr_t crtReleaseLockRva = 0x28A3D00;
	const uintptr_t crtReleaseLockAddr = ((uintptr_t)m_DLL + crtReleaseLockRva);
	((char (*)(char))crtReleaseLockAddr)(lock);

	InitializeCriticalSection((LPCRITICAL_SECTION)((uintptr_t)m_DLL + 0x64C1E18));
	InitializeCriticalSection((LPCRITICAL_SECTION)((uintptr_t)m_DLL + 0x709C4F0));

	*(unsigned char *)((uintptr_t)m_DLL + 0x64C1E0C) = 2;
	set_data((void *)((uintptr_t)m_DLL + 0x28F479E), (PBYTE) "\x30", 1);

	set_data((void *)((uintptr_t)m_DLL + 0x28F47CF), (PBYTE) "\x58\x01", 2);
	*(PackedData **)((uintptr_t)m_DLL + 0x665F1F0) = PackedData::get_singleton();

	set_data((void *)((uintptr_t)m_DLL + 0x1B6F96), (PBYTE) "\x60", 1);
	set_data((void *)((uintptr_t)m_DLL + 0x1B6FD6), (PBYTE) "\x60", 1);

	set_data((void *)((uintptr_t)m_DLL + 0x16E8E41), (PBYTE) "\x30", 1);

	set_data((void *)((uintptr_t)m_DLL + 0x569AE3), (PBYTE) "\x50\x02", 2);

	set_data((void *)((uintptr_t)m_DLL + 0x45EF2F8), (PBYTE) "data/cri.cpk", 13);
	set_data((void *)((uintptr_t)m_DLL + 0x45EF308), (PBYTE) "data/cri.cpk", 13);

	InitializeCriticalSectionEx((LPCRITICAL_SECTION)((uintptr_t)m_DLL + 0x709B610), 0xFA0, 0);
	InitializeConditionVariable((PCONDITION_VARIABLE)((uintptr_t)m_DLL + 0x709B638));

	void *chao = (void *)((uintptr_t)m_DLL + 0x709BE10);

	((void *(*)(void *))((uintptr_t)m_DLL + 0x28C1440))(chao);

	// END CRT/initializer stuff

	const uintptr_t acornInitRVA = 0x1703F20;
	const uintptr_t acornInitAddr = ((uintptr_t)m_DLL + acornInitRVA);
	m_AcornInit = (bool(*)())acornInitAddr;

	const uintptr_t acornUpdateRVA = 0x1704030;
	const uintptr_t acornUpdateAddr = ((uintptr_t)m_DLL + acornUpdateRVA);
	m_AcornUpdate = (bool(*)(float))acornUpdateAddr;

	const uintptr_t acornCleanupRVA = 0x4D3B00;
	const uintptr_t acornCleanupAddr = ((uintptr_t)m_DLL + acornCleanupRVA);
	m_AcornCleanup = (void(*)(float))acornCleanupAddr;

	m_AcornInit();
}

void AcornRunner::_process(float delta) {
	if (!m_DLL) return;
	
	m_AcornUpdate(delta);
	m_AcornCleanup(delta);
}

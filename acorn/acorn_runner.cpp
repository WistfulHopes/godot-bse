#include "acorn_runner.h"
#include "core/os/os.h"
#include "thirdparty/minhook/include/MinHook.h"

bool AcornRunner::initialized;
HMODULE AcornRunner::m_DLL;

bool(*m_AcornInit)();
bool(*m_AcornUpdate)(float delta);
void(*m_AcornCleanup)(float delta);

using FUNC_OS_get_singleton = OS*(*)();
FUNC_OS_get_singleton OS_get_singleton;

OS* HOOK_OS_get_singleton() 
{
	return OS::get_singleton();
}

void AcornRunner::apply_hooks() 
{
	MH_Initialize();
	
	OS_get_singleton = (FUNC_OS_get_singleton)((uintptr_t)m_DLL + 0x1B6F20);
	MH_CreateHook(OS_get_singleton, &HOOK_OS_get_singleton, nullptr);
	MH_EnableHook(OS_get_singleton);
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

typedef BOOL(WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

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
		if (pNTHeader->OptionalHeader.AddressOfEntryPoint != 0) {
			auto DllEntry = (DllEntryProc)(LPVOID)((unsigned char *)m_DLL + pNTHeader->OptionalHeader.AddressOfEntryPoint);
			// notify library about attaching to process
			(*DllEntry)((HINSTANCE)m_DLL, DLL_PROCESS_ATTACH, nullptr);
		}
	}

	apply_hooks();

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

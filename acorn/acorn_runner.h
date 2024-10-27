#ifndef ACORNRUNNER_H
#define ACORNRUNNER_H

#include "scene/main/node.h"
#include <Windows.h>

class AcornRunner {

public:
	static HMODULE m_DLL;
	static bool initialized;

	AcornRunner();

	static void _init();
	static bool apply_hooks();
	static void _process(float delta);
};

#endif

#ifndef ACORNRUNNER_H
#define ACORNRUNNER_H

#include "scene/main/node.h"
#include <Windows.h>

class AcornRunner {

private:
	static HMODULE m_DLL;

public:
	static bool initialized;

	AcornRunner();

	static void _init();
	static void apply_hooks();
	static void _process(float delta);
};

#endif

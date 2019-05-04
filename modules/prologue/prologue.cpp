#include "prologue.hpp"

#include <EGG/core/eggDvdRipper.hpp>
#include <EGG/core/eggExpHeap.hpp>

#include <Common/rk_types.h>

#include <libpokey/debug.h>
#include <libpokey/mkw/heaps.hpp>

#include <lib/ModuleLoader.hpp>
#include <lib/MemoryPatcher.hpp>

// for ctors
#include <modules/reload/reload.hpp>
#include <modules/bootstrap/bootstrap.hpp>

#include <vector.h>

namespace Prologue {


void applyPatches(u32* block);
void doPatch32(void* patch);


ModuleLoader* spModuleLoader;
MemoryPatcher* spMemoryPatcher;

// The first function will be the entry point.
void prologue()
{
	BOOL iState = OSDisableInterrupts();
	u32 amountRead;

	if (spModuleLoader)
		delete spModuleLoader;
	spModuleLoader = new ModuleLoader();

	if (spMemoryPatcher)
		spMemoryPatcher->revertPatches();
	else
		spMemoryPatcher = new MemoryPatcher();

	spMemoryPatcher->processDiscPatchFile();

	// call prologue functions
	
	// TODO: global system

	spModuleLoader->appendNewModule<Reload::Reloader>();
	spModuleLoader->appendNewModule<Bootstrap::Bootstrap>();

	spModuleLoader->loadModules();

	DebugReport("DONE\n");



	OSRestoreInterrupts(iState);
}



} // namespace Prologue
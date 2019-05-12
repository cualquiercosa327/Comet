#include "prologue.hpp"

#include <EGG/core/eggDvdRipper.hpp>
#include <EGG/core/eggExpHeap.hpp>

#include <Common/rk_types.h>

#include <libpokey/debug.h>
#include <libpokey/mkw/heaps.hpp>

#include <lib/CometSystem.hpp>

// for ctors
#include <modules/reload/reload.hpp>
#include <modules/log/log.hpp>

#include <libpokey/hooks.h>
#include <modules/lightCycler/cycler.hpp>


chain* __global_destructor_chain;

namespace Prologue {

namespace Modules
{
Reload::Reloader _Reloader = Reload::Reloader();
Logging::CometLogger _CometLogger;
LightCycler _LightCycler;
}


typedef void(*ctor)(void);
// The first function will be the entry point.
void prologue()
{
	DebugReport("Disabling multitasking and initializing.\n");
	bool iState = OSDisableInterrupts();

	__global_destructor_chain = 0;
	
	DebugReport("nBytesCtor: %u; b %p e %p\n", (u32)__ctor_loc - (u32)__ctor_end, __ctor_loc, __ctor_end);
	

	int q = 0;
	for (void*** f = &__ctor_loc; f < &__ctor_end; f++)
	{
		DebugReport("Loading __sinit #%u\n", q++);
		
		((void(*)(void))*f)();
	}

	DebugReport("Initializing system!\n");
	CometSystem::initSystem();

	DebugReport("Handling patches!\n");
	CometSystem::getSystem()->processDiscPatches();

	// incompat with reload?
#if 0
	// Start the memory watcher (ensure patches are not overwritten).
	DebugReport("Starting memory watcher!\n");
	CometSystem::getSystem()->setupMemoryWatcher();
#endif

	DebugReport("Adding modules!\n");
	{
		ModuleLoader* pModuleLoader = &CometSystem::getSystem()->mModuleLoader;

#ifdef DEBUG
		pModuleLoader->appendModule(&Modules::_Reloader);
#endif
		pModuleLoader->appendModule(&Modules::_CometLogger);
		pModuleLoader->appendModule(&Modules::_LightCycler);

	}

	DebugReport("Loading modules!\n");
	CometSystem::getSystem()->mModuleLoader.loadModules();


	DebugReport("Restoring interrupt state and returning!\n");
	OSRestoreInterrupts(iState);
}

void tick()
{
	bool iState = OSDisableInterrupts();
	CometSystem::getSystem()->tick();
	OSRestoreInterrupts(iState);
}

// Hook frame begin
PokeyWritePointer(0x80270C14, tick);

// hook end of scenemgr calc
// PokeyBranch(0x8023AEA8, tick);
} // namespace Prologue




extern "C" void __register_global_object(void* obj, void* dtor, chain* newchain)
{
	//DebugReport("\n\n__register_global_object\n\n");

	newchain->last = __global_destructor_chain;
	newchain->dtor = dtor;
	newchain->obj = obj;
	__global_destructor_chain = newchain;
}

void destroyGlobals()
{
	for (chain* it = __global_destructor_chain; it; it = it->last)
		((void(*)(void*))it->dtor)(it->obj);
}
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

namespace Prologue {


// The first function will be the entry point.
void prologue()
{
	DebugReport("Disabling multitasking and initializing.\n");
	bool iState = OSDisableInterrupts();

	DebugReport("Initializing system!\n");
	CometSystem::initSystem();

	DebugReport("Handling patches!\n");
	//CometSystem::getSystem()->processDiscPatches();

	// Start the memory watcher (ensure patches are not overwritten).
	DebugReport("Starting memory watcher!\n");
	CometSystem::getSystem()->setupMemoryWatcher();

	DebugReport("Adding modules!\n");
	{
		CometSystem::getSystem()->mModuleLoader.appendNewModule<Reload::Reloader>();

		CometSystem::getSystem()->mModuleLoader.appendNewModule<Logging::CometLogger>();

		CometSystem::getSystem()->mModuleLoader.appendNewModule<LightCycler>();
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
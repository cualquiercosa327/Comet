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

namespace Prologue {


// The first function will be the entry point.
void prologue()
{
	DebugReport("Disabling multitasking and initializing.\n");
	bool iState = OSDisableInterrupts();

	// Initialize the system
	CometSystem::initSystem();

	// Handle our patches
	CometSystem::getSystem()->processDiscPatches();

	// Start the memory watcher (ensure patches are not overwritten).
	CometSystem::getSystem()->setupMemoryWatcher();

	// Add our modules
	{
		CometSystem::getSystem()->mModuleLoader.appendNewModule<Reload::Reloader>();

		CometSystem::getSystem()->mModuleLoader.appendNewModule<Logging::CometLogger>();
	}

	// Load modules
	CometSystem::getSystem()->mModuleLoader.loadModules();


	DebugReport("Restoring interrupt state and returning!\n");
	OSRestoreInterrupts(iState);
}

void tick()
{
	CometSystem::getSystem()->tick();

}

// Hook frame begin
//PokeyBranch(0x80009820, tick);

} // namespace Prologue
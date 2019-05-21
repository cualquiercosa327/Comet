#include "prologue.hpp"
#include "universalizer.hpp"

#include <libpokey/debug.h>
#include <libpokey/hooks.h>

//
#include <lib/GlobalsManager.hpp>
#include <lib/CometSystem.hpp>

// tick hack
#include <modules/lightCycler/cycler.hpp>

namespace Prologue {


// The first function will be the entry point.
void prologue() { POKEY_BEGIN_NO_MULTITASK

	DebugReport("\n\nEnabling universal resource support!\n\n");
	NTSCU_Region_Suffix_Process();

	GlobalsManager::resetGlobalDestructorChain();
	DebugReport("\n\Creating globals!\n\n");
	GlobalsManager::createGlobals();

	DebugReport("\n\nHandling patches!\n\n");
	CometSystem::getSystem()->processDiscPatches();

#ifdef ENABLE_MEMORY_WATCHER
	// Start the memory watcher (ensure patches are not overwritten).
	DebugReport("\n\nStarting memory watcher!\n\n");
	CometSystem::getSystem()->setupMemoryWatcher();
#endif

POKEY_END_NO_MULTITASK }

inline void tickThreadSafe() { POKEY_BEGIN_NO_MULTITASK
	
	LightCyclerTick();
	// CometSystem::getSystem()->tick();

POKEY_END_NO_MULTITASK }

inline void tickSynchronous()
{

}

void systick()
{
	tickSynchronous();
	tickThreadSafe();
}

// Hook frame begin
PokeyWritePointer(0x80270C14, systick);

// hook end of scenemgr calc
// PokeyBranch(0x8023AEA8, tick);

} // namespace Prologue






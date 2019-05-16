#include "prologue.hpp"
#include "universalizer.hpp"

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

#include "globalsManager.hpp"


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

	// Start the memory watcher (ensure patches are not overwritten).
	DebugReport("\n\nStarting memory watcher!\n\n");
	CometSystem::getSystem()->setupMemoryWatcher();

POKEY_END_NO_MULTITASK }

void tick() { POKEY_BEGIN_NO_MULTITASK
	
	LightCyclerTick();
	// CometSystem::getSystem()->tick();

POKEY_END_NO_MULTITASK }

// Hook frame begin
PokeyWritePointer(0x80270C14, tick);

// hook end of scenemgr calc
// PokeyBranch(0x8023AEA8, tick);
} // namespace Prologue






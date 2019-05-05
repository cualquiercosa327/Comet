#include "prologue.hpp"

#include <EGG/core/eggDvdRipper.hpp>
#include <EGG/core/eggExpHeap.hpp>

#include <Common/rk_types.h>

#include <libpokey/debug.h>
#include <libpokey/mkw/heaps.hpp>

#include <lib/CometSystem.hpp>

// for ctors
#include <modules/reload/reload.hpp>

namespace Prologue {


// The first function will be the entry point.
void prologue()
{
	bool iState = OSDisableInterrupts();

	// todo: unload if already loaded
	CometSystem::initSystem();

	CometSystem::getSystem()->processDiscPatches();

	CometSystem::getSystem()->mModuleLoader.appendNewModule<Reload::Reloader>();
	
	CometSystem::getSystem()->mModuleLoader.loadModules();

	DebugReport("Restoring interrupt state and returning!\n");


	OSRestoreInterrupts(iState);
}



} // namespace Prologue
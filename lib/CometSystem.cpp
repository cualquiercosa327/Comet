#include <lib/CometSystem.hpp>
#include <libpokey/mkw/heaps.hpp>

#ifdef COMET_SYSTEM_BSS
CometSystem CometSystem::sInstance;
#else
CometSystem* CometSystem::spInstance;
#endif
namespace {
bool isWatching = false;
}
CometSystem::CometSystem()
{
	DebugReport("Initializing Comet System!\n");
}

CometSystem::~CometSystem()
{
	if (isWatching)
	{
		DebugReport("Canceling memory watcher..\n");
		OSCancelAlarm(&mMemoryWatcherAlarm);
	}
}

CometSystem* CometSystem::initSystem()
{
#ifndef COMET_SYSTEM_BSS
	if (spInstance)
	{
		DebugReport("Already initialized!\n");
		return spInstance;
	}
#endif

#ifdef COMET_SYSTEM_BSS

	return &sInstance;
#else

	return (spInstance = new () CometSystem());
#endif
}
void CometSystem::processDiscPatches()
{
	// todo: keep track if loaded
	mMemoryPatcher.revertPatches();
	mMemoryPatcher.processDiscPatchFile();
}
namespace {
static void AlarmHandler(OSAlarm* alarm, OSContext* ctx)
{
#pragma unused(alarm, ctx)
	CometSystem::getSystem()->memoryScan();
}
}


void CometSystem::setupMemoryWatcher()
{
	if (isWatching)
		return;
	DebugReport("Initializing memory watcher with period of %ds.\n", MEMORY_WATCHER_PERIOD);
	OSSetPeriodicAlarm(&mMemoryWatcherAlarm, OSGetTime(), OSSecondsToTicks(MEMORY_WATCHER_PERIOD), AlarmHandler);
	isWatching = true;
}
void CometSystem::memoryScan()
{
	mMemoryPatcher.scanForChanges();
}
void CometSystem::tick()
{
	//
}
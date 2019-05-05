#include <lib/CometSystem.hpp>

CometSystem* CometSystem::spInstance;

CometSystem::CometSystem()
{}

CometSystem::~CometSystem()
{}

CometSystem* CometSystem::initSystem()
{
	if (spInstance)
	{
		DebugReport("Already initialized!");
		return spInstance;
	}
	DebugReport("Initializing Comet System!\n");
	return (spInstance = new CometSystem());
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

	CometSystem::getSystem()->mMemoryPatcher.scanForChanges();
}
}


void CometSystem::setupMemoryWatcher()
{
	DebugReport("Initializing memory watcher with period of %ds.\n", MEMORY_WATCHER_PERIOD);
	
	OSSetPeriodicAlarm(&mMemoryWatcherAlarm, OSGetTime(), OSSecondsToTicks(MEMORY_WATCHER_PERIOD), AlarmHandler);
}
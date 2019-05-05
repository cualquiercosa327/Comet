#pragma once

#include <lib/ModuleLoader.hpp>
#include <lib/MemoryPatcher.hpp>


// Check memory patches every five minutes
#define MEMORY_WATCHER_PERIOD 60 * 5

class CometSystem
{
public:
	CometSystem();
	~CometSystem();


	static CometSystem* initSystem();
	static inline CometSystem* getSystem() { return spInstance; }

	void processDiscPatches();
	void setupMemoryWatcher();

private:
	static CometSystem* spInstance;

public: // for now
	ModuleLoader mModuleLoader;
	MemoryPatcher mMemoryPatcher;

	OSAlarm mMemoryWatcherAlarm;
};
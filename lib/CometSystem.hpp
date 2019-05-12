#pragma once

#include <lib/ModuleLoader.hpp>
#include <lib/MemoryPatcher.hpp>


// Check memory patches every five minutes
#define MEMORY_WATCHER_PERIOD 60 * 5 * 17 * 20

#define COMET_SYSTEM_BSS

class CometSystem
{
public:
	CometSystem();
	~CometSystem();

	static CometSystem* initSystem();
	static inline CometSystem* getSystem()
	{
#ifdef COMET_SYSTEM_BSS
		return &sInstance;
#else
		return spInstance;
#endif
	}

	void processDiscPatches();
	void setupMemoryWatcher();

	void tick();

private:

#ifdef COMET_SYSTEM_BSS
	static CometSystem sInstance;
#else
	static CometSystem* spInstance;
#endif

public: // for now
	ModuleLoader mModuleLoader;
	MemoryPatcher mMemoryPatcher;

	OSAlarm mMemoryWatcherAlarm;
};
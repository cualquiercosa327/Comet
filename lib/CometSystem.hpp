#pragma once

#include <lib/ModuleLoader.hpp>
#include <lib/MemoryPatcher.hpp>

class CometSystem
{
public:
	CometSystem();
	~CometSystem();


	static CometSystem* initSystem();
	static inline CometSystem* getSystem() { return spInstance; }

	void processDiscPatches();

private:
	static CometSystem* spInstance;

public: // for now
	ModuleLoader mModuleLoader;
	MemoryPatcher mMemoryPatcher;

};
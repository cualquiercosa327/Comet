#pragma once

#include <lib/ModuleAccessor.hpp>
#include <vector>
#include <libpokey/debug.h>


#define NUM_MODULE_SLOTS 8

class ModuleLoader
{
public:
	ModuleLoader();
	~ModuleLoader();

	void loadModules();
	void unloadModules();
	void reloadModules();

	// Dispatch module frame hooks
	void tick();


	

	void appendModuleAccessor(const ModuleAccessor& pM)
	{
		DebugAssert(mNum < NUM_MODULE_SLOTS);
		DebugReport("Current number of modules: %u/%u\n", mNum+1, NUM_MODULE_SLOTS);
		DebugReport("src: bLoaded: %u, ptr: %p\n", pM.bLoaded, pM.mpModule);
		//mModules[++mCursor] = pM;
		mModules[mNum].bLoaded = pM.bLoaded;
		mModules[mNum].mpModule = pM.mpModule;
		DebugAssert(mModules[mNum].mpModule);
		DebugAssert(mModules[mNum].mpModule->getModuleName());
		mNum++;
	}
	// Unsafe, as we don't have a reserved heap, yet
	template<class M>
	void appendModule(M* pM)
	{
		DebugAssert(mNum < NUM_MODULE_SLOTS);
		appendModuleAccessor(ModuleAccessor(pM));
	}
	// Unsafe, as we don't have a reserved heap, yet
	template<class M>
	void appendNewModule()
	{
		DebugAssert(mNum < NUM_MODULE_SLOTS);
		appendModuleAccessor(ModuleAccessor(new M()));
	}
private:
	u16 mNum;
	ModuleAccessor mModules[NUM_MODULE_SLOTS];
	
};
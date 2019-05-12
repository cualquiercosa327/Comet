#include <lib/ModuleLoader.hpp>
#include <libpokey/debug.h>

#include <modules/lightCycler/cycler.hpp>

ModuleLoader::ModuleLoader()
{
	PokeyVerboseReport("[ModuleLoader @%p] Constructed\n", this);
	mNum = 0;
}
ModuleLoader::~ModuleLoader()
{
	PokeyVerboseReport("[ModuleLoader @%p] Destroyed\n", this);

	// todo: keep track of what is unloaded
	unloadModules();

	// ensure modules are cleaned up
	for (int i = 0; i < mNum; i++)
	{
		delete mModules[i].mpModule;
	}
}

void ModuleLoader::loadModules()
{
	PokeyDebugReport("Loading %u modules\n", mNum);
	for (int i = 0; i < mNum; i++)
	{
		IModule* pModule = mModules[i].mpModule;
		PokeyDebugReport("module: %p\n", pModule);
		
		DebugAssert(pModule->getModuleName());
		DebugAssert(pModule->getModuleVersion());


		PokeyDebugReport("Loading module %s (%s)...", pModule->getModuleName(), pModule->getModuleVersion());

		mModules[i].load();
	}
}

void ModuleLoader::unloadModules()
{
	int i = 0;
	PokeyDebugReport("unloading modules..\n");
	for (int i = 0; i < mNum; i++)
	{
		ModuleAccessor* pM = &mModules[i];
		PokeyDebugReport("Unloading module %s (%s)...", pM->mpModule->getModuleName(), pM->mpModule->getModuleVersion());
		pM->unload();
	}
}

void ModuleLoader::reloadModules()
{
	PokeyDebugReport("Reloading modules!\n");
	unloadModules();
	loadModules();
}

void ModuleLoader::tick()
{
	// major hack lmao
	LightCyclerTick();
	return;
	for (int i = 0; i < mNum; ++i)
		if (mModules[i].isLoaded())
	{
		// DebugReport("%u, %s", i, mModules[i].mpModule->getModuleName());
		mModules[i].mpModule->onFrame();
	}
}
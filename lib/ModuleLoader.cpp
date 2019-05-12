#include <lib/ModuleLoader.hpp>
#include <libpokey/debug.h>

#include <modules/lightCycler/cycler.hpp>

ModuleLoader::ModuleLoader()
{
	PokeyVerboseReport("[ModuleLoader @%p] Constructed\n", this);
}
ModuleLoader::~ModuleLoader()
{
	PokeyVerboseReport("[ModuleLoader @%p] Destroyed\n", this);

	// todo: keep track of what is unloaded
	unloadModules();

	// ensure modules are cleaned up
	for (int i = 0; i < mModules.size(); i++)
	{
		delete &mModules[i];
	}
}

void ModuleLoader::loadModules()
{
	for (std::vector<ModuleAccessor>::iterator m = mModules.begin(); m != mModules.end(); m++)
	{
		PokeyDebugReport("Loading module %s (%s)...", (*m).mpModule->getModuleName(), (*m).mpModule->getModuleVersion());

		(*m).load();
	}
}

void ModuleLoader::unloadModules()
{
	int i = 0;
	PokeyDebugReport("unloading modules..\n");
	for (int i = 0; i < mModules.size(); i++)
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
	for (int i = 0; i < mModules.size(); ++i)
		if (mModules[i].isLoaded())
	{
		// DebugReport("%u, %s", i, mModules[i].mpModule->getModuleName());
		mModules[i].mpModule->onFrame();
	}
}
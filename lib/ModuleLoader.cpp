#include <lib/ModuleLoader.hpp>
#include <libpokey/debug.h>

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
	for (std::vector<ModuleAccessor>::iterator m = mModules.begin(); m != mModules.end(); m++)
	{
		DebugReport("it: %u\n", i++);
		PokeyDebugReport("Unloading module %s (%s)...", (*m).mpModule->getModuleName(), (*m).mpModule->getModuleVersion());

		(*m).unload();
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
	DebugReport("Tick!\n");
	for (std::vector<ModuleAccessor>::iterator m = mModules.begin(); m != mModules.end(); m++)
	{
		((*m).mpModule)->onFrame();
	}
}
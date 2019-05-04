#include <lib/ModuleAccessor.hpp>

void ModuleAccessor::load()
{
	if (bLoaded)
	{
		PokeyDebugReport("Module already loaded..skipping!\n");
		return;
	}

	if (mpModule->load() == IModule::RESULT_OK)
	{
		PokeyDebugReport("Module successfully loaded!\n");
		bLoaded = true;
		return;
	}

	if (mpModule->getIsCriticalModule())
	{
		GXColor bg = { 0x00, 0x00, 0x00, 0xff };
		GXColor fg = { 0xff, 0xff, 0xff, 0xff };
		PokeyDebugReport("Critical module failed to load!");
		// todo: macro wrapper
		OSFatal(fg, bg, "Critical module failed to load!\n");
	}
	else
	{
		PokeyDebugReport("Noncritical module %s failed to load!", mpModule->getModuleName());
	}
}

void ModuleAccessor::unload()
{
	if (!bLoaded)
	{
		PokeyDebugReport("Module not loaded..skipping!\n");
		return;
	}

	if (mpModule->unload() == IModule::RESULT_OK)
	{
		PokeyDebugReport("Module successfully unloaded!\n");
		bLoaded = false;
		return;
	}
	// critical loading restriction does not apply to unloading, at the moment
	PokeyDebugReport("Failed to unload module.\n");
}
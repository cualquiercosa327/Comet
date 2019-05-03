#pragma once
#include <Common/rk_types.h>

namespace Reload {

class Reloader : public IModule
{
	~Reloader() override {}
	eResult load() override { return RESULT_OK; }
	eResult unload() override { return RESULT_OK; }

	const char* getModuleName() override { return "Reloader"; }
	const char* getModuleVersion() override { return "2.0"; }
	bool getIsCriticalModule() override { return false; }
};

void reload();

} // namespace Reload
#pragma once

#include <lib/IModule.hpp>
#include <Common/rk_types.h>

namespace Bootstrap {

class Bootstrap : public IModule
{
	~Bootstrap() override {}
	eResult load() override { return RESULT_OK; }
	eResult unload() override { return RESULT_OK; }

	const char* getModuleName() override { return "Universal bootstrap"; }
	const char* getModuleVersion() override { return "1.0"; }
	// critical for non pal discs
	bool getIsCriticalModule() override { return false; }
};

} // namespace Bootstrap
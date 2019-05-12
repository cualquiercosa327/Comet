#pragma once

#include <Common/rk_types.h>
#include <lib/IModule.hpp>

#include <libpokey/debug.h>

void LightCyclerTick();
class LightCycler : public IModule
{
public:
	~LightCycler() override {}
	eResult load() override;
	eResult unload() override
	{
		return RESULT_OK;
	}

	void onFrame() override;

	const char* getModuleName() override { return "Comet Light Cycler"; }
	const char* getModuleVersion() override { return "1.0"; }
	bool getIsCriticalModule() override { return false; }

};
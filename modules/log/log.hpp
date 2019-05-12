#pragma once

#include <Common/rk_types.h>
#include <lib/IModule.hpp>

namespace Logging {

class CometLogger : public IModule
{
public:
	~CometLogger() override {}
	eResult load() override
	{
		return RESULT_OK;
	}
	eResult unload() override
	{
		return RESULT_OK;
	}

	const char* getModuleName() override { return "Comet Logger"; }
	const char* getModuleVersion() override { return "1.0"; }
	bool getIsCriticalModule() override { return false; }

	struct LogContext
	{
		// TODO: DWC logging integration
		enum LogTypes
		{
			COMET_LOG_TYPE_LIB_REGISTER = 0x1, // eg, "<< NW4R    - SND 	final   build: Mar  8 2008 20:59:14 (0x4201_127) >>"
			COMET_LOG_TYPE_SCENE_CHANGE = 0x2, // eg, "------------------- 1 Scene Exit -------------------"
		};
		u32 mLogMask;
	};
	static LogContext sLogCtx;
};


} // namespace Reload
#pragma once

#include <Common/rk_types.h>

namespace Logging {

class CometLogger
{
public:
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
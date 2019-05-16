#include "log.hpp"
#include <libpokey/hooks.h>
#include <libpokey/debug.h>


namespace Logging {

CometLogger::LogContext CometLogger::sLogCtx;

void myRegisterVersion(const char* pStr)
{
	if (CometLogger::sLogCtx.mLogMask & CometLogger::LogContext::COMET_LOG_TYPE_LIB_REGISTER)
		OSReport("%s\n", pStr);
}

PokeyBranch(0x801A0504, myRegisterVersion);

void SceneChangeReport(const char* pStr, u32 n)
{
	if (CometLogger::sLogCtx.mLogMask & CometLogger::LogContext::COMET_LOG_TYPE_SCENE_CHANGE)
		OSReport(pStr, n);
}
PokeyCall(0x8051B354, SceneChangeReport);
PokeyCall(0x8051B854, SceneChangeReport);


} // namespace Logging
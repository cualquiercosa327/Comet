#include <lib/CometSystem.hpp>

CometSystem::CometSystem()
{}

CometSystem::~CometSystem()
{}

CometSystem* CometSystem::initSystem()
{
	if (spInstance)
	{
		DebugReport("Already initialized!");
		return;
	}

	return (spInstance = new CometSystem());
}
void CometSystem::processDiscPatches()
{
	// todo: keep track if loaded
	mMemoryPatcher.revertPatches();
	mMemoryPatcher.processDiscPatchFile();
}
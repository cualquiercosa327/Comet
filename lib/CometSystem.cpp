#include <lib/CometSystem.hpp>

CometSystem* CometSystem::spInstance;

CometSystem::CometSystem()
{}

CometSystem::~CometSystem()
{}

CometSystem* CometSystem::initSystem()
{
	if (spInstance)
	{
		DebugReport("Already initialized!");
		return spInstance;
	}
	DebugReport("Initializing Comet System!\n");
	return (spInstance = new CometSystem());
}
void CometSystem::processDiscPatches()
{
	// todo: keep track if loaded
	mMemoryPatcher.revertPatches();
	mMemoryPatcher.processDiscPatchFile();
}
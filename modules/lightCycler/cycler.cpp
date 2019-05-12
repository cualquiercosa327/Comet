#include "cycler.hpp"
#include <libpokey/hooks.h>

#include <libpokey/debug.h>

#include <revolution/pad.h>

#include <nw4r/g3d/res/g3d_resmdl_ac.h>
#include <nw4r/g3d/res/g3d_resmat_ac.h>

#include <nw4r/g3d/res/g3d_resfile_ac.h>

#include <modules/reload/reload.hpp>

PADStatus Pads[4];
int tick, cooldown;

#define GetStaticModelManager() *(void**)0x809C2F44

LightCycler::eResult LightCycler::load() override
{
	tick, cooldown = 0;


	return LightCycler::RESULT_OK;
}

void* RMModelGetCourseModelManager()
{
	void* courseModelHandle;
	// Acquire the static model manager. Controls course_model.brres, vrcorn_model.brres and course.kcl aka KDGndCol
	void* pStaticModelManager = GetStaticModelManager();
	DebugAssert(pStaticModelManager);

	// We only care about course_model.brres for now
	courseModelHandle = (void*)((char*)pStaticModelManager + 8);
	DebugAssert(courseModelHandle && **(u32**)courseModelHandle == 'bres');

	return courseModelHandle;
}
void CycleLights()
{
	void* pMan = RMModelGetCourseModelManager();

	DebugAssert(pMan);

	nw4r::g3d::ResFile* pBres = (nw4r::g3d::ResFile*)pMan;

	nw4r::g3d::ResMdl mdl = pBres->GetResMdl("course");

	nw4r::g3d::ResMdl* pMdl = &mdl;
	// DebugReport("num mat: %u\n", pMdl->GetResMatNumEntries());
	DebugAssert(pMdl->GetResMatNumEntries() < 100);

	bool has_emitted_ls = false;

	for (u32 i = 0; i < pMdl->GetResMatNumEntries(); i++)
	{
		nw4r::g3d::ResMat mat = pMdl->GetResMat(i);

		int ls = mat.ref().misc.light_set_idx;

		if (ls == -1)
			continue;


		if (ls < 16)
			ls++;
		else
			ls = 0;
		
		mat.ref().misc.light_set_idx = ls;
		if (!has_emitted_ls)
		{
			PokeyDebugReport("LSI now %u\n", ls);
			has_emitted_ls = true;
		}
	}
}


void LightCyclerTick()
{
	if (cooldown)
		cooldown--;
	if (!(++tick % 4) || cooldown)
		return;

	PADRead(Pads);

	if (Pads[0].button & PAD_BUTTON_Y)
		CycleLights();
	else if (Pads[0].button & PAD_BUTTON_X)
		Reload::reload();
	else return;

	cooldown = 30;

}

void LightCycler::onFrame()
{
	LightCyclerTick();
}
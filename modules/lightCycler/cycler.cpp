#include "cycler.hpp"
#include <libpokey/hooks.h>

#include <libpokey/debug.h>

#include <revolution/pad.h>
#include <revolution/kpad.h>

#include <nw4r/g3d/res/g3d_resmdl_ac.h>
#include <nw4r/g3d/res/g3d_resmat_ac.h>

#include <nw4r/g3d/res/g3d_resfile_ac.h>

#include <modules/reload/reload.hpp>

#include <revolution/dvd.h>

PADStatus Pads[4];
int tick, cooldown;

#define GetStaticModelManager() *(void**)0x809C2F44

struct LCYC
{
	LCYC()
	{
		DebugReport("@@@@@@@@LCYC\n");
		tick, cooldown = 0;
	}
};

LCYC _LCYC;

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
	// DebugReport("cycleLights\n");
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

		if (ls < 15)
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
	{
		cooldown--;
		return;
	}

	if (!(++tick % 4))
		return;

	PADRead(Pads);

	//	KPADStatus kStat;
	//	
	//	KPADRead(0, &kStat, 1);
	//	
	//	if (kStat.trig & KPAD_BUTTON_HOME)
	//		CycleLights();

	if (Pads[0].button & PAD_BUTTON_Y)
		CycleLights();
#ifdef DEBUG
	else if (Pads[0].button & PAD_BUTTON_X)
		Reload::reload();
#endif
	else return;

	cooldown = 30;

}

#include "cycler.hpp"
#include <libpokey/hooks.h>

#include <libpokey/debug.h>

#include <revolution/pad.h>

#include <nw4r/g3d/res/g3d_resmdl_ac.h>
#include <nw4r/g3d/res/g3d_resmat_ac.h>

#include <nw4r/g3d/res/g3d_resfile_ac.h>

#include <modules/reload/reload.hpp>

#include <revolution/dvd.h>

PADStatus Pads[4];
int tick, cooldown;

#define GetStaticModelManager() *(void**)0x809C2F44

LightCycler::eResult LightCycler::load() override
{
	tick, cooldown = 0;

	NTSCU_Region_Suffix_Process();

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
	{
		cooldown--;
		return;
	}

	if (!(++tick % 4))
		return;
	PADRead(Pads);

	if (Pads[0].button & PAD_BUTTON_Y)
		CycleLights();
#ifdef DEBUG
	else if (Pads[0].button & PAD_BUTTON_X)
		Reload::reload();
#endif
	else return;

	cooldown = 30;

}

void LightCycler::onFrame()
{
	LightCyclerTick();
}

// This alone will allow JPN to work.
// Additional code is needed for NTSC-U/PAL region overlap (e.g., _E and _U)
//	const char* loaderRegionSuffixes[] = {
//		"_J.szs",	// Japanese
//		"_E.szs",	// English (NTSC-U: _U)
//		"_G.szs",	// German
//		"_F.szs",	// French (NTSC-U: _Q)
//		"_S.szs",	// Spanish (NTSC-U: _M)
//		"_I.szs",	// Italian
//		"_N.szs"	// Dutch
//	};
const char* jpn_suffix = "_J.szs";

PokeyWritePointer(0x808B3188, &jpn_suffix);

const char* eng_us_suffix = "_U.szs";
const char* french_qu_suffix = "_Q.szs";
const char* spanish_mx_suffix = "_M.szs";

// Really nasty hack to switch to US mode if one file is detected.
// This isn't actually universal


inline void Set_NTSCU_Region_Suffixes()
{
	DebugReport("Setting NTSC-U region suffixeds..\n");
	*(u32*)0x808B318C = *(u32*)&eng_us_suffix;
	*(u32*)0x808B3194 = *(u32*)&french_qu_suffix;
	*(u32*)0x808B3198 = *(u32*)&spanish_mx_suffix;
}

// We can't rely on the disc ID
inline bool isNTSCU()
{
	return DVDConvertPathToEntrynum("/Scene/UI/MenuSingle_U.szs") != -1;
}

void NTSCU_Region_Suffix_Process()
{
	if (isNTSCU())
		Set_NTSCU_Region_Suffixes();
}

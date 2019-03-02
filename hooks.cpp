#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/ObScript.h"
#include "skse64/GameExtraData.h"
#include "inifile.h"
#include "hooks.h"

UInt32 g_factionKeywordID = 0x0802;

TESObjectREFR* targetRefr = nullptr;
int currentMultiTapCount = 1;

bool IsSneaking(Actor* actor)
{
	return (actor && ((actor->actorState.flags04 & ActorState::kState_Sneaking) != 0));
}

bool IsHorseRace(TESObjectREFR* refr)
{
	if (!refr || !refr->baseForm)
		return false;

	static UInt32 horseRaceFormID = 0x00131FD;
	TESNPC* actorBase = DYNAMIC_CAST(refr->baseForm, TESForm, TESNPC);
	return (actorBase) ? ((actorBase->race.race)->formID == horseRaceFormID) : false;
}

bool IsStealObject(TESObjectREFR * refr)
{
	bool result = false;
	if (refr->baseForm->formType == kFormType_NPC && !IsHorseRace(refr))
		result = false;
	else if (refr->baseForm->formType == kFormType_NPC && IsHorseRace(refr))
		result = CALL_MEMBER_FN(refr, IsOffLimits)();
	else if (refr->formType == kFormType_Reference)
		result = CALL_MEMBER_FN(refr, IsOffLimits)();

	return result;
}

FnEval fn_org;
bool hook_fn(TESObjectREFR * thisObj, void * arg1, void * arg2, double& result)
{
#ifdef _DEBUG
	_MESSAGE("hook_fn");
#endif

	INIFile* iniFile = INIFile::GetSingleton();
	TESFaction* faction = static_cast<TESFaction*>(arg1);
	result = 0.0;

	if (!faction || faction->formID != g_factionKeywordID)
		return fn_org(thisObj, arg1, arg2, result);

	if (!iniFile)
		return false;
	else if (!thisObj)
		return false;
	else if (!thisObj->GetNiNode())
		return false;
	if (thisObj->formType == kFormType_Character && !IsHorseRace(thisObj))
		return false;
	if (!iniFile->IsInList(thisObj->baseForm->formType))
		return false;
	if (iniFile->enableSneak && IsSneaking(*g_thePlayer))
		return false;
	if (!IsStealObject(thisObj))
		return false;

	if (iniFile->enableMultiTap)
	{
		if (targetRefr == thisObj && currentMultiTapCount >= iniFile->multiTapCount)
		{
			targetRefr = nullptr;
			return false;
		}
	}

	targetRefr = thisObj;
	result = 1.0;

#ifdef _DEBUG
	_MESSAGE("thisObj_ID:%08X result:%0.1f  currentMultiTapCount:%d", thisObj->formID, result, currentMultiTapCount);
#endif

	return true;
}

void hooks::init()
{
#ifdef _DEBUG
	_MESSAGE("init()");
#endif
	DataHandler* dhnd = DataHandler::GetSingleton();

	const std::string modName = "BlockStealSE.esp";

	SInt32 modIndex = -1;

	//if (dhnd->GetModIndex(modName.c_str()) == -1)
	//	return;

	modIndex = dhnd->GetLoadedModIndex(modName.c_str());
	if (modIndex == -1)
	{
		//esl
		modIndex = dhnd->GetLoadedLightModIndex(modName.c_str());
		if (modIndex == -1)
			return;

		g_factionKeywordID |= 0xFE000000;
		g_factionKeywordID |= modIndex << 3;
	}
	else
	{
		//esp.esm
		g_factionKeywordID |= (modIndex << 24);
	}

	//check keyword id
	if (!LookupFormByID(g_factionKeywordID))
		return;

#ifdef _DEBUG
	_MESSAGE("g_factionKeywordID %08X", g_factionKeywordID);
#endif

	for (ObScriptCommand * iter = g_firstObScriptCommand; iter->opcode < kObScript_NumObScriptCommands + kObScript_ScriptOpBase; ++iter)
	{
		if (std::strcmp(iter->longName, "CanPayCrimeGold") == 0)
		{
#ifdef _DEBUG
			_MESSAGE("Hook CanPayCrimeGold function");
#endif
			fn_org = iter->eval;
			iter->eval = hook_fn;
			break;
		}
	}
}


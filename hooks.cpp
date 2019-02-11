#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/ObScript.h"
#include "skse64/GameExtraData.h"
#include "inifile.h"
#include "hooks.h"

UInt32 g_factionKeywordID = 0x0802;
UInt32 g_horseRaceFormID = 0x00131FD;

bool IsSneaking(Actor* actor)
{
	return (actor && ((actor->actorState.flags04 & ActorState::kState_Sneaking) != 0));
}

bool IsHorseRace(TESObjectREFR* refr)
{
	if (!refr || !refr->baseForm)
		return false;
	TESNPC* actorBase = DYNAMIC_CAST(refr->baseForm, TESForm, TESNPC);
	return (actorBase) ? ((actorBase->race.race)->formID == g_horseRaceFormID) : false;
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


bool IsQuestItem(BaseExtraList* extraList)
{
	if (!extraList)
		return false;
	ExtraAliasInstanceArray* exAliasArray = static_cast<ExtraAliasInstanceArray*>(extraList->GetByType(kExtraData_AliasInstanceArray));
	if (!exAliasArray)
		return false;

	for (int i = 0; i < exAliasArray->aliases.count; i++)
	{
		ExtraAliasInstanceArray::AliasInfo * aliasInfo = nullptr;
		if (exAliasArray->aliases.GetNthItem(i, aliasInfo))
		{
			//if (!aliasInfo)
			//	continue;
			BGSBaseAlias* baseAlias = aliasInfo->alias;
			if (!baseAlias)
				continue;
			if (!((baseAlias->flags >> 2) & 1))
				continue;

			return true;
		}
	}
	return false;
}

bool IsQuestItem(TESObjectREFR* refr)
{
	if (!refr)
		return false;

	TESObjectREFR* s_ref = refr;
	ExtraReferenceHandle* refHandle = static_cast<ExtraReferenceHandle*>(refr->extraData.GetByType(kExtraData_ReferenceHandle));
	if (refHandle)
		LookupREFRByHandle(&refHandle->handle, &s_ref);

	BaseExtraList* extraList = nullptr;
	if (s_ref)
		extraList = &s_ref->extraData;
	return IsQuestItem(extraList);
}

TESObjectREFR* targetRefr = nullptr;
int currentMultiTapCount = 0;

FnEval fn_org;
bool hook_fn(TESObjectREFR * thisObj, void * arg1, void * arg2, double& result)
{
#ifdef _DEBUG
	_MESSAGE("hook_fn");
#endif

	INIFile* iniFile = INIFile::GetSingleton();
	TESFaction* faction = static_cast<TESFaction*>(arg1);

	if (!faction || faction->formID != g_factionKeywordID)
		return fn_org(thisObj, arg1, arg2, result);

	if (!iniFile)
		return true;

	if (!thisObj || !thisObj->GetNiNode())
		return true;

	Actor* actor = DYNAMIC_CAST(thisObj, TESObjectREFR, Actor);
	if (actor && !IsHorseRace(thisObj))
	{
#ifdef _DEBUG
		_MESSAGE("* actor");
#endif
		return true;
	}

result = 0.0;

	if (!iniFile->IsInList(thisObj->baseForm->formType))
	{
#ifdef _DEBUG
		_MESSAGE("* !IsInList");
#endif
		return true;
	}

	if (iniFile->disableForQuestItem)
	{
		TESNPC* actorBase = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESNPC);
		bool isQuestObject = actorBase ? false : IsQuestItem(thisObj);
		if (isQuestObject)
		{
#ifdef _DEBUG
			_MESSAGE("* isQuestObject:yes");
#endif
			return true;
		}
	}

	if (iniFile->enableSneak && IsSneaking(*g_thePlayer))
	{
#ifdef _DEBUG
		_MESSAGE("* sneakSetting:yes & isSneaking:yes");
#endif
		return true;
	}

	if (iniFile->enableMultiTap && currentMultiTapCount >= iniFile->multiTapCount)
	{
#ifdef _DEBUG
		_MESSAGE("* currentMultiTapCount >= multiTapCount");
#endif
		targetRefr = nullptr;

		return true;
	}

	bool isStealObject = IsStealObject(thisObj);
	if (!isStealObject)
	{
#ifdef _DEBUG
		_MESSAGE("* !isStealObject");
#endif
		return true;
	}

	if (targetRefr != thisObj)
	{
#ifdef _DEBUG
		_MESSAGE("targetRefr != thisObj");
#endif
		currentMultiTapCount = 0;
	}

	targetRefr = thisObj;
	result = 1.0;


#ifdef _DEBUG
	_MESSAGE("result %0.1f", result);
#endif

	return true;

	//return fn_org(thisObj, arg1, arg2, result);
}

void hooks::init()
{
#ifdef _DEBUG
	_MESSAGE("init()");
#endif
	DataHandler* dhnd = DataHandler::GetSingleton();
	const ModInfo* modInfo = dhnd->LookupModByName("BlockStealSE.esp");
	if (!modInfo)
		return;

	UInt32 modIndex = modInfo->modIndex;
	g_factionKeywordID |= (modIndex << 24);

	for (ObScriptCommand * iter = g_firstObScriptCommand; iter->opcode < kObScript_NumObScriptCommands + kObScript_ScriptOpBase; ++iter)
	{
		if (std::strcmp(iter->longName, "CanPayCrimeGold") == 0)
		{
#ifdef _DEBUG
			_MESSAGE("CanPayCrimeGold hit!");
#endif
			fn_org = iter->eval;
			iter->eval = hook_fn;
			break;
		}
	}
}


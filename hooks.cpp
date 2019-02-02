#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/ObScript.h"
#include "skse64/GameExtraData.h"
#include "inifile.h"
#include "hooks.h"

UInt32 g_factionKeywordID = 0x0802;
UInt32 g_horseRaceFormID = 0x00131FD;

bool IsHorseRace(TESObjectREFR* refr)
{
	if (!refr || !refr->baseForm)
		return false;
	TESNPC* actorBase = DYNAMIC_CAST(refr->baseForm, TESForm, TESNPC);
	return (actorBase) ? ((actorBase->race.race)->formID == g_horseRaceFormID) : false;
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



FnEval fn_org;
bool hook_fn(TESObjectREFR * thisObj, void * arg1, void * arg2, double& result)
{
#ifdef _DEBUG
	_MESSAGE("hook_fn");
#endif

	INIFile* iniFile = INIFile::GetSingleton();
	TESFaction* faction = static_cast<TESFaction*>(arg1);
	if (iniFile && faction && faction->formID == g_factionKeywordID)
	{
		if (thisObj && thisObj->GetNiNode())
		{
			result = 0.0;
			bool isStealObject = false;
			bool isQuestObject = false;

			if (iniFile->IsInList(thisObj->baseForm->formType))
			{
				if (thisObj->baseForm->formType == kFormType_NPC && !IsHorseRace(thisObj))
					isStealObject = false;
				else if (thisObj->baseForm->formType == kFormType_NPC && IsHorseRace(thisObj))
					isStealObject = CALL_MEMBER_FN(thisObj, IsOffLimits)();
				else if (thisObj->formType == kFormType_Reference)
					isStealObject = CALL_MEMBER_FN(thisObj, IsOffLimits)();
			}

			if (iniFile->disableForQuestItem)
			{
				TESNPC* actorBase = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESNPC);
				isQuestObject = actorBase ? false : IsQuestItem(thisObj);
			}

			if (isStealObject && !isQuestObject)
				result = 1.0;

			return true;
		}
	}

	return fn_org(thisObj, arg1, arg2, result);
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


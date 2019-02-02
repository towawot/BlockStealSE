#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "papyrus.h"
#include "inifile.h"

namespace papyrus
{
	void DebugTrace(StaticFunctionTag* base, BSFixedString s) { _MESSAGE("%s", s); }
	float GetMultiTapTimer(StaticFunctionTag* base) { return INIFile::GetSingleton()->multiTapTimer; }
	SInt32 GetMultiTapCount(StaticFunctionTag* base) { return INIFile::GetSingleton()->multiTapCount; }
	bool IsEnableMultiTapSetting(StaticFunctionTag* base) { return INIFile::GetSingleton()->enableMultiTap; }
	bool IsEnableSneakSetting(StaticFunctionTag* base) { return INIFile::GetSingleton()->enableSneak; }
}

void papyrus::RegisterFuncs(VMClassRegistry* registry)
{
#ifdef _DEBUG
	_MESSAGE("papyrus::RegisterFuncs");
#endif

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, float>("GetMultiTapTimer", "BlockStealSE", papyrus::GetMultiTapTimer, registry));

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, SInt32>("GetMultiTapCount", "BlockStealSE", papyrus::GetMultiTapCount, registry));

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, bool>("IsEnableMultiTapSetting", "BlockStealSE", papyrus::IsEnableMultiTapSetting, registry));

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, bool>("IsEnableSneakSetting", "BlockStealSE", papyrus::IsEnableSneakSetting, registry));

#ifdef _DEBUG
	_MESSAGE("papyrus::RegisterFuncs");
#endif

}

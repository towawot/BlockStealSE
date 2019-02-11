#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "papyrus.h"
#include "inifile.h"
#include "hooks.h"

namespace papyrus
{
	void IncCurrentMultiTapCount(StaticFunctionTag* base) { currentMultiTapCount += 1; }
	void ResetCurrentMultiTapCount(StaticFunctionTag* base) { currentMultiTapCount = 1; }
	float GetMultiTapTimer(StaticFunctionTag* base) { return INIFile::GetSingleton()->multiTapTimer; }
}

void papyrus::RegisterFuncs(VMClassRegistry* registry)
{
#ifdef _DEBUG
	_MESSAGE("papyrus::RegisterFuncs");
#endif

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("IncCurrentMultiTapCount", "BlockStealSE", papyrus::IncCurrentMultiTapCount, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ResetCurrentMultiTapCount", "BlockStealSE", papyrus::ResetCurrentMultiTapCount, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, float>("GetMultiTapTimer", "BlockStealSE", papyrus::GetMultiTapTimer, registry));
}

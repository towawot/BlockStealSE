#pragma once

#include <vector>

#define INI_FILE "data\\SKSE\\Plugins\\BlockstealSE.ini"

class INIFile
{
private:
	static INIFile* s_pInstance;

	std::string ToLowerStr(std::string str);
	INIFile(void);

public:
	bool enableSneak;
	bool enableMultiTap;
	UInt32 multiTapCount;
	double multiTapTimer;
	std::vector<UInt32> onjectTypeList;

	bool IsInList(UInt32 num);

	static INIFile* GetSingleton(void)
	{
		if(s_pInstance == nullptr)
			s_pInstance = new INIFile();
		return s_pInstance;
	}
	static void DeleteInstance(void)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}
};

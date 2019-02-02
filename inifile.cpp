#include <skse64/GameForms.h> // kFormTypes
#include <string>
#include "inifile.h"

INIFile* INIFile::s_pInstance = nullptr;

INIFile::INIFile()
{
	char buf[256];
	std::string result;

// 	General section
	GetPrivateProfileString("General", "EnableSneak", "false", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	enableSneak = (result == "true");

	GetPrivateProfileString("General", "EnableMultiTap", "false", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	enableMultiTap = (result == "true");

	GetPrivateProfileString("General", "MultiTapCount", "2", buf, sizeof(buf), INI_FILE);
	multiTapCount = std::stoul(buf, nullptr, 10);
	
	GetPrivateProfileString("General", "MultiTapTimer", "0.3", buf, sizeof(buf), INI_FILE);
	multiTapTimer = std::stof(buf);

	GetPrivateProfileString("General", "DisableForQuestItem", "false", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	disableForQuestItem = (result == "true");

// 	Object section
	GetPrivateProfileString("Object", "Armor", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Armor);

	GetPrivateProfileString("Object", "Book", "false", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Book);

	GetPrivateProfileString("Object", "Ingredient", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Ingredient);

	GetPrivateProfileString("Object", "Scroll", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_ScrollItem);

	GetPrivateProfileString("Object", "SoulGem", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_SoulGem);

	GetPrivateProfileString("Object", "Misc", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Misc);

	GetPrivateProfileString("Object", "Tree", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Tree);

	GetPrivateProfileString("Object", "Flora", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Flora);

	GetPrivateProfileString("Object", "Ammo", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Ammo);

	GetPrivateProfileString("Object", "Weapon", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Weapon);
	
	GetPrivateProfileString("Object", "Horse", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_NPC);

	GetPrivateProfileString("Object", "Key", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Key);

	GetPrivateProfileString("Object", "Potion", "true", buf, sizeof(buf), INI_FILE);
	result = ToLowerStr(buf);
	if (result == "true")
		onjectTypeList.push_back(kFormType_Potion);
}

#include <algorithm>
std::string INIFile::ToLowerStr(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

bool INIFile::IsInList(UInt32 type)
{
	if (std::find(onjectTypeList.begin(), onjectTypeList.end(), type) != onjectTypeList.end())
		return true;

	return false;
}

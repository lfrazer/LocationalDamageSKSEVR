#include <skse64/GameRTTI.h>
//#include <common/DebugLog.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <shlwapi.h> // PathRemoveFileSpec
#pragma comment( lib, "shlwapi.lib" )
#include "iniSettings.h"
#include <algorithm>

#include <skse64/GameData.h>

INIFile ini;

INIFile::INIFile() : EnableHead(true), EnableArms(true), EnableFoot(true), EnableHeart(true), DamageTypeHead(1), DamageTypeArms(2), DamageTypeFoot(4), DamageTypeHeart(1), EffectTypeHead(0),
EffectTypeArms(1), EffectTypeFoot(4), EffectTypeHeart(2), InterruptBaseChance(30), StaggerBaseChance(30), KnockdownBaseChance(30), UnequipArmorBaseChance(30), UnequipWeaponBaseChance(30),
DisplayImpactEffect(true), DisplayNotification(true), HeadMessageFront("Head: "), HeadMessageBack(""), ArmsMessageFront("Arm: "), ArmsMessageBack(""), FootMessageFront("Feet: "),
FootMessageBack(""), HeartMessageFront("Heart: "), HeartMessageBack(""), HeavyArmorDamageMultiplier(0.3), LightArmorDamageMultiplier(0.6), HeavyArmorEffectChanceMultiplier(0.3),
LightArmorEffectChanceMultiplier(0.6)
{
	GeneralMap["DamageTypeHead"] = 1;
	GeneralMap["DamageTypeArms"] = 2;
	GeneralMap["DamageTypeFoot"] = 4;
	GeneralMap["DamageTypeHeart"] = 1;
	GeneralMap["EffectTypeHead"] = 0;
	GeneralMap["EffectTypeArms"] = 1;
	GeneralMap["EffectTypeFoot"] = 4;
	GeneralMap["EffectTypeHeart"] = 2;

	GeneralMap["InterruptBaseChance"] = 30;
	GeneralMap["StaggerBaseChance"] = 30;
	GeneralMap["KnockdownBaseChance"] = 30;
	GeneralMap["UnequipArmorBaseChance"] = 30;
	GeneralMap["UnequipWeaponBaseChance"] = 30;

	GeneralMap["DisplayImpactEffect"] = 1;
	GeneralMap["DisplayNotification"] = 1;
	GeneralMap["DisplayNotificationMinDamage"] = 1;

	GeneralMap["SpellDamageMultiplier"] = 1;

	MessageMap["HeadMessageFront"] = "Head: ";
	MessageMap["HeadMessageBack"] = "";
	MessageMap["ArmsMessageFront"] = "Arm: ";
	MessageMap["ArmsMessageBack"] = "";
	MessageMap["FootMessageFront"] = "Feet: ";
	MessageMap["FootMessageBack"] = "";
	MessageMap["HeartMessageFront"] = "Heart: ";
	MessageMap["HeartMessageBack"] = "";

	ArmorMap["HeavyArmorDamageMultiplier"] = 0.3;
	ArmorMap["LightArmorDamageMultiplier"] = 0.6;
	ArmorMap["HeavyArmorEffectChanceMultiplier"] = 0.3;
	ArmorMap["LightArmorEffectChanceMultiplier"] = 0.6;

	for (int i = 0; i < Type_Num; i++)
	{
		PlayerToNPC[i] = 1.0;
		NPCToPlayer[i] = 1.0;
		NPCToNPC[i] = 1.0;
	}
}

void INIFile::Load()
{
	// default impact data set
	const ModInfo* DGModInfo = DataHandler::GetSingleton()->LookupModByName("Dawnguard.esm");
	if (DGModInfo)
	{
		UInt32 dawnguardModIndex = DGModInfo->GetPartialIndex();
		ImpactEffectFormID = (dawnguardModIndex << 24) | DEFAULT_IMPACTEFFECT_FORMID; // default to vampire drain VFX
	}

	std::string iniFilePath = GetINIFile();
	bool fileFound = IsFoundFile(iniFilePath.c_str());

	_MESSAGE("Looking for INI @ %s, file found: %d\n", iniFilePath.c_str(), (int)fileFound);

	SetSettings();
	ShowSettings();

	EnableHead = DamageTypeHead != 0 || EffectTypeHead != 0;
	EnableArms = DamageTypeArms != 0 || EffectTypeArms != 0;
	EnableFoot = DamageTypeFoot != 0 || EffectTypeFoot != 0;
	EnableHeart = DamageTypeHeart != 0 || EffectTypeHeart != 0;

	GeneralMap.clear();
	MessageMap.clear();
	ArmorMap.clear();
}

void INIFile::ToLower(std::string &str)
{
	for (auto &c : str)
		c = tolower(c);
}

std::string INIFile::GetSkyrimPath()
{
	std::string result;
	char buf[MAX_PATH];
	GetModuleFileNameA(nullptr, buf, MAX_PATH);
	PathRemoveFileSpecA(buf);
	result = buf;
	result += "\\";
	return result;
}

std::string INIFile::GetSksePluginPath()
{
	std::string result = GetSkyrimPath();
	result += "data\\SKSE\\Plugins\\";
	return result;
}

bool INIFile::IsFoundFile(const char* fileName)
{
	std::ifstream ifs(fileName);
	return (ifs.fail()) ? false : true;
}

std::vector<std::string> INIFile::GetSectionKeys(const char* section_name, const char* ini_file_path)
{
	std::vector<std::string> result;
	std::string file_path(ini_file_path);
	if (IsFoundFile(ini_file_path))
	{
		char buf[32768] = {};
		GetPrivateProfileSectionA(section_name, buf, sizeof(buf), ini_file_path);
		for (char* seek = buf; *seek != '\0'; seek++)
		{
			std::string str(seek);
			result.push_back(str);
			while (*seek != '\0')
				seek++;
		}
	}
	return result;
}

std::vector<std::string> INIFile::Split(const std::string &str, char sep)
{
	std::vector<std::string> result;

	auto first = str.begin();
	while (first != str.end())
	{
		auto last = first;
		while (last != str.end() && *last != sep)
			++last;
		result.push_back(std::string(first, last));
		if (last != str.end())
			++last;
		first = last;
	}
	return result;
}

void INIFile::EraseIf(std::string &str, std::string sep)
{
	size_t c;
	while ((c = str.find_first_of(sep)) != std::string::npos)
		str.erase(c, 1);
}

void INIFile::SetSettings()
{
	static std::string section1 = "General";
	std::vector<std::string> list1 = GetSectionKeys(section1.c_str(), GetINIFile().c_str());
	std::sort(list1.begin(), list1.end());
	SetINIData(&list1);

	static std::string section2 = "Armor";
	std::vector<std::string> list2 = GetSectionKeys(section2.c_str(), GetINIFile().c_str());
	std::sort(list2.begin(), list2.end());
	SetINIDataArmor(&list2);

	static std::string section3 = "PlayerToNPC";
	std::vector<std::string> list3 = GetSectionKeys(section3.c_str(), GetINIFile().c_str());
	std::sort(list3.begin(), list3.end());
	SetINIDataDouble(&list3, PlayerToNPC);

	static std::string section4 = "NPCToPlayer";
	std::vector<std::string> list4 = GetSectionKeys(section4.c_str(), GetINIFile().c_str());
	std::sort(list4.begin(), list4.end());
	SetINIDataDouble(&list4, NPCToPlayer);

	static std::string section5 = "NPCToNPC";
	std::vector<std::string> list5 = GetSectionKeys(section5.c_str(), GetINIFile().c_str());
	std::sort(list5.begin(), list5.end());
	SetINIDataDouble(&list5, NPCToNPC);
}

std::string INIFile::GetINIFile()
{
	if (iniFilePath.empty())
	{
		iniFilePath = GetSksePluginPath();
		iniFilePath += INI_FILE;
	}
	return iniFilePath;
}

void INIFile::SetINIData(std::vector<std::string> *list)
{
	for (std::string str : *list)
	{
		if (str.empty())
			continue;
		auto vec = Split(str, '=');
		if (vec.size() != 2)
			continue;
		std::string key = vec.at(0);

		if (!key.empty())
		{
			if (key == "DisplayImpactEffect")
			{
				std::string value = vec.at(1);
				ToLower(value);
				if (value == "false")
				{
					GeneralMap.at("DisplayImpactEffect") = 0;
					DisplayImpactEffect = false;
				}
			}
			else if (key == "DisplayNotification")
			{
				std::string value = vec.at(1);
				ToLower(value);
				if (value == "false")
				{
					GeneralMap.at("DisplayNotification") = 0;
					DisplayNotification = false;
				}
			}
			else if (key == "PlaySoundEffect")
			{
				std::string value = vec.at(1);
				if (value == "true")
				{
					this->PlaySoundEffect = true;
				}
				if (value == "false")
				{
					this->PlaySoundEffect = false;
				}

			}
			else if (key == "PlaySoundAtEnemyLocation")
			{
				std::string value = vec.at(1);
				if (value == "true")
				{
					this->PlaySoundAtEnemyLocation = true;
				}
				if (value == "false")
				{
					this->PlaySoundAtEnemyLocation = false;
				}
			}
			else if (key == "DisplayNotificationMinDamage")
			{
				int value = std::atoi(vec.at(1).c_str());
				GeneralMap.at("DisplayNotificationMinDamage") = value;
				DisplayNotificationMinDamage = value;
			}
			else if (key == "UseSKSETrampolineInterface")
			{
				int value = std::atoi(vec.at(1).c_str());
				GeneralMap.at("UseSKSETrampolineInterface") = value;
				UseSKSETrampolineInterface = value;
			}
			else if (key == "HeadMessageFront")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("HeadMessageFront") = HeadMessageFront = value;
			}
			else if (key == "HeadMessageBack")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("HeadMessageBack") = HeadMessageBack = value;
			}
			else if (key == "ArmsMessageFront")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("ArmsMessageFront") = ArmsMessageFront = value;
			}
			else if (key == "ArmsMessageBack")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("ArmsMessageBack") = ArmsMessageBack = value;
			}
			else if (key == "FootMessageFront")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("FootMessageFront") = FootMessageFront = value;
			}
			else if (key == "FootMessageBack")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("FootMessageBack") = FootMessageBack = value;
			}
			else if (key == "HeartMessageFront")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("HeartMessageFront") = HeartMessageFront = value;
			}
			else if (key == "HeartMessageBack")
			{
				std::string value = vec.at(1);
				EraseIf(value, "\"");
				MessageMap.at("HeartMessageBack") = HeartMessageBack = value;
			}
			else if (key == "SpellDamageMultiplier")
			{
				double value = std::atof(vec.at(1).c_str());
				SpellDamageMultiplier = value;
				GeneralMap.at("SpellDamageMultiplier") = (UInt32)(value * 100.0);
			}
			else if (key == "ImpactEffectFormID")
			{
				this->ImpactEffectFormID = std::strtoul(vec.at(1).c_str(), 0, 16);
			}
			else if (key == "SoundEffectFormID")
			{
				this->SoundEffectFormID = std::strtoul(vec.at(1).c_str(), 0, 16);
			}
			else if (key == "SoundEffectSpellFormID")
			{
				this->SoundEffectSpellFormID = std::strtoul(vec.at(1).c_str(), 0, 16);
			}
			else
			{
				UInt32 value = std::atoi(vec.at(1).c_str());

				if (key == "DamageTypeHead")
				{
					GeneralMap.at("DamageTypeHead") = DamageTypeHead = value;
				}
				else if (key == "DamageTypeArms")
				{
					GeneralMap.at("DamageTypeArms") = DamageTypeArms = value;
				}
				else if (key == "DamageTypeFoot")
				{
					GeneralMap.at("DamageTypeFoot") = DamageTypeFoot = value;
				}
				else if (key == "DamageTypeHeart")
				{
					GeneralMap.at("DamageTypeHeart") = DamageTypeHeart = value;
				}
				else if (key == "EffectTypeHead")
				{
					GeneralMap.at("EffectTypeHead") = EffectTypeHead = value;
				}
				else if (key == "EffectTypeArms")
				{
					GeneralMap.at("EffectTypeArms") = EffectTypeArms = value;
				}
				else if (key == "EffectTypeFoot")
				{
					GeneralMap.at("EffectTypeFoot") = EffectTypeFoot = value;
				}
				else if (key == "EffectTypeHeart")
				{
					GeneralMap.at("EffectTypeHeart") = EffectTypeHeart = value;
				}
				else if (key == "InterruptBaseChance")
				{
					GeneralMap.at("InterruptBaseChance") = InterruptBaseChance = value;
				}
				else if (key == "StaggerBaseChance")
				{
					GeneralMap.at("StaggerBaseChance") = StaggerBaseChance = value;
				}
				else if (key == "KnockdownBaseChance")
				{
					GeneralMap.at("KnockdownBaseChance") = KnockdownBaseChance = value;
				}
				else if (key == "UnequipArmorBaseChance")
				{
					GeneralMap.at("UnequipArmorBaseChance") = UnequipArmorBaseChance = value;
				}
				else if (key == "UnequipWeaponBaseChance")
				{
					GeneralMap.at("UnequipWeaponBaseChance") = UnequipWeaponBaseChance = value;
				}
			}
		}
	}
}

void INIFile::SetINIDataArmor(std::vector<std::string> *list)
{
	for (std::string str : *list)
	{
		if (str.empty())
			continue;
		auto vec = Split(str, '=');
		if (vec.size() != 2)
			continue;
		std::string key = vec.at(0);

		if (!key.empty())
		{
			double value = std::atof(vec.at(1).c_str());

			if (key == "HeavyArmorDamageMultiplier")
			{
				if (value >= 0.0)
					ArmorMap.at("HeavyArmorDamageMultiplier") = HeavyArmorDamageMultiplier = value;
			}
			else if (key == "LightArmorDamageMultiplier")
			{
				if (value >= 0.0)
					ArmorMap.at("LightArmorDamageMultiplier") = LightArmorDamageMultiplier = value;
			}
			else if (key == "HeavyArmorEffectChanceMultiplier")
			{
				if (value >= 0.0)
					ArmorMap.at("HeavyArmorEffectChanceMultiplier") = HeavyArmorEffectChanceMultiplier = value;
			}
			else if (key == "LightArmorEffectChanceMultiplier")
			{
				if (value >= 0.0)
					ArmorMap.at("LightArmorEffectChanceMultiplier") = LightArmorEffectChanceMultiplier = value;
			}
		}
	}
}

void INIFile::SetINIDataDouble(std::vector<std::string> *list, double *setting)
{
	for (std::string str : *list)
	{
		if (str.empty())
			continue;
		auto vec = Split(str, '=');
		if (vec.size() != 2)
			continue;
		std::string key = vec.at(0);

		if (!key.empty())
		{
			double value = std::atof(vec.at(1).c_str());

			if (key == "HeadDamageMultiplier")
			{
				if (value >= 0.0)
					setting[Type_HeadDamageMultiplier] = value;
			}
			else if (key == "ArmsDamageMultiplier")
			{
				if (value >= 0.0)
					setting[Type_ArmsDamageMultiplier] = value;
			}
			else if (key == "FootDamageMultiplier")
			{
				if (value >= 0.0)
					setting[Type_FootDamageMultiplier] = value;
			}
			else if (key == "HeartDamageMultiplier")
			{
				if (value >= 0.0)
					setting[Type_HeartDamageMultiplier] = value;
			}
			else if (key == "HeadEffectChanceMultiplier")
			{
				if (value >= 0.0)
					setting[Type_HeadEffectChanceMultiplier] = value;
			}
			else if (key == "ArmsEffectChanceMultiplier")
			{
				if (value >= 0.0)
					setting[Type_ArmsEffectChanceMultiplier] = value;
			}
			else if (key == "FootEffectChanceMultiplier")
			{
				if (value >= 0.0)
					setting[Type_FootEffectChanceMultiplier] = value;
			}
			else if (key == "HeartEffectChanceMultiplier")
			{
				if (value >= 0.0)
					setting[Type_HeartEffectChanceMultiplier] = value;
			}
		}
	}
}

std::string INIFile::GetMultiplierString(MultiplierType type)
{
	switch (type)
	{
	case Type_HeadDamageMultiplier:
		return "HeadDamageMultiplier";
	case Type_ArmsDamageMultiplier:
		return "ArmsDamageMultiplier";
	case Type_FootDamageMultiplier:
		return "FootDamageMultiplier";
	case Type_HeartDamageMultiplier:
		return "HeartDamageMultiplier";
	case Type_HeadEffectChanceMultiplier:
		return "HeadEffectChanceMultiplier";
	case Type_ArmsEffectChanceMultiplier:
		return "ArmsEffectChanceMultiplier";
	case Type_FootEffectChanceMultiplier:
		return "FootEffectChanceMultiplier";
	case Type_HeartEffectChanceMultiplier:
		return "HeartEffectChanceMultiplier";
	}

	return "";
}

void INIFile::ShowSettings()
{
	_MESSAGE("General");
	for (auto& map : GeneralMap)
	{
		_MESSAGE("   %s  =  %d", (map.first).c_str(), map.second);
	}

	for (auto& map : MessageMap)
	{
		_MESSAGE("   %s  =  %s", (map.first).c_str(), (map.second).c_str());
	}

	_MESSAGE("Armor");
	for (auto& map : ArmorMap)
	{
		_MESSAGE("   %s  =  %lf", (map.first).c_str(), map.second);
	}

	_MESSAGE("PlayerToNPC");
	for (int i = 0; i < Type_Num; i++)
	{
		_MESSAGE("   %s  =  %lf", GetMultiplierString(static_cast<MultiplierType>(i)).c_str(), PlayerToNPC[i]);
	}

	_MESSAGE("NPCToPlayer");
	for (int i = 0; i < Type_Num; i++)
	{
		_MESSAGE("   %s  =  %lf", GetMultiplierString(static_cast<MultiplierType>(i)).c_str(), NPCToPlayer[i]);
	}

	_MESSAGE("NPCToNPC");
	for (int i = 0; i < Type_Num; i++)
	{
		_MESSAGE("   %s  =  %lf", GetMultiplierString(static_cast<MultiplierType>(i)).c_str(), NPCToNPC[i]);
	}
}
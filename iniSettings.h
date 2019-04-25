#pragma once

#define INI_FILE "LocationalDamage.ini"

#include <windows.h>
#include <unordered_map>

enum MultiplierType
{
	Type_HeadDamageMultiplier,
	Type_ArmsDamageMultiplier,
	Type_FootDamageMultiplier,
	Type_HeartDamageMultiplier,
	Type_HeadEffectChanceMultiplier,
	Type_ArmsEffectChanceMultiplier,
	Type_FootEffectChanceMultiplier,
	Type_HeartEffectChanceMultiplier,
	Type_Num
};

class INIFile
{
public:
	INIFile();
	void Load();
	void ToLower(std::string &str);

	bool EnableHead;
	bool EnableArms;
	bool EnableFoot;
	bool EnableHeart;

	UInt32 DamageTypeHead;
	UInt32 DamageTypeArms;
	UInt32 DamageTypeFoot;
	UInt32 DamageTypeHeart;
	UInt32 EffectTypeHead;
	UInt32 EffectTypeArms;
	UInt32 EffectTypeFoot;
	UInt32 EffectTypeHeart;

	UInt32 InterruptBaseChance;
	UInt32 StaggerBaseChance;
	UInt32 KnockdownBaseChance;
	UInt32 UnequipArmorBaseChance;
	UInt32 UnequipWeaponBaseChance;

	bool DisplayImpactEffect;
	bool DisplayNotification;

	std::string HeadMessageFront;
	std::string HeadMessageBack;
	std::string ArmsMessageFront;
	std::string ArmsMessageBack;
	std::string FootMessageFront;
	std::string FootMessageBack;
	std::string HeartMessageFront;
	std::string HeartMessageBack;

	double HeavyArmorDamageMultiplier;
	double LightArmorDamageMultiplier;
	double HeavyArmorEffectChanceMultiplier;
	double LightArmorEffectChanceMultiplier;

	double PlayerToNPC[Type_Num];
	double NPCToPlayer[Type_Num];
	double NPCToNPC[Type_Num];

private:
	std::string GetSkyrimPath();
	std::string GetSksePluginPath();
	bool IsFoundFile(const char* fileName);
	std::vector<std::string> GetSectionKeys(LPCTSTR section_name, LPCTSTR ini_file_path);
	std::vector<std::string> Split(const std::string &str, char sep);
	void EraseIf(std::string &str, std::string sep);

	void SetSettings();
	std::string GetINIlFile();
	void SetINIData(std::vector<std::string> *list);
	void SetINIDataArmor(std::vector<std::string> *list);
	void SetINIDataDouble(std::vector<std::string> *list, double *setting);
	std::string GetMultiplierString(MultiplierType type);
	void ShowSettings();

	std::string iniFilePath;

	std::unordered_map<std::string, UInt32> GeneralMap;
	std::unordered_map<std::string, std::string> MessageMap;
	std::unordered_map<std::string, double> ArmorMap;
};

extern INIFile ini;
#pragma once

#define INI_FILE "LocationalDamage.ini"

#include <windows.h>
#include <unordered_map>

#include "common.h"

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

	UInt32 SoundEffectFormID = DEFAULT_SOUNDEFFECT_FORMID;
	UInt32 SoundEffectSpellFormID = DEFAULT_SOUNDEFFECT_SPELL_FORMID;
	UInt32 ImpactEffectFormID = 0; // will be set in constructor based on mod index

	bool DisplayImpactEffect;
	bool DisplayNotification;
	bool PlaySoundEffect = true;
	bool PlaySoundAtEnemyLocation = false;
	int DisplayNotificationMinDamage = 1;
	int UseSKSETrampolineInterface = 1;

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

	// NEW ini setting for spells
	double SpellDamageMultiplier = 0.0;

	double PlayerToNPC[Type_Num];
	double NPCToPlayer[Type_Num];
	double NPCToNPC[Type_Num];

private:
	std::string GetSkyrimPath();
	std::string GetSksePluginPath();
	bool IsFoundFile(const char* fileName);
	std::vector<std::string> GetSectionKeys(const char* section_name, const char* ini_file_path);
	std::vector<std::string> Split(const std::string &str, char sep);
	void EraseIf(std::string &str, std::string sep);

	void SetSettings();
	std::string GetINIFile();
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
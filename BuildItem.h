#pragma once
#include <BWAPI.h>

class BuildItem
{
public:
	BWAPI::UnitType buildtype; // 1
	BWAPI::TechType researchtype; // 2
	BWAPI::UpgradeType upgradetype; // 3
	int typenr;

	BuildItem();
	BuildItem(BWAPI::UnitType t, int p);
	BuildItem(BWAPI::TechType t, int p);
	BuildItem(BWAPI::UpgradeType t, int p);

	int priority;
	int mineralPrice();
	int gasPrice();

	BuildItem expand(); // type 4

	int base;
	bool operator==(const BuildItem& other) const;
};

#pragma once
#include <BWAPI.h>

enum BuildItemType
{
	NoType,
	BuildType,
	ResearchType,
	UpgradeType,
	ExpandType
};

class BuildItem
{
public:
	BWAPI::UnitType buildtype;
	BWAPI::TechType researchtype;
	BWAPI::UpgradeType upgradetype;
	BuildItemType type;

	BuildItem();
	BuildItem(BWAPI::UnitType t, int p);
	BuildItem(BWAPI::TechType t, int p);
	BuildItem(BWAPI::UpgradeType t, int p);

	int priority;
	int mineralPrice();
	int gasPrice();

	BuildItem expand();

	int base;
	bool operator==(const BuildItem& other) const;
};
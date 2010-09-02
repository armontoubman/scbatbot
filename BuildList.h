#pragma once
#include "BuildItem.h"
#include <BWAPI.h>
#include <list>
class BuildList {
public:
	BuildList();
	void addItem(BuildItem b);
	void removeItem(BuildItem b);
	void removeAll(BuildItem b);
	void removeTop();
	void clear();
	int count(BWAPI::UnitType t);
	int count(BWAPI::TechType t);
	int count(BWAPI::UpgradeType t);
	int size();
	bool isEmpty();

	void update();

	bool containsUnits();
	bool containsExpand();

	BuildItem top();
	void addItemTop(BuildItem b);

	int supplyRequiredForTopThree();
	int countUnits();

	std::list<BuildItem> buildlist;
	void init();
};
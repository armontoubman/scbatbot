#pragma once
#include <BWAPI.h>
#include "EnemyUnitData.h"
#include <InformationManager.h>
#include <UnitGroup.h>
class EnemyUnitDataManager
{
public:
	EnemyUnitDataManager();
	EnemyUnitDataManager(InformationManager* im);
	void update(std::set<BWAPI::Unit*> units);
	void onRemoveUnit(BWAPI::Unit* unit);
	std::map<BWAPI::Unit*,EnemyUnitData> getData();
	int count(BWAPI::UnitType unittype);
	UnitGroup getUG();
private:
	std::string intToString(int i);
	std::map<BWAPI::Unit*,EnemyUnitData> enemyUnitsMap;
	InformationManager* informationManager;
};
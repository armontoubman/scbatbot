#pragma once
#include <BWAPI.h>
#include <InformationManager.h>
#include "EigenUnitDataManager.h"
#include "EnemyUnitDataManager.h"
#include "EigenUnitGroupManager.h"
class HighCommand
{
public:
	HighCommand(InformationManager* im);
	~HighCommand();
	void update(std::set<BWAPI::Unit*> myUnits, std::set<BWAPI::Unit*> enemyUnits);
	void onRemoveUnit(BWAPI::Unit* unit);
	BWAPI::Unit* getNearestHatchery(BWAPI::Position pos);
private:
	std::string intToString(int i);
	EigenUnitDataManager* eigenUnitDataManager;
	EnemyUnitDataManager* enemyUnitDataManager;
	EigenUnitGroupManager* eigenUnitGroupManager;
};
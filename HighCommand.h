#pragma once
#include <BWAPI.h>
#include <InformationManager.h>
#include "EigenUnitDataManager.h"
#include "EnemyUnitDataManager.h"
#include "EigenUnitGroupManager.h"
#include "TaskManager.h"
#include "WantBuildManager.h"
#include "MicroManager.h"
class EigenUnitDataManager;
class EnemyUnitDataManager;
class EigenUnitGroupManager;
class TaskManager;
class WantBuildManager;
class MicroManager;
class HighCommand
{
public:
	HighCommand(InformationManager* im, BuildOrderManager* bom, BaseManager* ba);
	~HighCommand();
	void update(std::set<BWAPI::Unit*> myUnits, std::set<BWAPI::Unit*> enemyUnits);
	void onRemoveUnit(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	BWAPI::Unit* getNearestHatchery(BWAPI::Position pos);

	BWAPI::Unit* hatchery;
private:
	std::string intToString(int i);
	EigenUnitDataManager* eigenUnitDataManager;
	EnemyUnitDataManager* enemyUnitDataManager;
	EigenUnitGroupManager* eigenUnitGroupManager;
	TaskManager* taskManager;
	WantBuildManager* wantBuildManager;
	MicroManager* microManager;

	int tick;
	int thisAlgorithmBecomingSkynetCost;
};
#pragma once
#include <BWAPI.h>
#include <BuildOrderManager.h>
#include <UnitGroup.h>
#include "EnemyUnitDataManager.h"
#include "TaskManager.h"
#include "HighCommand.h"
#include "EigenUnitDataManager.h"
#include "WantBuildManager.h"
class BuildOrderManager;
class EnemyUnitDataManager;
class TaskManager;
class HighCommand;
class EigenUnitDataManager;
class WantBuildManager;
class MicroManager
{
	friend class HighCommand;
public:
	MicroManager();
	MicroManager(BuildOrderManager* b, EnemyUnitDataManager* e, TaskManager* t, HighCommand* h, EigenUnitDataManager* ei, WantBuildManager* w);
	BWAPI::Position moveAway(BWAPI::Unit* unit, double radius);
	BWAPI::Position moveAway(BWAPI::Unit* unit);
	void moveAway(std::set<BWAPI::Unit*> units);
	std::set<BWAPI::Position> getAdjacentPositions(BWAPI::Position p);
	std::set<BWAPI::Position> sanitizePositions(std::set<BWAPI::Position> ps);
	bool overlordSupplyProvidedSoon();
	bool enemyInRange(BWAPI::Position p, double radius, int type);
	bool enemyInRange(BWAPI::Position p);
	UnitGroup enemiesInRange(BWAPI::Position p, double radius, int type);
	bool containsDetector(UnitGroup ug);
	int compareArmySize(UnitGroup x, UnitGroup y);
	BWAPI::Unit* nearestUnit(BWAPI::Position pos, UnitGroup ug);
	UnitGroup enemiesInSeekRange(BWAPI::Position p, double radius, int type);
	bool alliesCanAttack(BWAPI::Position p, UnitGroup ug);
	void mineWhere(BWAPI::Unit* unit);
	void gasWhere(BWAPI::Unit* unit);
	void gatherWhere(BWAPI::Unit* unit);
	void moveToNearestBase(BWAPI::Unit* unit);
	void moveToNearestBase(std::set<BWAPI::Unit*> units);
	bool isUnderDarkSwarm(BWAPI::Unit* unit);
	bool canAttackAir(BWAPI::Unit* unit);
	bool canAttackAir(std::set<BWAPI::Unit*> units);
	bool canAttackGround(BWAPI::Unit* unit);
	bool canAttackGround(std::set<BWAPI::Unit*> units);
	bool tooSplitUp(double radius, UnitGroup* ug);
	int amountCanAttackAir(std::set<BWAPI::Unit*> units);
	int amountCanAttackGround(std::set<BWAPI::Unit*> units);
	double minimalDistanceToGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* nearestEnemyThatCanAttackAir(BWAPI::Unit* unit);
	BWAPI::Unit* nearestNonBuildingEnemy(BWAPI::Unit* unit);
	BWAPI::Unit* nearestSwarm(BWAPI::Unit* unit);
	BWAPI::Unit* nearestEnemyNotUnderDarkSwarm(BWAPI::Unit* unit);
	BWAPI::Position splitUp(BWAPI::Unit* unit);
	void splitUp(std::set<BWAPI::Unit*> units);
	UnitGroup* inRadiusUnitGroup(double radius, UnitGroup* ug);
	UnitGroup* inRadiusUnitGroupUnitType(double radius, UnitGroup* ug, BWAPI::UnitType ut);
	BWAPI::Position getCenterPositionFromEnemyMap(std::map<BWAPI::Unit*, EnemyUnitData> data);
	BWAPI::Unit* getVisibleUnit(std::set<BWAPI::Unit*> units);

	void doMicro(std::set<UnitGroup*> listUG);

	// zelfde als in WantBuildManager
	UnitGroup getHatcheriesWithMinerals();
	UnitGroup getUnusedMineralsNearHatcheries();

	BuildOrderManager* bom;
	EnemyUnitDataManager* eudm;
	TaskManager* tm;
	HighCommand* hc;
	EigenUnitDataManager* eiudm;
	WantBuildManager* wbm;
	
	std::string intToString(int i);
	void logx(BWAPI::Unit*, std::string msg);
	void logc(const char* msg);

	double dist(int d);
};
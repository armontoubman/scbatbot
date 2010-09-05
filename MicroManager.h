#pragma once
#include <BWAPI.h>
#include <BuildOrderManager.h>
#include <UnitGroup.h>
#include "EnemyUnitDataManager.h"
#include "TaskManager.h"
#include "HighCommand.h"
#include "EigenUnitDataManager.h"
class BuildOrderManager;
class EnemyUnitDataManager;
class TaskManager;
class HighCommand;
class EigenUnitDataManager;
class MicroManager
{
	friend class HighCommand;
public:
	MicroManager();
	MicroManager(BuildOrderManager* b, EnemyUnitDataManager* e, TaskManager* t, HighCommand* h, EigenUnitDataManager* ei);
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
	void moveToNearestBase(BWAPI::Unit* unit);
	void moveToNearestBase(std::set<BWAPI::Unit*> units);
	bool isUnderDarkSwarm(BWAPI::Unit* unit);
	bool canAttackAir(BWAPI::Unit* unit);
	bool canAttackAir(std::set<BWAPI::Unit*> units);
	bool canAttackGround(BWAPI::Unit* unit);
	bool canAttackGround(std::set<BWAPI::Unit*> units);
	int amountCanAttackAir(std::set<BWAPI::Unit*> units);
	int amountCanAttackGround(std::set<BWAPI::Unit*> units);
	double minimalDistanceToGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* nearestEnemyThatCanAttackAir(BWAPI::Unit* unit);
	BWAPI::Unit* nearestNonBuildingEnemy(BWAPI::Unit* unit);
	BWAPI::Unit* nearestSwarm(BWAPI::Unit* unit);
	BWAPI::Unit* nearestEnemyNotUnderDarkSwarm(BWAPI::Unit* unit);
	BWAPI::Position splitup(BWAPI::Unit* unit);
	UnitGroup* MicroManager::inRadiusUnitGroup(double radius, UnitGroup* ug);
	UnitGroup* MicroManager::inRadiusUnitGroupUnitType(double radius, UnitGroup* ug, BWAPI::UnitType ut);

	void doMicro(std::set<UnitGroup*> listUG);

	// zelfde als in WantBuildManager
	UnitGroup getHatcheriesWithMinerals();
	UnitGroup getUnusedMineralsNearHatcheries();

	BuildOrderManager* bom;
	EnemyUnitDataManager* eudm;
	TaskManager* tm;
	HighCommand* hc;
	EigenUnitDataManager* eiudm;

	
	std::string intToString(int i);
	void logx(std::string func, int id, std::string msg);

	double dist(int d);
};
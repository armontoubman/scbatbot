#pragma once
#include <BWAPI.h>
#include "BuildList.h"
#include "EnemyUnitDataManager.h"
#include "BuildOrderManager.h"
#include "BaseManager.h"
class WantBuildManager
{
	friend class HighCommand;
public:
	WantBuildManager();
	WantBuildManager(EnemyUnitDataManager* e, BuildOrderManager* b, BaseManager* ba);

	void update();

	void doLists();

	void addWant(BWAPI::UnitType unittype);
	void addWant(BWAPI::UnitType unittype, int amount);
	void addWant(BWAPI::TechType techtype);
	void addWant(BWAPI::UpgradeType upgradetype);

	void addBuild(BWAPI::UnitType unittype);
	void addBuild(BWAPI::UnitType unittype, int amount);
	void addBuild(BWAPI::TechType techtype);
	void addBuild(BWAPI::UpgradeType upgradetype);

	void addBuildTop(BWAPI::UnitType unittype);

	void wantExpand();
	void buildExpand();

	int nrOfEnemy(BWAPI::UnitType unittype);
	int nrOfOwn(BWAPI::UnitType unittype);
	
	bool wantListIsEmpty();
	bool buildListIsEmpty();

	bool wantListContains(BWAPI::UnitType unittype);
	bool buildListContains(BWAPI::UnitType unittype);

	bool wantListIsCompleted();

	int nrOfEnemyMilitaryUnits();
	int nrOfOwnMilitaryUnits();

	int nrOfEnemyBases();

	bool photonCannonNearBase();
	int countEggsMorphingInto(BWAPI::UnitType unittype);
	UnitGroup getHatcheriesWithMinerals();
	UnitGroup getUnusedMineralsNearHatcheries();
	BWTA::BaseLocation* getNaturalExpansion();

	bool requirementsSatisfied(BWAPI::UnitType unittype);
	bool requirementsSatisfied(BWAPI::TechType techtype);
	bool requirementsSatisfied(BWAPI::UpgradeType upgradetype);
	bool canBeMade(BWAPI::UnitType unittype);
	bool canBeMade(BWAPI::TechType techtype);
	bool canBeMade(BWAPI::UpgradeType upgradetype);

	BuildList buildList;
	BuildList wantList;

	EnemyUnitDataManager* eudm;
	BuildOrderManager* bom;
	BaseManager* bm;

	double dist(int d);
	void logx(std::string func, int id, std::string msg);
	std::string intToString(int i);
};
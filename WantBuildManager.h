#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
#include "BuildList.h"
#include "EnemyUnitDataManager.h"
#include "BuildOrderManager.h"
#include "BaseManager.h"
#include "HighCommand.h"
#include "MicroManager.h"
class MicroManager;
class WantBuildManager
{
	friend class HighCommand;
public:
	WantBuildManager();
	WantBuildManager(EnemyUnitDataManager* e, HighCommand* h, MicroManager* m);

	int stap;

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
	int dronesRequiredAll();
	UnitGroup getHatcheriesWithMinerals();
	BWTA::BaseLocation* getNaturalExpansion();

	bool requirementsSatisfied(BuildItem b);
	bool requirementsSatisfied(BWAPI::UnitType unittype);
	bool requirementsSatisfied(BWAPI::TechType techtype);
	bool requirementsSatisfied(BWAPI::UpgradeType upgradetype);
	bool canBeMade(BuildItem b);
	bool canBeMade(BWAPI::UnitType unittype);
	bool bothCanBeMade(BWAPI::UnitType unittype,BWAPI::UnitType unittypetwo);
	bool canBeMade(BWAPI::TechType techtype);
	bool bothCanBeMade(BWAPI::UnitType unittype, BWAPI::TechType techtype);
	bool canBeMade(BWAPI::UpgradeType upgradetype);
	bool bothCanBeMade(BWAPI::UnitType unittype, BWAPI::UpgradeType researchtype);
	bool bothCanBeMadeExpandUnit(BWAPI::UnitType unittype);
	bool bothCanBeMadeExpand(BWAPI::TechType techtype);
	bool bothCanBeMadeExpand(BWAPI::UpgradeType researchtype);
	

	BWAPI::TilePosition placeFound(BWAPI::UnitType unittype);
	BWAPI::TilePosition placeFoundExtractor();
	void bouwStruc(BWAPI::TilePosition tilepos, BWAPI::UnitType unittype);
	BWAPI::Unit* pickBuildDrone(BWAPI::TilePosition tilepos);

	BuildList buildList;
	BuildList wantList;

	EnemyUnitDataManager* eudm;
	HighCommand* hc;
	MicroManager* mm;

	// zelfde als in MicroManager
	BWAPI::Unit* nearestUnit(BWAPI::Position pos, UnitGroup ug);

	double dist(int d);
	void logx(std::string func, int id, std::string msg);
	std::string intToString(int i);
	void logc(const char* msg);

	std::set<BWAPI::Unit*> bouwdrones;
	void doExpand();
	bool isBeingHandled(BuildItem b);

	void logBuildList(BuildList bl);
	std::string bouwlistString(BuildList bl);

	void eigenResearch(BWAPI::TechType techtype);
	void eigenUpgrade(BWAPI::UpgradeType upgradetype);

	int lastBuildOrderIssued;

	void buildNow(BuildItem b);
	bool checkGemaakt();
};
#pragma once
#include <BWAPI.h>
#include "ProductList.h"
#include "UnitGroup.h"
class HighCommand;

using namespace BWAPI;

class ProductionManager
{
public:
	ProductionManager(HighCommand* h);
	void update();
	ProductList* getWantList();
	ProductList* getBuildList();
private:
	HighCommand* hc;
	ProductList wantList;
	ProductList buildList;
	int step;

	/* update lists */
	void updateLists();
	void updateListsAgainstProtoss();
	void updateListsAgainstProtossReinforcements();
	void updateListsAgainstTerran();
	void updateListsAgainstTerranReinforcements();
	void updateListsAgainstZerg();
	void updateListsAgainstZergReinforcements();
	void updateListsUpgrades();
	void updateListsGeneral();
	void updateListsGenericRule();
	/* end update lists */

	/* ProductList interface */
	void addWant(BWAPI::UnitType unittype);
	void addWant(BWAPI::UnitType unittype, int amount);
	void addWant(BWAPI::TechType techtype);
	void addWant(BWAPI::UpgradeType upgradetype);
	void addBuild(BWAPI::UnitType unittype);
	void addBuild(BWAPI::UnitType unittype, int amount);
	void addBuild(BWAPI::TechType techtype);
	void addBuild(BWAPI::UpgradeType upgradetype);
	void addBuildTop(BWAPI::UnitType unittype);
	bool wantListContains(BWAPI::UnitType unittype);
	bool buildListContains(BWAPI::UnitType unittype);
	/* end ProductList interface */

	/* intel */
	int nrOfEnemy(BWAPI::UnitType unittype);
	int nrOfOwn(BWAPI::UnitType unittype);
	int nrOfEnemyBases();
	int nrOfOwnBasesWithMinerals();
	UnitGroup getOwnBasesWithMinerals();
	int nrOfOwnMilitaryUnits();
	int nrOfEnemyMilitaryUnits();
	int countEggsMorphingInto(BWAPI::UnitType unittype);
	/* end intel */

	void buildExpand();

	int nrOfExtraDronesRequired();
};
#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

using namespace BWAPI;

class ConstructionManager
{
public:
	ConstructionManager(HighCommand* h);
	void update();
private:
	HighCommand* hc;

	int gas;
	int minerals;
	int supplyTotal;
	int supplyUsed;

	bool requirementsSatisfied(Product p);
	bool requirementsSatisfied(BWAPI::UnitType unittype);
	bool requirementsSatisfied(BWAPI::TechType techtype);
	bool requirementsSatisfied(BWAPI::UpgradeType upgradetype);

	bool process(Product p);
	bool processBuild(Product p);
	bool processTech(Product p);
	bool processUpgrade(Product p);
	bool processExpand(Product p);
	bool processBuilding(Product p);
	bool processUnit(Product p);
	bool processHatcheryUpgrade();

	bool canAfford(Product p);
	void buy(Product p);
};
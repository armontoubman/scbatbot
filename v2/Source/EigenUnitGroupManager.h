#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

#include "UnitGroup.h"

using namespace BWAPI;

class EigenUnitGroupManager
{
public:
	EigenUnitGroupManager(HighCommand* h);
	void update();
	void init();
	void onUnitCreate(Unit* u);
	void onUnitDestroy(Unit* u);
	void onUnitMorph(Unit* u);
	void onUnitRenegade(Unit* u);
	std::set<UnitGroup*> getUnitGroupSet();
	std::string chat();
	bool groupIsDroneUG(UnitGroup* ug);
	UnitGroup* getGroupOfUnit(BWAPI::Unit* u);
	UnitGroup* getDroneUG();

private:
	HighCommand* hc;
	std::set<UnitGroup*> unitgroupset;
	UnitGroup* droneUG;
	bool initiated;
	bool isGroupable(Unit* u);
	void removeUnit(Unit* u);
	void cleanup();
	void balanceGroups();
	void assignUnit(Unit* u);
	UnitGroup* findFittingGroupForUnit(Unit* u);
	void moveUnitBetweenGroups(Unit* u, UnitGroup* ug1, UnitGroup* ug2);
	void splitGroup(UnitGroup* ugptr);
	bool unitIsInAGroup(Unit* u);
};
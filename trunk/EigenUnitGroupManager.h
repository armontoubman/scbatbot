#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
class HighCommand;
class EigenUnitGroupManager
{
public:
	EigenUnitGroupManager(HighCommand* hc);
	bool canAttackAir(UnitGroup unitGroup);
	bool canAttackGround(UnitGroup unitGroup);
	bool onlyAirUnits(UnitGroup unitGroup);
	bool onlyGroundUnits(UnitGroup unitGroup);
	BWAPI::Position getCenterPosition(UnitGroup unitGroup);
	double seenRatio(UnitGroup unitGroup);

	void addUG(UnitGroup* unitGroup);
	void removeUG(UnitGroup* unitGroup);
	void mergeUGs(UnitGroup* ug1, UnitGroup* ug2);
	void moveUnitBetweenGroups(UnitGroup* ug1, BWAPI::Unit* unit, UnitGroup* ug2);
	UnitGroup* findUnitGroupWithUnit(BWAPI::Unit* unit);

	void onRemoveUnit(BWAPI::Unit* unit);
private:
	std::set<UnitGroup*> unitGroups;
	HighCommand* highCommand;
};
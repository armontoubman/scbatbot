#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
#include "HighCommand.h"
#include "EigenUnitDataManager.h"
#include "TaskManager.h"
class HighCommand;
class EigenUnitDataManager;
class EigenUnitGroupManager
{
public:
	EigenUnitGroupManager(HighCommand* hc, EigenUnitDataManager* e, TaskManager* t);
	bool canAttackAir(UnitGroup unitGroup);
	bool canAttackGround(UnitGroup unitGroup);
	bool onlyAirUnits(UnitGroup unitGroup);
	bool onlyGroundUnits(UnitGroup unitGroup);
	BWAPI::Position getCenterPosition(UnitGroup unitGroup);
	double seenRatio(UnitGroup unitGroup);

	void addUG(UnitGroup* unitGroup);
	void removeUG(UnitGroup* unitGroup);
	void moveAll(UnitGroup* ug1, UnitGroup* ug2);
	void moveUnitBetweenGroups(UnitGroup* ug1, BWAPI::Unit* unit, UnitGroup* ug2);
	UnitGroup* findUnitGroupWithUnit(BWAPI::Unit* unit);

	void assignUnit(BWAPI::Unit* unit);
	void assignUnits(std::set<BWAPI::Unit*> units);

	void onRemoveUnit(BWAPI::Unit* unit);

	void splitGroup(UnitGroup* ug);

	// zelfde als in micromanager
	BWAPI::Unit* nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);

	void update();

private:
	std::set<UnitGroup*> unitGroups;
	HighCommand* highCommand;
	EigenUnitDataManager* eiudm;
	TaskManager* tm;

	UnitGroup* defendlingUG;
	UnitGroup* overlordUG;
	UnitGroup* droneUG;
	UnitGroup* defendmutaUG;
	UnitGroup* defendgroepUG;
	UnitGroup* lurkergroepUG;
};
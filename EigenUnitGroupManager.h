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
	friend class HighCommand;
public:
	EigenUnitGroupManager(HighCommand* hc, EigenUnitDataManager* e, TaskManager* t, PlanAssigner* p);
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
	UnitGroup* findOtherUG(BWAPI::Unit* unit);
	void dropInEenUG(BWAPI::Unit* unit);

	void onRemoveUnit(BWAPI::Unit* unit);

	void splitGroup(UnitGroup* ug);

	bool groupContainsType(std::set<BWAPI::Unit*>* ug, BWAPI::UnitType unittype);
	std::string getName(UnitGroup* group);
	void logug(UnitGroup* group, char* msg);
	void logc(const char* msg);
	void printGroepen();

	// zelfde als in micromanager
	BWAPI::Unit* nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units);

	void update();


	std::set<UnitGroup*> unitGroups;
	HighCommand* highCommand;
	EigenUnitDataManager* eiudm;
	TaskManager* tm;
	PlanAssigner* pa;

	UnitGroup* droneUG;
};
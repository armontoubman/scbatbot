#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
#include "TaskManager.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include "MicroManager.h"
class HighCommand;
class TaskManager;
class EigenUnitGroupManager;
class EnemyUnitDataManager;
class MicroManager;
class PlanAssigner
{
public:
	PlanAssigner(HighCommand* h, TaskManager* t, EigenUnitGroupManager* e, EnemyUnitDataManager* ed, MicroManager* m);

	HighCommand* hc;
	TaskManager* tm;
	EigenUnitGroupManager* eiugm;
	EnemyUnitDataManager* eudm;
	MicroManager* mm;

	std::map<UnitGroup*, Task> maakPlan();
	int canAttack(UnitGroup* ug1, UnitGroup* ug2);
	Task mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan);
	Task mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan, bool nullwaarde);
	bool containsTask(std::map<UnitGroup*, Task> plan, Task t);
	void addAssignment(UnitGroup* ug, Task t, std::map<UnitGroup*, Task>* plan);
	double logicaldistance(UnitGroup* ug, BWAPI::Position pos);
	
	std::map<UnitGroup*, Task> plan;
	void update();
	Task vindTask(UnitGroup* ug);
};


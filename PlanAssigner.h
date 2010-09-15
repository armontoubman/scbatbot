#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
#include "TaskManager.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include "MicroManager.h"
#include "UnitGroup.h"
#include "Task.h"
#include <map>
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
	int canAttack(UnitGroup* ug, Task t);
	int canAttack(Task t, UnitGroup* ug);
	Task mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan);
	Task mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan, bool nullwaarde);
	bool containsTask(std::map<UnitGroup*, Task> plan, Task t);
	double logicaldistance(UnitGroup* ug, BWAPI::Position pos);
	
	std::map<UnitGroup*, Task> plan;
	void update();
	Task vindTask(std::map<UnitGroup*, Task> lijst, UnitGroup* ug);
	std::list<Task> findTasksWithType(std::map<UnitGroup*, Task> lijst, int t);
	void logc(const char* msg);
};


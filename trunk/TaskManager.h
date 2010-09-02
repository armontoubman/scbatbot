#pragma once
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include <BWAPI.h>
#include <list>
class TaskManager {
public:
	TaskManager();
	TaskManager(EigenUnitGroupManager* e);
	EigenUnitGroupManager* eugm;

	void insertTask(Task t);
	void removeTask(Task t);
	Task highestPriorityTask();
	Task findTaskWithUnitGroup(UnitGroup* ug);
	Task findTaskWithUnit(BWAPI::Unit* unit);
	std::set<Task> findTasksWithUnitType(BWAPI::UnitType unittype);

	void update();

	std::list<Task> tasklist;
};
#pragma once
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include <BWAPI.h>
#include <list>
class EigenUnitGroupManager;
class TaskManager {
	friend class HighCommand;
public:
	TaskManager();
	TaskManager(EigenUnitGroupManager* e, EnemyUnitDataManager* eu);
	EigenUnitGroupManager* eugm;
	EnemyUnitDataManager* eudm;

	void insertTask(Task t);
	void removeTask(Task t);
	Task highestPriorityTask();
	Task findTaskWithUnitGroup(UnitGroup* ug);
	Task findTaskWithUnit(BWAPI::Unit* unit);
	std::set<Task> findTasksWithUnitType(BWAPI::UnitType unittype);
	std::set<Task> findTasksWithType(int t);

	void update();

	std::set<Task> tasklist;

	// zelfde als overal
	double dist(int d);
	BWAPI::Unit* nearestUnit(BWAPI::Position pos, UnitGroup ug);
	BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset);
};
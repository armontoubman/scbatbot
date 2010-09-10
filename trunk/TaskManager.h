#pragma once
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include <BWAPI.h>
#include <list>
#include "HighCommand.h"
class EigenUnitGroupManager;
class TaskManager {
	friend class HighCommand;
public:
	TaskManager();
	TaskManager(EigenUnitGroupManager* e, EnemyUnitDataManager* eu, HighCommand* h);
	EigenUnitGroupManager* eugm;
	EnemyUnitDataManager* eudm;
	HighCommand* hc;

	void insertTask(Task t);
	void removeTask(Task t);
	Task highestPriorityTask();
	Task findTaskWithUnitGroup(UnitGroup* ug);
	Task findTaskWithUnit(BWAPI::Unit* unit);
	std::set<Task> findTasksWithUnitType(BWAPI::UnitType unittype);
	std::set<Task> findTasksWithType(int t);
	BWAPI::Position frontlineBuilding(std::set<BWAPI::Position> posset);
	bool existsTaskWithType(int type);
	bool existsTaskWithPriority(int priority);
	Task nearestTask(UnitGroup* ug, std::set<Task> tasks);

	void update();

	std::set<Task> tasklist;

	// zelfde als overal
	double dist(int d);
	BWAPI::Unit* nearestUnit(BWAPI::Position pos, UnitGroup ug);
	BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset);
};
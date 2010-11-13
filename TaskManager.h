#pragma once
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include <BWAPI.h>
#include <list>
#include "HighCommand.h"
#include "PlanAssigner.h"
class PlanAssigner;
class EigenUnitGroupManager;
class TaskManager {
	friend class HighCommand;
public:
	TaskManager();
	TaskManager(EigenUnitGroupManager* e, EnemyUnitDataManager* eu, HighCommand* h, PlanAssigner* p);
	EigenUnitGroupManager* eugm;
	EnemyUnitDataManager* eudm;
	HighCommand* hc;
	PlanAssigner* pa;

	void insertTask(Task t);
	void removeTask(Task t);
	Task highestPriorityTask();
	Task findTaskWithUnitGroup(UnitGroup* ug);
	Task findTaskWithUnit(BWAPI::Unit* unit);
	std::list<Task> findTasksWithType(int t);
	BWAPI::Position frontlineBuilding(std::set<BWAPI::Position> posset);
	bool existsTaskWithType(int type);
	bool existsTaskWithPriority(int priority);
	Task nearestTask(UnitGroup* ug, std::list<Task> tasks);
	std::string getName(int type);

	Task createTask(int type, BWAPI::Position pos, std::map<BWAPI::Unit*, EnemyUnitData> data);

	void update();

	std::list<Task> tasklist;

	// zelfde als overal
	double dist(int d);
	BWAPI::Unit* nearestUnit(BWAPI::Position pos, UnitGroup ug);
	BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset);
};
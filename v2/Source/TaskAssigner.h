#pragma once
#include <BWAPI.h>
#include "HighCommand.h"

using namespace BWAPI;

class TaskAssigner
{
public:
	TaskAssigner(HighCommand* h);
	void update();
	std::map<UnitGroup*, Task> getPlan();
	std::map<BWAPI::Unit*, Task> getDronePlan();
	Task getTaskOfUnit(BWAPI::Unit* unit);

	void onUnitDestroy(Unit* u);
	void onUnitMorph(Unit* u);

private:
	HighCommand* hc;

	std::map<UnitGroup*, Task> plan;
	std::map<BWAPI::Unit*, Task> dronePlan;
	void makePlan();
	void assign(UnitGroup* ug, Task task);
	void assignDrone(BWAPI::Unit*, Task task);
	Task bestTask(UnitGroup* ug, TaskType tasktype);
	bool taskIsAssigned(Task task);
	int countDronesOnTask(Task task);
	Task nearestTask(BWAPI::Position pos, std::list<Task> tasklist);
};
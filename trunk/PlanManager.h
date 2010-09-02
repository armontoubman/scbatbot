#pragma once
#include "EigenAssignment.h"
#include "Task.h"
#include <UnitGroup.h>
#include <BWAPI.h>
#include <list>

class PlanManager {
public:
	PlanManager();
	EigenAssignment* nieuweAssignment(Task* t, UnitGroup* ug);

	void update();

private:
	std::list<EigenAssignment> plan;
};
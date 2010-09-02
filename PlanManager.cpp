#include "PlanManager.h"
#include "EigenAssignment.h"
#include "Task.h"
#include <BWAPI.h>
#include <list>
#include <algorithm>
PlanManager::PlanManager()
{
}

EigenAssignment* PlanManager::nieuweAssignment(Task* t, UnitGroup* ug)
{
	EigenAssignment* a = new EigenAssignment(t, ug);
	this->plan.push_back(*a);
	return a;
}

void PlanManager::update()
{
}
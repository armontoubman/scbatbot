#include "Task.h"
#include <BWAPI.h>
#include <UnitGroup.h>
Task::Task()
{
}

Task::Task(int t, int p, BWAPI::Position pos, UnitGroup* ug)
{
	this->type = t;
	this->priority = p;
	this->position = pos;
	this->unitGroup = ug;
}

bool Task::operator==(const Task& param) const
{
	return param.type == type && param.priority == priority && param.position == position && param.unitGroup == unitGroup;
}

bool Task::operator<(const Task& rhs) const
{
	return priority < rhs.priority;
}
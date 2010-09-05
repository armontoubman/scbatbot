#include "Task.h"
#include <BWAPI.h>
#include <UnitGroup.h>
Task::Task()
{
	this->type = -1;
}

Task::Task(int t, int p, BWAPI::Position pos, UnitGroup* ug)
{
	this->type = t;
	this->priority = p;
	this->position = pos;
	this->unitGroup = ug;
	this->enemy = NULL;
}

Task::Task(int t, int p, BWAPI::Position pos, UnitGroup* ug, UnitGroup* e)
{
	this->type = t;
	this->priority = p;
	this->position = pos;
	this->unitGroup = ug;
	this->enemy = e;
}

bool Task::operator==(const Task& param) const
{
	return param.type == type && param.priority == priority && param.position == position && param.unitGroup == unitGroup;
}

bool Task::operator<(const Task& rhs) const
{
	return priority < rhs.priority;
}
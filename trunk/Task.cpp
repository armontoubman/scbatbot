#include "Task.h"
#include <BWAPI.h>
#include <UnitGroup.h>
Task::Task()
{
	this->type = -999;
}

Task::Task(int t, int p, BWAPI::Position pos)
{
	this->type = t;
	this->priority = p;
	this->position = pos;
	this->enemy = NULL;
}

Task::Task(int t, int p, BWAPI::Position pos, UnitGroup* e)
{
	this->type = t;
	this->priority = p;
	this->position = pos;
	this->enemy = e;
}

bool Task::operator==(const Task& param) const
{
	return param.type == type && param.priority == priority && param.position == position && param.enemy == enemy;
}

bool Task::operator<(const Task& rhs) const
{
	return priority > rhs.priority; // hogere prioriteit (>) is eerder in de lijst (<)
}
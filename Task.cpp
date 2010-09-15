#include "Task.h"
#include <BWAPI.h>
#include <UnitGroup.h>
Task::Task()
{
	this->type = -999;
}

Task::Task(int t, BWAPI::Position pos)
{
	this->type = t;
	this->position = pos;
	this->enemyContainsAir = 0;
	this->enemyContainsGround = 0;
	this->enemySize = 0;
	this->enemyMilitarySize = 0;
}

Task::Task(int t, BWAPI::Position pos, bool eCA, bool eCG, int eS, int eMS)
{
	this->type = t;
	this->position = pos;
	this->enemyContainsAir = eCA;
	this->enemyContainsGround = eCG;
	this->enemySize = eS;
	this->enemyMilitarySize = eMS;
}

bool Task::operator==(const Task& param) const
{
	return param.type == type && param.position == position;
}

bool Task::operator<(const Task& rhs) const
{
	return type > rhs.type;
}
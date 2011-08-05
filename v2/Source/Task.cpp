#pragma once
#include "Task.h"
#include <BWAPI.h>
#include "UnitGroup.h"
#include "Util.h"
#include <boost/format.hpp>

using namespace BWAPI;

Task::Task()
{
	this->type = NoTask;
	this->position = Positions::None;
	this->target = NULL;
}

Task::Task(TaskType t, BWAPI::Position pos)
{
	this->type = t;
	this->position = pos;
	this->target = NULL;
}

Task::Task(TaskType t, BWAPI::Position pos, Unit* target)
{
	this->type = t;
	this->position = pos;
	this->target = target;
}

bool Task::operator==(const Task& param) const
{
	return param.type == type && param.position == position;
}

bool Task::operator<(const Task& rhs) const
{
	return type < rhs.type;
}

TaskType Task::getType()
{
	return this->type;
}

Position Task::getPosition()
{
	return this->position;
}

Unit* Task::getTarget()
{
	if(this->target == NULL)
	{
		std::string tname = this->getTypeName();
		log(boost::str(boost::format("Task::getTarget() returns NULL with Task(type %s, x:%d y%d)\n") % tname % this->position.x() % this->position.y() ).c_str());
	}
	return this->target;
}

std::string Task::getTypeName()
{
	return this->getTypeName(this->type);
}

std::string Task::getTypeName(TaskType t)
{
	if(t == NoTask) { return "NoTask"; }
	if(t == ScoutTask) { return "ScoutTask"; }
	if(t == CombatTask) { return "CombatTask"; }
	if(t == PrepareTask) { return "PrepareTask"; }
	if(t == DetectorTask) { return "DetectorTask"; }
	if(t == DefendTask) { return "DefendTask"; }
	if(t == GatherMineralsTask) { return "GatherMineralsTask"; }
	if(t == GatherGasTask) { return "GatherGasTask"; }
	return "Task::getTaskTypeName(INVALID TASKTYPE)";
}
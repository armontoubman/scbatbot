#pragma once
#include "TaskManager.h"

#include <BWAPI.h>
#include "HighCommand.h"

#include "Task.h"
#include "Util.h"

using namespace BWAPI;
using namespace BWTA;

TaskManager::TaskManager(HighCommand* h)
{
	this->hc = h;
}

void TaskManager::update()
{
	this->tasklistmap.clear();
	this->collectTasks();
	this->makeScoutTasksOnBaseLocations();
}

void TaskManager::collectTasks()
{
	this->tasklistmap[GatherMineralsTask].merge(this->hc->rm->getTasklist(GatherMineralsTask));
	this->tasklistmap[GatherGasTask].merge(this->hc->rm->getTasklist(GatherGasTask));
	this->tasklistmap[ScoutTask].merge(this->hc->eudm->getTasklist(ScoutTask));
	this->tasklistmap[CombatTask].merge(this->hc->eudm->getTasklist(CombatTask));
}

std::list<Task> TaskManager::getTasklist(TaskType tasktype)
{
	return this->tasklistmap[tasktype];
}

void TaskManager::makeScoutTasksOnBaseLocations()
{
	std::set<BaseLocation*> baselocs = getBaseLocations();
	if(Broodwar->self()->getUnits().size() < 60)
	{
		for each(BaseLocation* baseloc in baselocs)
		{
			if(!Broodwar->isExplored(baseloc->getTilePosition()))
			{
				this->tasklistmap[ScoutTask].push_back(Task(ScoutTask, baseloc->getPosition()));
			}
		}
	}
	else
	{
		for each(BaseLocation* baseloc in baselocs)
		{
			if(!Broodwar->isVisible(baseloc->getTilePosition()))
			{
				this->tasklistmap[ScoutTask].push_back(Task(ScoutTask, baseloc->getPosition()));
			}
		}
	}
}

std::string TaskManager::chat()
{
	std::string result = "TM: ";
	for each(std::pair<TaskType, std::list<Task>> typepair in this->tasklistmap)
	{
		result.append(intToString(typepair.second.size())).append(" ").append(Task().getTypeName(typepair.first)).append(", ");
	}
	return result;
}

std::map<TaskType, std::list<Task>> TaskManager::getTasklists()
{
	return this->tasklistmap;
}

bool TaskManager::existsTaskWithType(TaskType tasktype)
{
	return this->tasklistmap[tasktype].size() > 0;
}
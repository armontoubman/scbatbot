#include "TaskManager.h"
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include <BWAPI.h>
#include <list>
#include <algorithm>
TaskManager::TaskManager()
{
}

TaskManager::TaskManager(EigenUnitGroupManager* e)
{
	this->eugm = e;
}

void TaskManager::insertTask(Task t)
{
	std::list<Task>::iterator insertposition;
	bool plek = false;
	for(std::list<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if((*i).priority > t.priority) {
			insertposition = i;
			plek = true;
			break;
		}
	}
	if(plek) {
		this->tasklist.insert(insertposition, t);
	}
	else {
		this->tasklist.push_back(t);
	}
}

void TaskManager::removeTask(Task t)
{
	this->tasklist.erase(find(tasklist.begin(), tasklist.end(), t));
}

Task TaskManager::highestPriorityTask()
{
	return this->tasklist.back();
}

Task TaskManager::findTaskWithUnitGroup(UnitGroup* ug)
{
	for(std::list<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if(&(i->unitGroup) == ug) {
			return *i;
		}
	}
	return Task();
}

Task TaskManager::findTaskWithUnit(BWAPI::Unit* unit)
{
	return this->findTaskWithUnitGroup(this->eugm->findUnitGroupWithUnit(unit));
}

void TaskManager::update()
{
}

std::set<Task> TaskManager::findTasksWithUnitType(BWAPI::UnitType unittype)
{
	std::set<Task> result;
	for(std::list<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if(i->unitGroup(GetType,unittype).size() > 0) {
			result.insert(*i);
		}
	}
	return result;
}

std::set<Task> TaskManager::findTasksWithType(int t)
{
	std::set<Task> result;
	for(std::list<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if(i->type == t) {
			result.insert(*i);
		}
	}
	return result;
}
#include "TaskManager.h"
#include "Task.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include "EnemyUnitData.h"
#include <BWAPI.h>
#include <set>
#include <algorithm>
#include "Task.h"
#include "Util.h"
class Task;
class EigenUnitGroupManager;
TaskManager::TaskManager()
{
}

TaskManager::TaskManager(EigenUnitGroupManager* e, EnemyUnitDataManager* eu)
{
	this->eugm = e;
	this->eudm = eu;
}

void TaskManager::insertTask(Task t)
{
	log("insertTask()\n");
	std::set<Task>::iterator insertposition;
	bool plek = false;
	for(std::set<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		log("for\n");
		if((*i).priority > t.priority) {
			log("for if priority >\n");
			insertposition = i;
			plek = true;
			break;
		}
	}
	if(plek) {
		log("if plek\n");
		this->tasklist.insert(insertposition, t);
	}
	else {
		log("else\n");
		this->tasklist.insert(this->tasklist.end(), t);
	}

	BWAPI::Broodwar->printf("%s %d", "Task gemaakt met type", t.type);
}

void TaskManager::removeTask(Task t)
{
	this->tasklist.erase(find(tasklist.begin(), tasklist.end(), t));
}

Task TaskManager::highestPriorityTask()
{
	return *this->tasklist.end();
}

Task TaskManager::findTaskWithUnitGroup(UnitGroup* ug)
{
	for(std::set<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if((i->unitGroup) == ug) {
			return *i;
		}
	}
	return Task(-1, 1, (*ug->begin())->getPosition(), ug);
}

Task TaskManager::findTaskWithUnit(BWAPI::Unit* unit)
{
	return this->findTaskWithUnitGroup(this->eugm->findUnitGroupWithUnit(unit));
}

void TaskManager::update()
{
	/*this->tasklist.clear();

	std::map<BWAPI::Unit*, EnemyUnitData> datamap = this->eudm->getData();

	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in datamap)
	{
		if(enemy.second.position == BWAPI::Positions::Unknown)
		{
		}
	}*/
}

std::set<Task> TaskManager::findTasksWithUnitType(BWAPI::UnitType unittype)
{
	std::set<Task> result;
	for(std::set<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		UnitGroup* mettype = &(*i->unitGroup)(GetType,unittype);
		if(mettype->size() > 0) {
			result.insert(*i);
		}
	}
	return result;
}

std::set<Task> TaskManager::findTasksWithType(int t)
{
	std::set<Task> result;
	for(std::set<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if(i->type == t) {
			result.insert(*i);
		}
	}
	return result;
}
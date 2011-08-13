#include "TaskAssigner.h"

#include <BWAPI.h>
#include "HighCommand.h"
#include "Task.h"
#include "Util.h"

TaskAssigner::TaskAssigner(HighCommand* h)
{
	this->hc = h;
}

void TaskAssigner::update()
{
	this->plan.clear();
	this->dronePlan.clear();
	this->makePlan();
}

std::map<UnitGroup*, Task> TaskAssigner::getPlan()
{
	return this->plan;
}

std::map<BWAPI::Unit*, Task> TaskAssigner::getDronePlan()
{
	return this->dronePlan;
}

void TaskAssigner::makePlan()
{
	std::set<UnitGroup*> unitgroups = this->hc->eiugm->getUnitGroupSet();

	for each(UnitGroup* ug in unitgroups)
	{
		if(ug->size() == 0)
		{
			continue;
		}
		else if(this->hc->eiugm->groupIsDroneUG(ug))
		{
			// gas/minerals assignment hier

			std::map<TaskType, std::list<Task>> tasklists = this->hc->tm->getTasklists();
			int aantalMinerals = tasklists[GatherMineralsTask].size();
			int aantalGas = tasklists[GatherGasTask].size();

			for(std::set<BWAPI::Unit*>::const_iterator i=ug->begin();i!=ug->end();i++)
			{
				if(this->hc->ctm->isContracted(*i))
				{
					continue;
				}
				for each(Task task in tasklists[GatherGasTask])
				{
					if(this->countDronesOnTask(task) < 3)
					{
						this->assignDrone((*i), task);
						break;
					}
				}
				for each(Task task in tasklists[GatherMineralsTask])
				{
					if(this->countDronesOnTask(task) < 3)
					{
						this->assignDrone((*i), task);
						break;
					}
				}
			}

		}
		else if(ug->size() < 3)
		{
			if(ug->containsUnitOfType(BWAPI::UnitTypes::Zerg_Zergling) || ug->containsUnitOfType(BWAPI::UnitTypes::Zerg_Drone))
			{
				this->assign(ug, bestTask(ug, ScoutTask));
			}
			else
			{
				if(ug->containsUnitOfType(BWAPI::UnitTypes::Zerg_Overlord))
				{
					if(this->hc->tm->existsTaskWithType(DetectorTask))
					{
						this->assign(ug, bestTask(ug, DetectorTask));
					}
					else
					{
						this->assign(ug, bestTask(ug, ScoutTask));
					}
				}
				else
				{
					this->assign(ug, bestTask(ug, ScoutTask));
				}
			}
		}
		else
		{
			this->assign(ug, bestTask(ug, NoTask));
		}
	}
}

void TaskAssigner::assign(UnitGroup* ug, Task task)
{
	this->plan.insert(std::make_pair(ug, task));
}

void TaskAssigner::assignDrone(BWAPI::Unit* unit, Task task)
{
	this->dronePlan.insert(std::make_pair(unit, task));
}

Task TaskAssigner::bestTask(UnitGroup* ug, TaskType tasktype)
{
	std::map<TaskType, std::list<Task>> tasklists = this->hc->tm->getTasklists();
	std::list<Task> idealTasks;
	std::list<Task> goodTasks;
	std::list<Task> badTasks;
	std::list<Task> tasksToConsider;
	
	if(tasktype == NoTask)
	{
		tasksToConsider = tasklists[CombatTask];
		tasksToConsider.merge(tasklists[PrepareTask]);
		for each(Task t in tasksToConsider)
		{
			if(ug->canReach(t.getPosition()))
			{
				std::set<BWAPI::Unit*> grab = getEnemyUnitsInRadius(t.getPosition(), dist(10));
				if(grab.size() == 0)
				{
					badTasks.push_front(t);
				}
				else
				{
					if(ug->size() > 5)
					{
						if(grab.size() > 4)
						{
							idealTasks.push_front(t);
						}
						else
						{
							goodTasks.push_front(t);
						}
					}
					else
					{
						if(grab.size() > 4)
						{
							goodTasks.push_front(t);
						}
						else
						{
							idealTasks.push_front(t);
						}
					}
				}
			}
			else
			{
				// can't reach
			}
		}
		tasksToConsider = tasklists[DefendTask];
		for each(Task t in tasksToConsider)
		{
			std::set<BWAPI::Unit*> grab = getEnemyUnitsInRadius(t.getPosition(), dist(10));
			if(grab.size() > 4)
			{
				return t; // lol gewoon als defend met groot leger dit gewoon gelijk returnen, es zien wat happens
			}
			else
			{
				goodTasks.push_front(t);
			}
		}
		tasksToConsider = tasklists[ScoutTask];
		for each(Task t in tasksToConsider)
		{
			badTasks.push_front(t);
		}
	}
	if(tasktype == DetectorTask) // kan enkel overlord zijn (die een task van deze type wil)
	{
		tasksToConsider = tasklists[ScoutTask];
		tasksToConsider = tasklists[DetectorTask];
		for each(Task t in tasksToConsider)
		{
			if(t.getType() == ScoutTask && ug->canReach(t.getPosition()))
			{
				goodTasks.push_front(t);
			}
			else
			{
				if(t.getType() == DetectorTask && ug->canReach(t.getPosition()))
				{
					idealTasks.push_front(t);
				}
				else
				{
					if((t.getType() == DefendTask || t.getType() == CombatTask || t.getType() == PrepareTask) && ug->canReach(t.getPosition()))
					{
						badTasks.push_front(t);
					}
					else
					{
						// no task for you
					}
				}
			}
		}
	}
	else
	{
		tasksToConsider = tasklists[ScoutTask];
		for each(Task t in tasksToConsider)
		{
			if(ug->canReach(t.getPosition()))
			{
				idealTasks.push_front(t);
			}
		}
	}

	if(idealTasks.size() > 0)
	{
		return this->nearestTask(ug->getCenter(), idealTasks);
	}
	if(goodTasks.size() > 0)
	{
		return this->nearestTask(ug->getCenter(), goodTasks);
	}
	if(badTasks.size() > 0)
	{
		return this->nearestTask(ug->getCenter(), badTasks);
	}
	// hier mag hij niet komen
	return Task(ScoutTask, ug->getCenter());
}

bool TaskAssigner::taskIsAssigned(Task task)
{
	for each(std::pair<UnitGroup*, Task> assignment in this->plan)
	{
		if(assignment.second == task)
		{
			return true;
		}
	}
	return false;
}

Task TaskAssigner::nearestTask(BWAPI::Position pos, std::list<Task> tasks)
{
	Task nearest;
	double distance = -1;

	for(std::list<Task>::iterator it=tasks.begin(); it!=tasks.end(); it++)
	{
		double currentDistance = pos.getDistance((*it).getPosition());
		if(distance == -1)
		{
			nearest = (*it);
			distance = currentDistance;
		}
		else if(currentDistance < distance)
		{
			nearest = (*it);
			distance = currentDistance;
		}
	}
	return nearest;
}

int TaskAssigner::countDronesOnTask(Task task)
{
	int result = 0;
	for each(std::pair<BWAPI::Unit*, Task> assignment in this->getDronePlan())
	{
		if(assignment.second == task)
		{
			result = result+1;
		}
	}
	return result;
}

Task TaskAssigner::getTaskOfUnit(BWAPI::Unit* unit)
{
	UnitGroup* ug = this->hc->eiugm->getGroupOfUnit(unit);
	if(this->hc->eiugm->groupIsDroneUG(ug))
	{
		for each(std::pair<BWAPI::Unit*, Task> assignment in this->getDronePlan())
		{
			if(assignment.first == unit)
			{
				return assignment.second;
			}
		}
	}
	else
	{
		for each(std::pair<UnitGroup*, Task> assignment in this->getPlan())
		{
			if(assignment.first == ug)
			{
				return assignment.second;
			}
		}
	}
	return Task(NoTask, unit->getPosition());
}

void TaskAssigner::onUnitDestroy(Unit* u)
{
	if(u->getType() == BWAPI::UnitTypes::Zerg_Drone && u->getPlayer() == BWAPI::Broodwar->self())
	{
		this->dronePlan.erase(u);
	}
}

void TaskAssigner::onUnitMorph(Unit* u)
{
	if(u->getType().isBuilding() && u->getPlayer() == BWAPI::Broodwar->self())
	{
		this->dronePlan.erase(u);
	}
}
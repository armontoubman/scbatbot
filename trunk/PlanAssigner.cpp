#include "PlanAssigner.h"
#include <BWAPI.h>
#include <map>
#include "Util.h"
#include "HighCommand.h"
#include "TaskManager.h"
#include "EigenUnitGroupManager.h"
#include "EnemyUnitDataManager.h"
#include "MicroManager.h"
#include "UnitGroup.h"
#include "Task.h"

PlanAssigner::PlanAssigner(HighCommand* h, TaskManager* t, EigenUnitGroupManager* e, EnemyUnitDataManager* ed, MicroManager* m)
{
	this->hc = h;
	this->tm = t;
	this->eiugm = e;
	this->eudm = ed;
	this->mm = m;
}

std::map<UnitGroup*, Task> PlanAssigner::maakPlan()
{
	logc("PA::maakPlan()\n");
	std::list<Task> tasklist = this->tm->tasklist;
	std::set<UnitGroup*> unitgroupset = this->eiugm->unitGroups;
	std::map<UnitGroup*, Task> currentPlan;

	for each(UnitGroup* ug in unitgroupset)
	{
		if(ug == this->eiugm->droneUG || ug->size() == 0)
		{
			continue;
		}
		logc("PA for each\n");
		if(ug->size()<3)
		{
			logc("PA if\n");
			if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Zergling) || this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Drone))
			{
				logc("PA if if\n");
				currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 1, currentPlan)));
			}
			else
			{
				logc("PA if else\n");
				if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord))
				{
					logc("PA if else if\n");
					if(this->tm->existsTaskWithType(4))
					{
						logc("PA if else if if\n");
						currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 4, currentPlan)));
					}
					else
					{
						logc("PA if else if else\n");
						currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 1, currentPlan)));
					}
				}
				else
				{
					logc("PA if else else\n");
					currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 1, currentPlan)));
				}
			}
		}
		else
		{
			logc("PA else\n");
			currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 0, currentPlan, true)));
		}
	}
	logc("PA::maakPlan() einde\n");
	return currentPlan;
}

Task PlanAssigner::mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan)
{
	return mostAppropriate(current, tasktype, currentPlan, false);
}

Task PlanAssigner::mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan, bool nullwaarde)
{
	logc("PA::mostAppropriate()\n");
	logc(" voor: ");
	logc(this->hc->eigenUnitGroupManager->getName(current).append("\n").c_str());
	std::list<Task> idealTasks;
	std::list<Task> appropriateTasks;
	std::list<Task> lessAppropriateTasks;

	std::list<Task> originalTasks = this->tm->tasklist;
	for each(Task otask in originalTasks)
	{
		logc("ma for each\n");
		if(nullwaarde || !containsTask(currentPlan, otask))
		{
			logc("ma if nullwaarde\n");
			if(tasktype == 0)
			{
				if(otask.type == 2 || otask.type == 3)
				{
					logc("ma if 0523\n");
					if(canReach(current, otask.position))
					{
						logc("ma if !air && connected\n");
						if(otask.enemySize == 0)
						{
							lessAppropriateTasks.push_front(otask);
						}
						else
						{
							logc("ma enemy=null||size=0\n");
							//int wk = canAttack(current, otask);
							//int zk = canAttack(otask, current);

							logc("ma wk!=0\n");
							if(current->size() > 5)
							{
								logc("ma size>9\n");
								if(otask.enemyMilitarySize>4) 
								{
									logc("ma mil>6\n");
									idealTasks.push_front(otask);
								}
								else
								{
									logc("ma size>6\n");
									appropriateTasks.push_front(otask);
								}
							}
							else
							{
								logc("ma currentsize <= 9\n");
								if(otask.enemyMilitarySize > 4)
								{
									logc("ma zk2 wk!2 else\n");
									appropriateTasks.push_front(otask);
								}
								else
								{
									logc("ma militar>6 else\n");
									idealTasks.push_front(otask);
								}
							}
						}
					}
				}
				else
				{
					if (otask.type == 5)
					{
						appropriateTasks.push_front(otask);
					}
					else
					{
						if (otask.type == 1)
						{
							lessAppropriateTasks.push_front(otask);
						}
					}
				}
			}
			else
			{
				if (tasktype == 4) // kan enkel overlord zijn
				{
					if (otask.type == 1 && canReach(current, otask.position))
					{
						appropriateTasks.push_front(otask);
					}
					else
					{
						if (otask.type == 4 && canReach(current, otask.position))
						{
							idealTasks.push_front(otask);
							
						}
						else
						{
							if ((otask.type == 5 || otask.type == 2 || otask.type == 3) && canReach(current, otask.position))
							{
								lessAppropriateTasks.push_front(otask);
							}
						}

					}
				}
				else // type task == 2 -> scout dus) // of al het andere
				{
					if (otask.type == 1 && canReach(current, otask.position))
					{
						idealTasks.push_front(otask);
					}
				}
			}
		}
		else
		{
			logc("ma niet null\n");
			lessAppropriateTasks.push_front(otask);
		}
	}
	if(idealTasks.size() > 0)
	{
		logc("PA::mostAppropriate() idealTasks >0\n");
		return this->tm->nearestTask(current, idealTasks);
	}
	if(appropriateTasks.size() > 0)
	{
		logc("PA::mostAppropriate() appropriateTasks >0\n");
		return this->tm->nearestTask(current, appropriateTasks);
	}
	if(lessAppropriateTasks.size() > 0)
	{
		logc("PA::mostAppropriate() lessAppropriateTasks >0\n");
		return this->tm->nearestTask(current, lessAppropriateTasks);
	}
	log("PA::mostAppropriate() laatste return, hier mag hij nooit komen\n");
	lessAppropriateTasks.push_front(Task(5, this->hc->hatchery->getPosition()));
	return this->tm->nearestTask(current, lessAppropriateTasks);
}

bool PlanAssigner::containsTask(std::map<UnitGroup*, Task> plan, Task t)
{
	for each(std::pair<UnitGroup*, Task> assignment in plan)
	{
		if(assignment.second == t)
		{
			return true;
		}
	}
	return false;
}

double PlanAssigner::logicaldistance(UnitGroup* ug, BWAPI::Position pos)
{
	BWAPI::Position center = ug->getCenter();
	if(this->eiugm->onlyAirUnits(*ug))
	{
		return center.getDistance(pos);
	}
	else
	{
		return BWTA::getGroundDistance(BWAPI::TilePosition(center), BWAPI::TilePosition(pos));
	}
}

void PlanAssigner::update()
{
	logc("PA:update()\n");
	logc(this->hc->wantBuildManager->intToString(this->plan.size()).append("\n").c_str());
	//this->plan = maakPlan();
	maakPlan();
	logc("PA:update() eind\n");
	logc(this->hc->wantBuildManager->intToString(this->plan.size()).append("\n").c_str());
}

Task PlanAssigner::vindTask(std::map<UnitGroup*, Task> lijst, UnitGroup* ug)
{
	log(this->hc->eigenUnitGroupManager->getName(ug).append("\n").c_str());
	for each(std::pair<UnitGroup*, Task> paar in lijst)
	{
		//logc("plan iteratie\n");
		if(paar.first == ug)
		{
			//log("PA::vindTask2() bijbehorende task gevonden\n");
			return paar.second;
		}
	}

	//log("PA::vindTask2() geen task gevonden, geef defend hatchery\n");

	return Task(-1, ug->getCenter());
}

std::list<Task> PlanAssigner::findTasksWithType(std::map<UnitGroup*, Task> lijst, int t)
{
	std::list<Task> result;
	for each(std::pair<UnitGroup*, Task> paar in this->plan)
	{
		if(paar.second.type == t)
		{
			result.push_front(paar.second);
		}
	}
	return result;
}

int PlanAssigner::canAttack(UnitGroup* ug, Task t)
{
	if(ug == NULL || ug->size() == 0)
	{
		return 0;
	}
	if((*ug)(isFlyer).size() > 0)
	{
		return 2;
	}
	if((*ug)(isFlyer).size() == 0 && t.enemyContainsAir)
	{
		if(t.enemyContainsGround)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

int PlanAssigner::canAttack(Task t, UnitGroup* ug)
{
	if(ug == NULL || ug->size() == 0)
	{
		return 0;
	}
	if(t.enemyContainsAir)
	{
		return 2;
	}
	if(t.enemyContainsAir == false && (*ug)(isFlyer).size() > 0)
	{
		if((*ug).not(isFlyer).size() > 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

bool PlanAssigner::canReach(UnitGroup* ug, BWAPI::Position pos)
{
	if(!this->eiugm->onlyAirUnits(*ug) && BWTA::isConnected((*ug->begin())->getTilePosition(), BWAPI::TilePosition(pos)))
	{
		return true;
	}
	if(this->eiugm->onlyAirUnits(*ug))
	{
		return true;
	}
	return false;
}

void PlanAssigner::logc(const char* msg)
{
	if(true)
	{
		log(msg);
	}
}
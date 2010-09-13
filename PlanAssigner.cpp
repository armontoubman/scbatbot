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
	std::set<Task> tasklist = this->tm->tasklist;
	std::set<UnitGroup*> unitgroupset = this->eiugm->unitGroups;
	std::map<UnitGroup*, Task> currentPlan;

	for each(UnitGroup* ug in unitgroupset)
	{
		if(ug == this->eiugm->droneUG)
		{
			continue;
		}
		logc("PA for each\n");
		if(ug->size()<2 && ug != this->eiugm->defendlingUG && ug != this->eiugm->defendgroepUG)
		{
			logc("PA if\n");
			if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Zergling) || this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord)) // nieuwe functie
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
					if(this->tm->existsTaskWithType(4)) // nieuwe functie
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
					currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 5, currentPlan, true)));
				}
			}
		}
		else
		{
			logc("PA else\n");
			if(ug == this->eiugm->defendgroepUG || ug == this->eiugm->defendlingUG || ug == this->eiugm->defendmutaUG)
			{
				logc("PA else if\n");
				currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 5, currentPlan, true)));
			}
			else
			{
				logc("PA else else\n");
				if(this->tm->existsTaskWithPriority(5))
				{
					logc("PA else else if\n");
					currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 5, currentPlan, true)));
				}
				else
				{
					logc("PA else else else\n");
					currentPlan.insert(std::make_pair(ug, mostAppropriate(ug, 0, currentPlan, true)));
				}
			}
		}
	}
	logc("PA::maakPlan() einde\n");
	return currentPlan;
}

int PlanAssigner::canAttack(UnitGroup* ug1, UnitGroup* ug2)
{
	int eigen = 2;
	if(ug2 == NULL) {
		logc("ca ug2=null\n");
		return 2;
	}
	if(ug1 == NULL) {
		logc("ca ug1=null\n");
		return 0;
	}
	if((*ug1->begin())->getPlayer() == BWAPI::Broodwar->self())
	{
		logc("ca ug1=self\n");
		eigen = 1;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug2);
		logc("ca ug1=self, ok opgezocht\n");
		if(this->mm->canAttackAir(*ug1) && this->mm->canAttackGround(*ug1) || this->mm->canAttackAir(*ug1) && this->eudm->onlyAirUnits(data) || this->mm->canAttackGround(*ug1) && this->eudm->onlyGroundUnits(data))
		{
			logc("ca ug1=self airconditie\n");
			return 2;
		}
		else
		{
			if(!this->eudm->onlyGroundUnits(data) && !this->eudm->onlyAirUnits(data))
			{
				logc("ca ug1=self mixed\n");
				return 1;
			}
			return 0;
		}
	}
	else
	{
		logc("ca ug2=self\n");
		eigen = 2;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug1);
		logc("ca ug2=self, ok opgezocht\n");
		if(this->eudm->canAttackAir(data) && this->eudm->canAttackGround(data) || this->eudm->canAttackAir(data) && (*ug2)(isFlyer).size() == ug2->size() || this->eudm->canAttackGround(data) && (*ug2)(isFlyer).size() == 0)
		{
			logc("ca ug2=self airconditie\n");
			return 2;
		}
		else
		{
			if((*ug2)(isFlyer).size() > 0 && (*ug2).not(isFlyer).size() > 0)
			{
				logc("ca ug2=self mixed\n");
				return 1;
			}
			return 0;
		}
	}
}

Task PlanAssigner::mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan)
{
	return mostAppropriate(current, tasktype, currentPlan, false);
}

Task PlanAssigner::mostAppropriate(UnitGroup* current, int tasktype, std::map<UnitGroup*, Task> currentPlan, bool nullwaarde)
{
	logc("PA::mostAppropriate()\n");
	std::set<Task> idealTasks;
	std::set<Task> appropriateTasks;
	std::set<Task> lessAppropriateTasks;

	std::set<Task> originalTasks = this->tm->tasklist;
	for each(Task otask in originalTasks)
	{
		logc("ma for each\n");
		if(nullwaarde || !containsTask(currentPlan, otask)) // nieuwe functie
		{
			logc("ma if nullwaarde\n");
			if(tasktype == 0 && (otask.type == 5 || otask.type == 2 || otask.type == 3))
			{
				logc("ma if 0523\n");
				if((!this->eiugm->onlyAirUnits(*current) && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getTilePosition())) || this->eiugm->onlyAirUnits(*current)) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
				{
					logc("ma if !air && connected\n");
					if(otask.enemy == NULL)
					{
						lessAppropriateTasks.insert(otask);
					}
					else
					{
						//if(otask.enemy == NULL || otask.enemy->size() == 0) // <---- hier staat de ==NULL check, omdat als ie null hij zal crash op ->size()
						logc("ma enemy=null||size=0\n");
						int wk = canAttack(current, otask.enemy);
						int zk = canAttack(otask.enemy, current);

						if(wk!=0)
						{
							logc("ma wk!=0\n");
							if(current->size() > 9)
							{
								logc("ma size>9\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy)>6) // nullpointers
								{
									logc("ma mil>6\n");
									idealTasks.insert(otask);
								}
								else
								{
									logc("ma else\n");
									if(otask.enemy->size()>6)
									{
										logc("ma size>6\n");
										appropriateTasks.insert(otask);
									}
									else
									{
										logc("ma else !>6\n");
										lessAppropriateTasks.insert(otask);
									}
								}
							}
							else
							{
								logc("ma currentsize <= 9\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy) > 6)
								{
									logc("ma military>6\n");
									if(zk==2 || wk!=2)
									{
										logc("ma zk2 wk!2\n");
										lessAppropriateTasks.insert(otask);
									}
									else
									{
										logc("ma zk2 wk!2 else\n");
										appropriateTasks.insert(otask);
									}
								}
								else
								{
									logc("ma militar>6 else\n");
									idealTasks.insert(otask);
								}
							}
						}
					}
				}
			}
			else
			{
				logc("ma wel enemy\n");
				if(tasktype == 5 && otask.type == 5)
				{
					logc("ma task 5\n");
					if((!this->eiugm->onlyAirUnits(*current) 
						&& (otask.enemy != NULL && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getPosition())) )
						|| (this->eiugm->onlyAirUnits(*current))) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
					{
						logc("ma air\n");
						int wk=canAttack(current, otask.enemy);
						int zk=canAttack(otask.enemy, current);
						if(wk!=0)
						{
							logc("ma !wk=0\n");
							if(logicaldistance(current, otask.position)<=(this->mm->dist(20)))
							{
								logc("ma logicaldist\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy) >6)
								{
									logc("ma mil>6\n");
									if(current->size()>6 || zk==0)
									{
										logc("ma cur>6\n");
										idealTasks.insert(otask);
									}
									else
									{
										logc("ma cur!>6\n");
										appropriateTasks.insert(otask);
									}
								}
								else
								{
									logc("ma mil!>6\n");
									if(current->size()>9 || zk==2)
									{
										logc("ma size>9 zk=2\n");
										lessAppropriateTasks.insert(otask);
									}
									else
									{
										logc("ma ook niet\n");
										idealTasks.insert(otask);
									}
								}
							}
							else
							{
								logc("ma illogical\n");
								lessAppropriateTasks.insert(otask);
							}
						}
					}
				}
				else
				{
					logc("ma niet 5\n");
					if(tasktype == 4) // zeker weten?
					{
						logc("ma 4\n");
						if(otask.type == 1)
						{
							logc("ma 1\n");
							appropriateTasks.insert(otask);
						}
						else
						{
							logc("ma niet 1\n");
							if(otask.type == 4)
							{
								logc("ma toch wel 4\n");
								idealTasks.insert(otask);
							}
						}
					}
					else // type task == 2 -> scout dus) // alleen is scout 1 hier
					{
						logc("ma niet 4\n");
						if(otask.type == 1)
						{
							logc("ma 1 dan\n");
							idealTasks.insert(otask);
						}
					}
				}
			}
		}
		else
		{
			logc("ma niet null\n");
			if(otask.type == 5 || otask.type == 2)
			{
				logc("ma 5 of 2\n");
				lessAppropriateTasks.insert(otask);
			}
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
	logc("PA::mostAppropriate() laatste return, hier mag hij nooit komen\n");
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

Task PlanAssigner::vindTask(UnitGroup* ug)
{
	logc("PA::vindTask()\n");
	int lolsize = this->hc->hcplan.size();
	logc("PAlolololool\n");
	logc(this->hc->wantBuildManager->intToString(lolsize).append("\n").c_str());
	if(lolsize == 0)
	{
		logc("plan is leeg...\n");
	}
	if(lolsize > 0) {
		logc("plan is niet leeg\n");
	}
	/*for each(std::pair<UnitGroup*, Task> paar in this->plan)
	{
		logc("plan iteratie\n");
		if(paar.first == ug)
		{
			logc("PA::vindTask() bijbehorende task gevonden\n");
			return paar.second;
		}
	}*/

	logc("PA::vindTask() geen task gevonden, geef defend hatchery\n");

	return this->hc->hcplan[ug];

	//return Task(-1, 1, this->hc->hatchery->getPosition());
}

Task PlanAssigner::vindTask(std::map<UnitGroup*, Task> lijst, UnitGroup* ug)
{
	logc("PA::vindTask2()\n");
	int lolsize = lijst.size();
	logc(this->hc->wantBuildManager->intToString(lolsize).append("\n").c_str());
	if(lolsize == 0)
	{
		logc("plan is leeg...\n");
	}
	if(lolsize > 0)
	{
		logc("plan is niet leeg\n");
	}
	for each(std::pair<UnitGroup*, Task> paar in lijst)
	{
		logc("plan iteratie\n");
		if(paar.first == ug)
		{
			logc("PA::vindTask2() bijbehorende task gevonden\n");
			return paar.second;
		}
	}

	logc("PA::vindTask2() geen task gevonden, geef defend hatchery\n");

	return Task(-1, 1, this->hc->hatchery->getPosition());
}

std::set<Task> PlanAssigner::findTasksWithType(std::map<UnitGroup*, Task> lijst, int t)
{
	std::set<Task> result;
	for each(std::pair<UnitGroup*, Task> paar in this->plan)
	{
		if(paar.second.type == t)
		{
			result.insert(paar.second);
		}
	}
	return result;
}

void PlanAssigner::logc(const char* msg)
{
	if(false)
	{
		log(msg);
	}
}
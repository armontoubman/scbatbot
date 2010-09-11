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
	log("PA::maakPlan()\n");
	std::set<Task> tasklist = this->tm->tasklist;
	std::set<UnitGroup*> unitgroupset = this->eiugm->unitGroups;
	std::map<UnitGroup*, Task> currentPlan;

	for each(UnitGroup* ug in unitgroupset)
	{
		log("PA for each\n");
		if(ug->size()<2 && ug != this->eiugm->defendlingUG && ug != this->eiugm->defendgroepUG)
		{
			log("PA if\n");
			if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Zergling) || this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord)) // nieuwe functie
			{
				log("PA if if\n");
				currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 1, currentPlan)));
			}
			else
			{
				log("PA if else\n");
				if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord))
				{
					log("PA if else if\n");
					if(this->tm->existsTaskWithType(4)) // nieuwe functie
					{
						log("PA if else if if\n");
						currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 4, currentPlan)));
					}
					else
					{
						log("PA if else if else\n");
						currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 1, currentPlan)));
					}
				}
				else
				{
					log("PA if else else\n");
					currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 5, currentPlan, true)));
				}
			}
		}
		else
		{
			log("PA else\n");
			if(ug == this->eiugm->defendgroepUG || ug == this->eiugm->defendlingUG || ug == this->eiugm->defendmutaUG)
			{
				log("PA else if\n");
				currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 5, currentPlan, true)));
			}
			else
			{
				log("PA else else\n");
				if(this->tm->existsTaskWithPriority(5))
				{
					log("PA else else if\n");
					currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 5, currentPlan, true)));
				}
				else
				{
					log("PA else else else\n");
					currentPlan.insert(std::pair<UnitGroup*, Task>(ug, mostAppropriate(ug, 0, currentPlan, true)));
				}
			}
		}
	}
	log("PA::maakPlan() einde\n");
	return currentPlan;
}

int PlanAssigner::canAttack(UnitGroup* ug1, UnitGroup* ug2)
{
	int eigen = 2;
	if(ug2 == NULL) {
		log("ca ug2=null\n");
		return 2;
	}
	if(ug1 == NULL) {
		log("ca ug1=null\n");
		return 0;
	}
	if((*ug1->begin())->getPlayer() == BWAPI::Broodwar->self())
	{
		log("ca ug1=self\n");
		eigen = 1;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug2);
		if(this->mm->canAttackAir(*ug1) && this->mm->canAttackGround(*ug1) || this->mm->canAttackAir(*ug1) && this->eudm->onlyAirUnits(data) || this->mm->canAttackGround(*ug1) && this->eudm->onlyGroundUnits(data))
		{
			log("ca ug1=self airconditie\n");
			return 2;
		}
		else
		{
			if(!this->eudm->onlyGroundUnits(data) && !this->eudm->onlyAirUnits(data))
			{
				log("ca ug1=self mixed\n");
				return 1;
			}
			return 0;
		}
	}
	else
	{
		log("ca ug2=self\n");
		eigen = 2;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug1);
		if(this->eudm->canAttackAir(data) && this->eudm->canAttackGround(data) || this->eudm->canAttackAir(data) && (*ug2)(isFlyer).size() == ug2->size() || this->eudm->canAttackGround(data) && (*ug2)(isFlyer).size() == 0)
		{
			log("ca ug2=self airconditie\n");
			return 2;
		}
		else
		{
			if((*ug2)(isFlyer).size() > 0 && (*ug2).not(isFlyer).size() > 0)
			{
				log("ca ug2=self mixed\n");
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
	log("PA::mostAppropriate()\n");
	std::set<Task> idealTasks;
	std::set<Task> appropriateTasks;
	std::set<Task> lessAppropriateTasks;

	std::set<Task> originalTasks = this->tm->tasklist;
	for each(Task otask in originalTasks)
	{
		log("ma for each\n");
		if(nullwaarde || !containsTask(currentPlan, otask)) // nieuwe functie
		{
			log("ma if nullwaarde\n");
			if(tasktype == 0 && (otask.type == 5 || otask.type == 2 || otask.type == 3))
			{
				log("ma if 0523\n");
				if((!this->eiugm->onlyAirUnits(*current) && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getTilePosition())) || this->eiugm->onlyAirUnits(*current)) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
				{
					log("ma if !air && connected\n");
					if(otask.enemy == NULL)
					{
						lessAppropriateTasks.insert(otask);
					}
					else
					{
						//if(otask.enemy == NULL || otask.enemy->size() == 0) // <---- hier staat de ==NULL check, omdat als ie null hij zal crash op ->size()
						log("ma enemy=null||size=0\n");
						int wk = canAttack(current, otask.enemy);
						int zk = canAttack(otask.enemy, current);

						if(wk!=0)
						{
							log("ma wk!=0\n");
							if(current->size() > 9)
							{
								log("ma size>9\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy)>6) // nullpointers
								{
									log("ma mil>6\n");
									idealTasks.insert(otask);
								}
								else
								{
									log("ma else\n");
									if(otask.enemy->size()>6)
									{
										log("ma size>6\n");
										appropriateTasks.insert(otask);
									}
									else
									{
										log("ma else !>6\n");
										lessAppropriateTasks.insert(otask);
									}
								}
							}
							else
							{
								log("ma currentsize <= 9\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy) > 6)
								{
									log("ma military>6\n");
									if(zk==2 || wk!=2)
									{
										log("ma zk2 wk!2\n");
										lessAppropriateTasks.insert(otask);
									}
									else
									{
										log("ma zk2 wk!2 else\n");
										appropriateTasks.insert(otask);
									}
								}
								else
								{
									log("ma militar>6 else\n");
									idealTasks.insert(otask);
								}
							}
						}
					}
				}
			}
			else
			{
				log("ma wel enemy\n");
				if(tasktype == 5 && otask.type == 5)
				{
					log("ma task 5\n");
					if((!this->eiugm->onlyAirUnits(*current) && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getPosition())) || (this->eiugm->onlyAirUnits(*current))) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
					{
						log("ma air\n");
						int wk=canAttack(current, otask.enemy);
						int zk=canAttack(otask.enemy, current);
						if(wk!=0)
						{
							log("ma !wk=0\n");
							if(logicaldistance(current, otask.position)<=(this->mm->dist(20)))
							{
								log("ma logicaldist\n");
								if(this->eudm->nrMilitaryUnits(*otask.enemy) >6)
								{
									log("ma mil>6\n");
									if(current->size()>6 || zk==0)
									{
										log("ma cur>6\n");
										idealTasks.insert(otask);
									}
									else
									{
										log("ma cur!>6\n");
										appropriateTasks.insert(otask);
									}
								}
								else
								{
									log("ma mil!>6\n");
									if(current->size()>9 || zk==2)
									{
										log("ma size>9 zk=2\n");
										lessAppropriateTasks.insert(otask);
									}
									else
									{
										log("ma ook niet\n");
										idealTasks.insert(otask);
									}
								}
							}
							else
							{
								log("ma illogical\n");
								lessAppropriateTasks.insert(otask);
							}
						}
					}
				}
				else
				{
					log("ma niet 5\n");
					if(tasktype == 4) // zeker weten?
					{
						log("ma 4\n");
						if(otask.type == 1)
						{
							log("ma 1\n");
							appropriateTasks.insert(otask);
						}
						else
						{
							log("ma niet 1\n");
							if(otask.type == 4)
							{
								log("ma toch wel 4\n");
								idealTasks.insert(otask);
							}
						}
					}
					else // type task == 2 -> scout dus) // alleen is scout 1 hier
					{
						log("ma niet 4\n");
						if(otask.type == 1)
						{
							log("ma 1 dan\n");
							idealTasks.insert(otask);
						}
					}
				}
			}
		}
		else
		{
			log("ma niet null\n");
			if(otask.type == 5 || otask.type == 2)
			{
				log("ma 5 of 2\n");
				lessAppropriateTasks.insert(otask);
			}
		}
	}
	if(idealTasks.size() > 0)
	{
		log("PA::mostAppropriate() idealTasks >0\n");
		return this->tm->nearestTask(current, idealTasks);
	}
	if(appropriateTasks.size() > 0)
	{
		log("PA::mostAppropriate() appropriateTasks >0\n");
		return this->tm->nearestTask(current, appropriateTasks);
	}
	if(lessAppropriateTasks.size() > 0)
	{
		log("PA::mostAppropriate() lessAppropriateTasks >0\n");
		return this->tm->nearestTask(current, lessAppropriateTasks);
	}
	log("PA::mostAppropriate() laatste return, hier mag hij nooit komen\n");
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
	log("PA:update()\n");
	log(this->hc->wantBuildManager->intToString(this->plan.size()).append("\n").c_str());
	this->plan = maakPlan();
	log("PA:update() eind\n");
	log(this->hc->wantBuildManager->intToString(this->plan.size()).append("\n").c_str());
}

Task PlanAssigner::vindTask(UnitGroup* ug)
{
	log("PA::vindTask()\n");
	int lolsize = this->hc->planAssigner->plan.size();
	log("PAlolololool\n");
	log(this->hc->wantBuildManager->intToString(lolsize).append("\n").c_str());
	if(lolsize == 0) { log("plan is leeg...\n"); }
	log("lolprint1\n");
	if(lolsize > 0) { log("plan is niet leeg\n"); }
	log("lolprint2\n");
	for each(std::pair<UnitGroup*, Task> paar in this->plan)
	{
		log("plan iteratie\n");
		if(paar.first == ug)
		{
			log("PA::vindTask() bijbehorende task gevonden\n");
			return paar.second;
		}
	}
	log("PA::vindTask() geen task gevonden, geef defend hatchery\n");
	return Task(5, 1, this->hc->hatchery->getPosition());
}
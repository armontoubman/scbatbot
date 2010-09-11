#include "PlanAssigner.h"
#include <BWAPI.h>
#include <map>

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
	std::set<Task> tasklist = this->tm->tasklist;
	std::set<UnitGroup*> unitgroupset = this->eiugm->unitGroups;
	std::map<UnitGroup*, Task> currentPlan;

	for each(UnitGroup* ug in unitgroupset)
	{
		if(ug->size()<2 && ug != this->eiugm->defendlingUG && ug != this->eiugm->defendgroepUG)
		{
			if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Zergling) || this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord)) // nieuwe functie
			{
				addAssignment(ug, mostAppropriate(ug, 1, currentPlan), &currentPlan);
			}
			else
			{
				if(this->eiugm->groupContainsType(ug, BWAPI::UnitTypes::Zerg_Overlord))
				{
					if(this->tm->existsTaskWithType(4)) // nieuwe functie
					{
						addAssignment(ug, mostAppropriate(ug, 4, currentPlan), &currentPlan);
					}
					else
					{
						addAssignment(ug, mostAppropriate(ug, 1, currentPlan), &currentPlan);
					}
				}
				else
				{
					addAssignment(ug, mostAppropriate(ug, 5, currentPlan, true), &currentPlan);
				}
			}
		}
		else
		{
			if(ug == this->eiugm->defendgroepUG || ug == this->eiugm->defendlingUG || ug == this->eiugm->defendmutaUG)
			{
				addAssignment(ug, mostAppropriate(ug, 5, currentPlan, true), &currentPlan);
			}
			else
			{
				if(this->tm->existsTaskWithPriority(5))
				{
					addAssignment(ug, mostAppropriate(ug, 5, currentPlan, true), &currentPlan);
				}
				else
				{
					addAssignment(ug, mostAppropriate(ug, 0, currentPlan, true), &currentPlan);
				}
			}
		}
	}
	return currentPlan;
}

int PlanAssigner::canAttack(UnitGroup* ug1, UnitGroup* ug2)
{
	int eigen = 2;
	if(ug2 == NULL) {
		return 2;
	}
	if((*ug1->begin())->getPlayer() == BWAPI::Broodwar->self())
	{
		eigen = 1;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug2);
		if(this->mm->canAttackAir(*ug1) && this->mm->canAttackGround(*ug1) || this->mm->canAttackAir(*ug1) && this->eudm->onlyAirUnits(data) || this->mm->canAttackGround(*ug1) && this->eudm->onlyGroundUnits(data))
		{
			return 2;
		}
		else
		{
			if(!this->eudm->onlyGroundUnits(data) && !this->eudm->onlyAirUnits(data))
			{
				return 1;
			}
			return 0;
		}
	}
	else
	{
		eigen = 2;
		std::map<BWAPI::Unit*, EnemyUnitData> data = this->eudm->getMapFromUG(ug1);
		if(this->eudm->canAttackAir(data) && this->eudm->canAttackGround(data) || this->eudm->canAttackAir(data) && (*ug2)(isFlyer).size() == ug2->size() || this->eudm->canAttackGround(data) && (*ug2)(isFlyer).size() == 0)
		{
			return 2;
		}
		else
		{
			if((*ug2)(isFlyer).size() > 0 && (*ug2).not(isFlyer).size() > 0)
			{
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
	std::set<Task> idealTasks;
	std::set<Task> appropriateTasks;
	std::set<Task> lessAppropriateTasks;

	std::set<Task> originalTasks = this->tm->tasklist;

	for each(Task otask in originalTasks)
	{
		if(nullwaarde || !containsTask(currentPlan, otask)) // nieuwe functie
		{
			if(tasktype == 0 || tasktype == 5 || tasktype == 2 || tasktype == 3)
			{
				if(!this->eiugm->onlyAirUnits(*current) && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getTilePosition()) || this->eiugm->onlyAirUnits(*current)) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
				{
					if(otask.enemy == NULL || otask.enemy->size() == 0)
					{
						int wk = canAttack(current, otask.enemy);
						int zk = canAttack(otask.enemy, current);

						if(wk!=0)
						{
							if(current->size() > 9)
							{
								if(this->eudm->nrMilitaryUnits(*otask.enemy)>6) // nullpointers
								{
									idealTasks.insert(otask);
								}
								else
								{
									if(otask.enemy->size()>6)
									{
										appropriateTasks.insert(otask);
									}
									else
									{
										lessAppropriateTasks.insert(otask);
									}
								}
							}
							else
							{
								if(this->eudm->nrMilitaryUnits(*otask.enemy) > 6)
								{
									if(zk==2 || wk!=2)
									{
										lessAppropriateTasks.insert(otask);
									}
									else
									{
										appropriateTasks.insert(otask);
									}
								}
								else
								{
									idealTasks.insert(otask);
								}
							}
						}
					}
					else
					{
						if(tasktype == 5 && otask.type == 5)
						{
							if(!this->eiugm->onlyAirUnits(*current) && BWTA::isConnected((*current->begin())->getTilePosition(), (*otask.enemy->begin())->getPosition()) || this->eiugm->onlyAirUnits(*current)) // nieuwe functie, en enemybegin mogelijk ongeldig KAN NULL ZIJN
							{
								int wk=canAttack(current, otask.enemy);
								int zk=canAttack(otask.enemy, current);
								if(wk!=0)
								{
									if(logicaldistance(current, otask.position)<=20)
									{
										if(this->eudm->nrMilitaryUnits(*otask.enemy))
										{
											if(current->size()>6 || zk==0)
											{
												idealTasks.insert(otask);
											}
											else
											{
												appropriateTasks.insert(otask);
											}
										}
										else
										{
											if(current->size()>9 || zk==2)
											{
												lessAppropriateTasks.insert(otask);
											}
											else
											{
												idealTasks.insert(otask);
											}
										}
									}
									else
									{
										lessAppropriateTasks.insert(otask);
									}
								}
							}
						}
						else
						{
							if(tasktype == 4) // zeker weten?
							{
								if(otask.type == 1)
								{
									appropriateTasks.insert(otask);
								}
								else
								{
									if(otask.type == 4)
									{
										idealTasks.insert(otask);
									}
								}
							}
							else // type task == 2 -> scout dus) // alleen is scout 1 hier
							{
								if(otask.type == 1)
								{
									idealTasks.insert(otask);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(otask.type == 5 || otask.type == 2)
			{
				lessAppropriateTasks.insert(otask);
			}
		}
	}
	if(idealTasks.size() > 0)
	{
		return this->tm->nearestTask(current, idealTasks);
	}
	if(appropriateTasks.size() > 0)
	{
		return this->tm->nearestTask(current, appropriateTasks);
	}
	if(lessAppropriateTasks.size() > 0)
	{
		return this->tm->nearestTask(current, lessAppropriateTasks);
	}
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

void PlanAssigner::addAssignment(UnitGroup* ug, Task t, std::map<UnitGroup*, Task>* plan)
{
	plan->insert(std::pair<UnitGroup*, Task>(ug, t));
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
	this->plan = maakPlan();
}

Task PlanAssigner::vindTask(UnitGroup* ug)
{
	for each(std::pair<UnitGroup*, Task> paar in this->plan)
	{
		if(paar.first == ug)
		{
			return paar.second;
		}
	}
	return Task(5, 1, this->hc->hatchery->getPosition());
}
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

Task TaskManager::findTaskWithUnitGroup(UnitGroup* ug) // moet via plan
{
	for(std::set<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		if((i->unitGroup) == ug) {
			return *i;
		}
	}
	return Task(-1, 1, (*ug->begin())->getPosition(), ug);
}

Task TaskManager::findTaskWithUnit(BWAPI::Unit* unit) // via plan
{
	return this->findTaskWithUnitGroup(this->eugm->findUnitGroupWithUnit(unit));
}

void TaskManager::update()
{
	/*
	types:
	1 scout
	2 combat
	3 prepare
	4 detector
	5 defend
	*/
	this->tasklist.clear();

	std::map<BWAPI::Unit*, EnemyUnitData> datamap = this->eudm->getData();

	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in datamap)
	{
		UnitGroup enemyUG;
		enemyUG.insert(enemy.first);

		if(enemy.second.position != BWAPI::Positions::Unknown)
		{
			std::map<BWAPI::Unit*, EnemyUnitData> inrange = this->eudm->getEnemyUnitsInRadius(10*32, enemy.second.position);
			if(!enemy.first->getType().isBuilding())
			{
				if(BWAPI::Broodwar->getFrameCount() - enemy.second.lastSeen > 10*24)
				{
					if(enemy.second.unitType.isCloakable() || enemy.second.unitType.hasPermanentCloak())
					{
						insertTask(Task(4, 1, enemy.second.position));
					}
					else
					{
						insertTask(Task(1, 1, enemy.second.position));
					}
				}
				else
				{
					if(enemy.second.unitType.isCloakable() || enemy.second.unitType.hasPermanentCloak())
					{
						insertTask(Task(4, 3, enemy.second.position, &enemyUG));
					}
					if(!enemy.second.unitType.isBuilding()
						&& enemy.second.unitType != BWAPI::UnitTypes::Zerg_Overlord
						&& enemy.second.unitType != BWAPI::UnitTypes::Zerg_Drone
						&& enemy.second.unitType != BWAPI::UnitTypes::Terran_SCV
						&& enemy.second.unitType != BWAPI::UnitTypes::Protoss_Probe
						&& enemy.second.unitType != BWAPI::UnitTypes::Protoss_Observer)
					{
						UnitGroup eigenInRange = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(10), enemy.second.position);
						if(eigenInRange.size() > 0)
						{
							insertTask(Task(5, 2, enemy.second.position, &enemyUG));
						}
						else
						{
							insertTask(Task(2, 1, enemy.second.position, &enemyUG));
						}
					}
					else
					{
						UnitGroup eigenInRange = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(10), enemy.second.position);
						if(eigenInRange.size() > 0)
						{
							UnitGroup enemyUG2;
							std::map<BWAPI::Unit*, EnemyUnitData> enemyInRange = this->eudm->getEnemyUnitsInRadius(dist(10), enemy.second.position);
							if(enemyInRange.size() > 4)
							{
								std::set<BWAPI::Position> posset;
								for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemypair in enemyInRange)
								{
									if(enemypair.second.position != BWAPI::Positions::Unknown)
									{
										posset.insert(enemypair.second.position);
										enemyUG2.insert(enemypair.first);
									}
								}
								BWAPI::Position taskpos = enemy.second.position;
								if(posset.size() > 0)
								{
									taskpos = getCenterPosition(posset);
								}
								insertTask(Task(5, 5, taskpos, &enemyUG2));
							}
						}
						else
						{
							UnitGroup enemyBuildingsUG;
							std::map<BWAPI::Unit*, EnemyUnitData> enemyBuildings = this->eudm->getEnemyUnitsInRadius(dist(10), enemy.second.position);
							bool buildingnear = false;
							for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemypair in enemyBuildings)
							{
								if(enemypair.second.unitType.isBuilding())
								{
									buildingnear = true;
								}
								enemyBuildingsUG.insert(enemypair.first);
							}
							if(buildingnear)
							{
								insertTask(Task(2, 2, enemy.second.position, &enemyBuildingsUG));
							}
							else
							{
								std::map<BWAPI::Unit*, EnemyUnitData> enemyInRange = this->eudm->getEnemyUnitsInRadius(dist(10), enemy.second.position);
								UnitGroup enemyUG3;
								for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemypair in enemyInRange)
								{
									if(enemypair.second.position != BWAPI::Positions::Unknown)
									{
										enemyUG3.insert(enemypair.first);
									}
								}
								if(enemy.second.unitType == BWAPI::UnitTypes::Terran_Dropship || enemy.second.unitType == BWAPI::UnitTypes::Protoss_Shuttle)
								{
									UnitGroup eigenbuildings = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isBuilding);
									BWAPI::Unit* nearestbuilding = nearestUnit(enemy.second.position, eigenbuildings);
									insertTask(Task(5, 5, nearestbuilding->getPosition(), &enemyUG3));
								}
								insertTask(Task(2, 4, enemy.second.position, &enemyUG3));
							}
						}
					}
				}
			}
			else
			{
				std::map<BWAPI::Unit*, EnemyUnitData> enemyInRange = this->eudm->getEnemyUnitsInRadius(dist(10), enemy.second.position);
				UnitGroup enemyUG4;
				for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemypair in enemyInRange)
				{
					if(enemypair.second.position != BWAPI::Positions::Unknown)
					{
						enemyUG4.insert(enemypair.first);
					}
				}
				if(BWAPI::Broodwar->getFrameCount() - enemy.second.lastSeen > 30*24)
				{
					insertTask(Task(1, 1, enemy.second.position, &enemyUG4));
				}
				insertTask(Task(2, 2, enemy.second.position, &enemyUG4));
			}
		}
	}

	/*

	//
	add task(defend, 3, nearestalliedstructuretoanenemystructureGebruikGroundDistance, UG=null); // idee is dat de dichtsbijzijnde expansie voor de enemy ook gedekt wordt.	

	// gegarandeerd scouten:
	if numberofEnemyUnits <15 (zover wij weten dus!)
		add task(scout,1,positiedienognietbekekenis*,UG=null); // *=checke hoe bwsal dit doet, die stuurt de scout gewoon van ene naar de andere basis zonder dezelfde te herhalen enzo.
	}

	// add scout task dichtbij de rand van de map die het dichts bij onze enemy is en die van ons.
	***check 4 posities diens gezamenlijke afstand tot de enemy/ally.
	add task(scout,1,mapheight*32,mapwidth*16, UG=null);
	add task(scout,1,mapheight, mapwidth*16, UG=null);
	add task(scout,1,mapheight*16, mapwidth, UG=null);
	add task(scout,1,mapheight*16, mapwidth*32, UG=null);


	// check of er een expansion is gemaakt:
	add task(scout, 1, nearestExpansionToEnemy, UG=null);
	add task(scout, 1, pick random expansion where no allied/enemy units have been built, UG=null);

	*/

}

std::set<Task> TaskManager::findTasksWithUnitType(BWAPI::UnitType unittype) // via plan
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

double TaskManager::dist(int d)
{
	return d*32;
}

BWAPI::Position TaskManager::getCenterPosition(std::set<BWAPI::Position> posset)
{
	int result_x;
	int result_y;
	int cur_x;
	int cur_y;
	double avg_x = 0.0;
	double avg_y = 0.0;
	int aantal = 0;

	for(std::set<BWAPI::Position>::iterator i=posset.begin();i!=posset.end();i++)
	{
			cur_x = (*i).x();
			cur_y = (*i).y();

			avg_x = cur_x + aantal * avg_x / aantal + 1;
			aantal++;
	}

	result_x = int(avg_x);
	result_y = int(avg_y);

	return BWAPI::Position(result_x, result_y);
}

BWAPI::Unit* TaskManager::nearestUnit(BWAPI::Position pos, UnitGroup ug)
{
	double besteAfstand = -1.00;
	BWAPI::Unit* besteUnit = NULL;

	for(std::set<BWAPI::Unit*>::iterator it = ug.begin(); it != ug.end(); it++)
	{
		if(besteAfstand == -1)
		{
			besteAfstand = (*it)->getDistance(pos);
			besteUnit = (*it);
		}
		else
		{
			if((*it)->getDistance(pos) < besteAfstand)
			{
				besteAfstand = (*it)->getDistance(pos);
				besteUnit = (*it);
			}
		}
	}

	return besteUnit;
}
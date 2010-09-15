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
#include "HighCommand.h"
#include "PlanAssigner.h"
#include <BWTA.h>
class Task;
class EigenUnitGroupManager;
TaskManager::TaskManager()
{
}

TaskManager::TaskManager(EigenUnitGroupManager* e, EnemyUnitDataManager* eu, HighCommand* h, PlanAssigner* p)
{
	this->eugm = e;
	this->eudm = eu;
	this->hc = h;
	this->pa = p;
}

void TaskManager::insertTask(Task t)
{
	// tasklist had altijd maar grootte 1
	// tasklist was eerst een set
	// unique = fout
	log("insertTask()\n");
	log("tasklist voor: ");
	log(this->hc->wantBuildManager->intToString(this->tasklist.size()).append("\n").c_str());

	this->tasklist.push_front(t);
	log(" tasklist na: ");
	log(this->hc->wantBuildManager->intToString(this->tasklist.size()).append("\n").c_str());
	/*std::list<Task>::iterator insertposition;
	bool plek = false;
	for(std::list<Task>::iterator i=this->tasklist.begin();i!=this->tasklist.end();i++)
	{
		//log("for\n");
		if((*i).priority > t.priority) {
			//log("for if priority >\n");
			insertposition = i;
			plek = true;
			break;
		}
	}
	if(plek) {
		//log("if plek\n");
		this->tasklist.insert(insertposition, t);
	}
	else {
		//log("else\n");
		this->tasklist.insert(this->tasklist.end(), t);
	}*/

	//BWAPI::Broodwar->printf("%s %d", "Task gemaakt met type", t.type);
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
	return this->pa->vindTask(ug);
}

Task TaskManager::findTaskWithUnit(BWAPI::Unit* unit)
{
	UnitGroup* group = this->eugm->findUnitGroupWithUnit(unit);
	if(group==NULL) log("TM:fTWU group==NULL\n");
	return this->pa->vindTask(group);
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
		std::map<BWAPI::Unit*, EnemyUnitData> enemyMap = this->eudm->getMapFromUG(&enemyUG);

		if(enemy.second.position != BWAPI::Positions::Unknown)
		{
			std::map<BWAPI::Unit*, EnemyUnitData> inrange = this->eudm->getEnemyUnitsInRadius(10*32, enemy.second.position);
			if(!enemy.second.unitType.isBuilding())
			{
				if(BWAPI::Broodwar->getFrameCount() - enemy.second.lastSeen > 10*24)
				{
					if(enemy.second.unitType.isCloakable() || enemy.second.unitType.hasPermanentCloak())
					{
						insertTask(Task(4, enemy.second.position));
					}
					else
					{
						insertTask(Task(1, enemy.second.position));
					}
				}
				else
				{
					if(enemy.second.unitType.isCloakable() || enemy.second.unitType.hasPermanentCloak())
					{
						insertTask(createTask(4, enemy.second.position, enemyMap));
					}
					if((enemy.second.unitType == BWAPI::UnitTypes::Zerg_Overlord
						|| enemy.second.unitType == BWAPI::UnitTypes::Zerg_Drone
						|| enemy.second.unitType == BWAPI::UnitTypes::Terran_SCV
						|| enemy.second.unitType == BWAPI::UnitTypes::Protoss_Probe
						|| enemy.second.unitType == BWAPI::UnitTypes::Protoss_Observer))
					{
						UnitGroup eigenInRange = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isBuilding).inRadius(dist(10), enemy.second.position);
						if(eigenInRange.size() > 0)
						{
							insertTask(createTask(5, enemy.second.position, enemyMap));
						}
						else
						{
							insertTask(createTask(2, enemy.second.position, enemyMap));
						}
					}
					else
					{
						log("tm1\n");
						UnitGroup eigenInRange = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isBuilding).inRadius(dist(10), enemy.second.position);
						log("tm2\n");
						if(eigenInRange.size() > 0)
						{
							UnitGroup enemyUG2;
							std::map<BWAPI::Unit*, EnemyUnitData> enemyInRange = this->eudm->getEnemyUnitsInRadius(dist(10), enemy.second.position);
							log("tm3\n");
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
							if(enemyInRange.size() > 4)
							{
								insertTask(createTask(5, taskpos, this->eudm->getMapFromUG(&enemyUG2)));
							}
							else
							{
								insertTask(createTask(5, taskpos, this->eudm->getMapFromUG(&enemyUG2)));
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
								insertTask(createTask(2, enemy.second.position, this->eudm->getMapFromUG(&enemyBuildingsUG)));
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
									insertTask(createTask(5, nearestbuilding->getPosition(), this->eudm->getMapFromUG(&enemyUG3)));
								}
								insertTask(createTask(2, enemy.second.position, this->eudm->getMapFromUG(&enemyUG3))); // huge
								log(std::string("***** enemyInRange ").append(this->hc->wantBuildManager->intToString(enemyInRange.size())).append(" ").c_str());
								log(std::string("enemyUG3 ").append(this->hc->wantBuildManager->intToString(enemyUG3.size())).append("\n").c_str());
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
					insertTask(createTask(1, enemy.second.position, this->eudm->getMapFromUG(&enemyUG4)));
				}
				insertTask(createTask(2, enemy.second.position, this->eudm->getMapFromUG(&enemyUG4))); // huge
			}
		}
	}

	std::map<BWAPI::Unit*, EnemyUnitData> buildingdata = this->eudm->getData();
	std::set<BWAPI::Position> posset;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in buildingdata)
	{
		if(enemy.second.unitType.isBuilding())
		{
			posset.insert(enemy.second.lastKnownPosition);
		}
	}
	if(posset.size() > 0)
	{
		UnitGroup eigenbuildings = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isBuilding);	
		BWAPI::Position frontline = frontlineBuilding(posset);
		insertTask(Task(5, frontline));
	}

	if(this->eudm->getData().size() < 15)
	{
		std::set<BWTA::BaseLocation*> baselocs = BWTA::getBaseLocations();
		for each(BWTA::BaseLocation* baseloc in baselocs)
		{
			if(!BWAPI::Broodwar->isExplored(baseloc->getPosition()))
			{
				insertTask(Task(1, baseloc->getPosition()));
			}
		}
	}
	else
	{
		std::set<BWTA::BaseLocation*> baselocs = BWTA::getBaseLocations();
		for each(BWTA::BaseLocation* baseloc in baselocs)
		{
			if(!BWAPI::Broodwar->isVisible(baseloc->getPosition()))
			{
				insertTask(Task(1, baseloc->getPosition()));
			}
		}
	}

	/*
	insertTask(Task(1, 1, BWAPI::Position(BWAPI::Broodwar->mapHeight()*1, BWAPI::Broodwar->mapWidth()*0.5).makeValid()));
	insertTask(Task(1, 1, BWAPI::Position(BWAPI::Broodwar->mapHeight()*0.01, BWAPI::Broodwar->mapWidth()*0.5).makeValid()));
	insertTask(Task(1, 1, BWAPI::Position(BWAPI::Broodwar->mapHeight()*0.5, BWAPI::Broodwar->mapWidth()*0.01).makeValid()));
	insertTask(Task(1, 1, BWAPI::Position(BWAPI::Broodwar->mapHeight()*0.5, BWAPI::Broodwar->mapWidth()*1).makeValid()));	
	*/

	// zit nu in mostappropriate
	//insertTask(Task(5, 3, this->hc->hatchery->getPosition()));

}

std::list<Task> TaskManager::findTasksWithType(int t)
{
	return this->hc->planAssigner->findTasksWithType(this->hc->hcplan, t);
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

BWAPI::Position TaskManager::frontlineBuilding(std::set<BWAPI::Position> posset)
{
	UnitGroup eigenbuildings = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isBuilding);
	BWAPI::Position nearest = this->hc->hatchery->getPosition();

	double distance = -1;

	if(posset.size() == 0)
	{
		return this->hc->hatchery->getPosition();
	}

	for(std::set<BWAPI::Unit*>::iterator it=eigenbuildings.begin(); it!=eigenbuildings.end(); it++)
	{
		double currentDistance = (*posset.begin()).getDistance((*it)->getPosition());
		if(distance == -1)
		{
			nearest = (*it)->getPosition();
			distance = currentDistance;
		}
		else if(currentDistance < distance)
		{
			nearest = (*it)->getPosition();
			distance = currentDistance;
		}
	}
	return nearest;
}

bool TaskManager::existsTaskWithType(int type)
{
	for each(Task t in this->tasklist)
	{
		if(t.type == type)
		{
			return true;
		}
	}
	return false;
}

bool TaskManager::existsTaskWithPriority(int priority) // schrap
{
	/*for each(Task t in this->tasklist)
	{
		if(t.priority == priority)
		{
			return true;
		}
	}*/
	return false;
}

Task TaskManager::nearestTask(UnitGroup* ug, std::list<Task> tasks)
{
	Task nearest;
	double distance = -1;
	BWAPI::Position center = ug->getCenter();

	for(std::list<Task>::iterator it=tasks.begin(); it!=tasks.end(); it++)
	{
		double currentDistance = center.getDistance((*it).position);
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

std::string TaskManager::getName(int type)
{
	if(type == 1) return "scout";
	if(type == 2) return "combat";
	if(type == 3) return "prepare";
	if(type == 4) return "detector";
	if(type == 5) return "defend";
	return "";
}

Task TaskManager::createTask(int type, BWAPI::Position pos, std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	bool containsAir = this->eudm->mapContainsAir(data);
	bool containsGround = this->eudm->mapContainsGround(data);
	int size = this->eudm->mapSize(data);
	int milsize = this->eudm->mapMilitarySize(data);
	return Task(type, pos, containsAir, containsGround, size, milsize);
}
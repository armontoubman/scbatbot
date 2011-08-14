#include "EnemyUnitDataManager.h"

#include <BWAPI.h>
#include "HighCommand.h"

#include "UnitGroup.h"
#include "Util.h"
#include <boost/format.hpp>
#include "Task.h"

EnemyUnitDataManager::EnemyUnitDataManager(HighCommand* h)
{
	this->hc = h;
}

void EnemyUnitDataManager::update()
{
	this->clearTasklists();
	this->cleanup();

	for each(std::pair<Unit*, EnemyUnitData> datapair in this->unitmap)
	{
		if(datapair.first->isVisible())
		{
			datapair.second.update(datapair.first);
		}
		if(datapair.second.position != Positions::Unknown && 
			Broodwar->isVisible(TilePosition(datapair.second.lastKnownPosition)) &&
			!datapair.first->isVisible())
		{
			datapair.second.position = Positions::Unknown;
		}

		if(datapair.second.position == Positions::Unknown)
		{
			this->createTask(ScoutTask, datapair.second.lastKnownPosition, datapair.first);
		}
		else
		{
			this->createTask(CombatTask, datapair.second.position, datapair.first);
		}
	}
}

void EnemyUnitDataManager::onStart()
{
}

void EnemyUnitDataManager::updateUnit(Unit* u)
{
	this->unitmap[u].update(u);
}

std::string EnemyUnitDataManager::chat()
{
	std::set<Unit*> collection;
	for each(std::pair<Unit*, EnemyUnitData> datapair in this->unitmap)
	{
		collection.insert(datapair.first);
	}
	std::map<UnitType, int> unitTypeCounts = countUnitTypesInUnitGroup(collection);
	std::string result;
	for each(std::pair<UnitType, int> typepair in unitTypeCounts)
	{
		result.append(intToString(typepair.second)).append(" ").append(typepair.first.getName()).append(",");
	}
	Broodwar->printf(result.c_str());

	std::string s;
	s = boost::str( boost::format( "EUDM: %d units" ) % this->unitmap.size() );
	return s;
}

void EnemyUnitDataManager::onUnitDiscover(Unit* u)
{
	if(u->getPlayer() == Broodwar->enemy())
	{
		this->updateUnit(u);
	}
}

void EnemyUnitDataManager::onUnitEvade(Unit* u)
{
	if(u->getPlayer() == Broodwar->enemy())
	{
		this->updateUnit(u);
	}
}

void EnemyUnitDataManager::onUnitShow(Unit* u)
{
}

void EnemyUnitDataManager::onUnitHide(Unit* u)
{
}

void EnemyUnitDataManager::onUnitCreate(Unit* u)
{
}

void EnemyUnitDataManager::onUnitDestroy(Unit* u)
{
	if(u->getPlayer() == Broodwar->enemy())
	{
		this->unitmap.erase(u);
	}
}

void EnemyUnitDataManager::onUnitMorph(Unit* u)
{
	if(u->getPlayer() == Broodwar->enemy())
	{
		this->updateUnit(u);
	}
}

void EnemyUnitDataManager::onUnitRenegade(Unit* u)
{
	if(u->getPlayer() == Broodwar->enemy())
	{
		this->updateUnit(u);
	}
}

void EnemyUnitDataManager::cleanup()
{
}

void EnemyUnitDataManager::createTask(TaskType tasktype, Position position, Unit* u)
{
	if(tasktype == ScoutTask)
	{
		this->scouttasklist.push_back(Task(tasktype, position, u));
	}
	if(tasktype == CombatTask)
	{
		this->combattasklist.push_back(Task(tasktype, position, u));
	}
}

std::list<Task> EnemyUnitDataManager::getTasklist(TaskType tasktype)
{
	if(tasktype == ScoutTask)
	{
		return this->scouttasklist;
	}
	if(tasktype == CombatTask)
	{
		return this->combattasklist;
	}
	return this->scouttasklist;
}

void EnemyUnitDataManager::clearTasklists()
{
	this->scouttasklist.clear();
	this->combattasklist.clear();
}

BWAPI::Unit* EnemyUnitDataManager::nearestEnemyThatCanAttackAir(BWAPI::Unit* unit)
{
	BWAPI::Unit* enemy = NULL;
	double distance = -1;
	
	for(boost::unordered_map<BWAPI::Unit*, EnemyUnitData>::iterator it=this->unitmap.begin(); it!=this->unitmap.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance(it->second.lastKnownPosition);
		if(it->second.unitType.airWeapon().targetsAir() || it->second.unitType.groundWeapon().targetsAir())
		{
			if(distance == -1)
			{
				enemy = it->first;
				distance = currentDistance;
			}
			else if(currentDistance < distance)
			{
				enemy = it->first;
				distance = currentDistance;
			}
		}
	}

	return enemy;
}

int EnemyUnitDataManager::count(BWAPI::UnitType unittype)
{
	int result = 0;
	for(boost::unordered_map<BWAPI::Unit*, EnemyUnitData>::iterator it=this->unitmap.begin(); it!=this->unitmap.end(); it++)
	{
		if(it->second.unitType == unittype)
		{
			result = result+1;
		}
	}
	return result;
}
#include "EnemyUnitDataManager.h"
#include "EnemyUnitData.h"
#include <BWAPI.h>
#include <InformationManager.h>
#include "Util.h"
#include <sstream>
#include <UnitGroup.h>

EnemyUnitDataManager::EnemyUnitDataManager(){} // niet gebruiken

EnemyUnitDataManager::EnemyUnitDataManager(InformationManager* im)
{
	this->informationManager = im;
}

void EnemyUnitDataManager::update(std::set<BWAPI::Unit*> units)
{
	EnemyUnitData currentUnitData;
	for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
		//BWAPI::Broodwar->drawCircleMap((*i)->getPosition().x(), (*i)->getPosition().y(), (*i)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Red), false);
		
		// currentUnitData is value (geen pointer) dus moet opnieuw opgeslagen dmv insert()
		currentUnitData = enemyUnitsMap[*i];

		if((*i)->isVisible())
		{
			log("EUDM ");
			log((*i)->getType().getName().c_str());
			log(" gezien\n");
			//update info
			currentUnitData.update(*i);
		}
		if(BWAPI::Broodwar->isVisible((*i)->getPosition()) && BWAPI::Broodwar->unitsOnTile((*i)->getTilePosition().x(), (*i)->getTilePosition().y()).count((*i)) == 0)
		{
			log("EUDM ");
			log((*i)->getType().getName().c_str());
			log(" niet op bekende plek\n");
			currentUnitData.position = BWAPI::Positions::Unknown;
		}
		// updates uitvoeren
		enemyUnitsMap.erase(*i);
		enemyUnitsMap.insert(std::make_pair(*i, currentUnitData));
	}
}

void EnemyUnitDataManager::onRemoveUnit(BWAPI::Unit* unit)
{
	std::map<BWAPI::Unit*, EnemyUnitData>::iterator it = enemyUnitsMap.find(unit);
	enemyUnitsMap.erase(it);

	BWAPI::Broodwar->printf("onRemoveUnit");
	log("onRemoveUnit: ");
	log(unit->getType().getName().c_str());
	log("\n");
}

std::string EnemyUnitDataManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

std::map<BWAPI::Unit*,EnemyUnitData> EnemyUnitDataManager::getData()
{
	return this->enemyUnitsMap;
}

int EnemyUnitDataManager::count(BWAPI::UnitType unittype)
{
	int i = 0;
	for(std::map<BWAPI::Unit*,EnemyUnitData>::iterator it=enemyUnitsMap.begin(); it!=enemyUnitsMap.end(); it++)
	{
		if(it->first->getType() == unittype)
		{
			i++;
		}
	}
	return i;
}

UnitGroup EnemyUnitDataManager::getUG()
{
	std::set<BWAPI::Unit*> unitset;
	for(std::map<BWAPI::Unit*,EnemyUnitData>::iterator it=enemyUnitsMap.begin(); it!=enemyUnitsMap.end(); it++)
	{
		unitset.insert(it->first);
	}
	return UnitGroup::getUnitGroup(unitset);
}

EnemyUnitData EnemyUnitDataManager::getEnemyUnitData(BWAPI::Unit* unit)
{
	/*EnemyUnitData result;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in enemyUnitsMap)
	{
		if(enemy.first == unit)
		{
			result = enemy.second;
		}
	}
	return result;*/
	return enemyUnitsMap[unit];
}

std::map<BWAPI::Unit*, EnemyUnitData> EnemyUnitDataManager::getEnemyUnitsInRadius(double radius, BWAPI::Position pos)
{
	std::map<BWAPI::Unit*, EnemyUnitData> result;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in enemyUnitsMap)
	{
		if(enemy.second.position.getDistance(pos) <= radius)
		{
			result.insert(enemy);
		}
	}
	return result;
}

int EnemyUnitDataManager::nrMilitaryUnits(std::set<BWAPI::Unit*> ug)
{
	int count = 0;
	for each(BWAPI::Unit* unit in ug)
	{
		EnemyUnitData data = getEnemyUnitData(unit);
		if(isMilitary(data.unitType))
		{
			count++;
		}
	}
	return count;
}

bool EnemyUnitDataManager::isMilitary(BWAPI::UnitType unittype)
{
	return !unittype.isBuilding() && unittype != BWAPI::UnitTypes::Zerg_Overlord
		&& unittype != BWAPI::UnitTypes::Zerg_Drone && unittype != BWAPI::UnitTypes::Terran_SCV
		&& unittype != BWAPI::UnitTypes::Protoss_Probe && unittype != BWAPI::UnitTypes::Protoss_Observer;
}

std::map<BWAPI::Unit*, EnemyUnitData> EnemyUnitDataManager::getMapFromUG(UnitGroup* ug)
{
	log("EUDM gmFUG\n");
	log("size ug: ");
	log(this->intToString(ug->size()).c_str());
	log("\n");
	std::map<BWAPI::Unit*, EnemyUnitData> result;
	for(std::set<BWAPI::Unit*>::iterator it=ug->begin(); it!=ug->end(); it++)
	{
		//result.insert(std::pair<BWAPI::Unit*, EnemyUnitData>(*it, getEnemyUnitData(*it)));
		result.insert(std::make_pair(*it, getEnemyUnitData(*it)));
	}
	log("size result: ");
	log(this->intToString(result.size()).c_str());
	log("\n");
	return result;
}

bool EnemyUnitDataManager::onlyAirUnits(std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	bool result = true;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in data)
	{
		if(!enemy.second.unitType.isFlyer())
		{
			result = false;
		}
	}
	return result;
}

bool EnemyUnitDataManager::onlyGroundUnits(std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	bool result = true;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in data)
	{
		if(enemy.second.unitType.isFlyer())
		{
			result = false;
		}
	}
	return result;
}

bool EnemyUnitDataManager::canAttackAir(BWAPI::UnitType unittype)
{
	return unittype.airWeapon().targetsAir();
}

bool EnemyUnitDataManager::canAttackGround(BWAPI::UnitType unittype)
{
	return unittype.groundWeapon().targetsGround();
}

bool EnemyUnitDataManager::canAttackAir(std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	bool result = true;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in data)
	{
		if(!canAttackAir(enemy.second.unitType))
		{
			result = false;
		}
	}
	return result;
}

bool EnemyUnitDataManager::canAttackGround(std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	bool result = true;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in data)
	{
		if(!canAttackGround(enemy.second.unitType))
		{
			result = false;
		}
	}
	return result;
}
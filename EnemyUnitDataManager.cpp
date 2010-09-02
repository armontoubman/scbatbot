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
		BWAPI::Broodwar->drawCircleMap((*i)->getPosition().x(), (*i)->getPosition().y(), (*i)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Red), false);
		// currentUnitData is value (geen pointer) dus moet opnieuw opgeslagen dmv insert()
		currentUnitData = enemyUnitsMap[*i];

		//update info
		currentUnitData.update(*i);

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
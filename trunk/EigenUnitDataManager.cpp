#include "EigenUnitDataManager.h"
#include "EigenUnitData.h"
#include <BWAPI.h>
#include "Util.h"
#include <sstream>
#include "HighCommand.h"

EigenUnitDataManager::EigenUnitDataManager(HighCommand* h)
{
	this->hc = h;
}

void EigenUnitDataManager::update(std::set<BWAPI::Unit*> units, std::set<BWAPI::Unit*> enemyUnits)
{
	clearHealthSet();

	EigenUnitData currentUnitData;
	int currentHitPoints;
	for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
		//BWAPI::Broodwar->drawCircleMap((*i)->getPosition().x(), (*i)->getPosition().y(), (*i)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Green), false);
		// currentUnitData is value (geen pointer) dus moet opnieuw opgeslagen dmv insert()
		currentUnitData = myUnitsMap[*i];
		currentHitPoints = (*i)->getHitPoints();

		// TODO: als type niet meer drone is, uit wbm->bouwdrones halen

		// nieuwe unit, hitpoints goed opslaan
		if(currentUnitData.unit == NULL || (*i)->getType() != currentUnitData.type) {

			//unit is nieuw of zn type is veranderd
			// type veranderd -> uit eugm halen voordat we opnieuw assignen
			if((*i)->getType() != currentUnitData.type)
			{
				this->hc->eigenUnitGroupManager->onRemoveUnit(*i);
			}

			currentUnitData.hitPoints = currentHitPoints;
			currentUnitData.unit = *i;
			currentUnitData.type = (*i)->getType();
		}

		// current unit heeft schade opgelopen sinds vorige update
		if(currentUnitData.hitPoints > currentHitPoints) {
			eventHitPointsChanged(*i, currentUnitData.hitPoints - currentHitPoints);

			// update de hitpoints in het geheugen want ze zijn veranderd
			currentUnitData.hitPoints = currentHitPoints;
		}

		// kijken of de unit gezien is door een enemy unit
		// als ie al gezien is hoeft het niet nog een keer te worden gecheckt
		if(currentUnitData.seenByEnemy == false) {
			for(std::set<BWAPI::Unit*>::iterator j=enemyUnits.begin();j!=enemyUnits.end();j++)
			{
				// cirkeltje met range van enemy
				//BWAPI::Broodwar->drawCircleMap((*j)->getPosition().x(), (*j)->getPosition().y(), (*j)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Red), false);
				if((*i)->getDistance(*j) <= (*j)->getType().sightRange()) {
					currentUnitData.seenByEnemy = true;
					currentUnitData.seenByEnemyTime = BWAPI::Broodwar->getFrameCount();
					currentUnitData.seenByEnemyPosition = new BWAPI::Position((*i)->getPosition().x(), (*i)->getPosition().y());
					eventUnitSeen(*i);
					break;
				}
			}
		}

		// updates uitvoeren
		myUnitsMap.erase(*i);
		myUnitsMap.insert(std::make_pair(*i, currentUnitData));
	}
}

void EigenUnitDataManager::onRemoveUnit(BWAPI::Unit* unit)
{
	log("EIUDM onRemoveUnit: ");
	log(unit->getType().getName().c_str());
	log("\n");
	std::map<BWAPI::Unit*, EigenUnitData>::iterator it = myUnitsMap.find(unit);
	myUnitsMap.erase(it);
	log("EIUDM onRemoveUnit ok\n");
}

void EigenUnitDataManager::eventHitPointsChanged(BWAPI::Unit* unit, int difference)
{
	BWAPI::Broodwar->printf("eventHitPointsChanged");
	log("eventHitPointsChanged: ");
	log(unit->getType().getName().c_str());
	log("\n");

	addToHealthSet(unit);
}

void EigenUnitDataManager::eventUnitSeen(BWAPI::Unit* unit)
{
	BWAPI::Broodwar->printf("eventSeen");
	log("eventSeen: ");
	log(unit->getType().getName().c_str());
	log("\n");
}

std::string EigenUnitDataManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

bool EigenUnitDataManager::haveUnitOfType(BWAPI::UnitType type) {
	bool result = false;
	for(std::map<BWAPI::Unit*,EigenUnitData>::iterator i=myUnitsMap.begin();i!=myUnitsMap.end();i++)
	{
		if(i->first->getType()==type) {
			result = true;
			break;
		}
	}
	return result;
}

bool EigenUnitDataManager::unitIsSeen(BWAPI::Unit* unit)
{
	std::map<BWAPI::Unit*, EigenUnitData>::iterator it = myUnitsMap.find(unit);
	return it->second.seenByEnemy;
}

void EigenUnitDataManager::addToHealthSet(BWAPI::Unit* unit)
{
	lostHealthSet.insert(unit);
}

void EigenUnitDataManager::clearHealthSet()
{
	lostHealthSet.clear();
}

bool EigenUnitDataManager::lostHealthThisFrame(BWAPI::Unit* unit)
{
	return lostHealthSet.count(unit) > 0;
}
#include "HighCommand.h"
#include <BWAPI.h>
#include "Util.h"
#include <sstream>

HighCommand::HighCommand()
{
  
}

void HighCommand::update(std::set<BWAPI::Unit*> units, std::set<BWAPI::Unit*> enemyUnits)
{
	UnitData currentUnitData;
	int currentHitPoints;
	for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
		BWAPI::Broodwar->drawCircleMap((*i)->getPosition().x(), (*i)->getPosition().y(), (*i)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Green), false);
		// currentUnitData is value (geen pointer) dus moet opnieuw opgeslagen dmv insert()
		currentUnitData = myUnits[*i];
		currentHitPoints = (*i)->getHitPoints();

		// nieuwe unit, hitpoints goed opslaan
		if(currentUnitData.hitPoints == 0) {
			currentUnitData.hitPoints = currentHitPoints;
		}

		// current unit heeft schade opgelopen sinds vorige update
		if(currentUnitData.hitPoints > currentHitPoints) {
			eventHitPointsChanged(*i, currentUnitData.hitPoints - currentHitPoints);

			// update de hitpoints in het geheugen want ze zijn veranderd
			currentUnitData.hitPoints = currentHitPoints;
		}

		// kijken of de unit gezien is door een enemy unit
		// als ie al gezien is hoeft het niet nog een keer te worden gecheckt
		if(currentUnitData.seen == false) {
			for(std::set<BWAPI::Unit*>::iterator j=enemyUnits.begin();j!=enemyUnits.end();j++)
			{
				// cirkeltje met range van enemy
				//BWAPI::Broodwar->drawCircleMap((*j)->getPosition().x(), (*j)->getPosition().y(), (*j)->getType().sightRange(), BWAPI::Color(BWAPI::Colors::Red), false);
				if((*i)->getDistance(*j) <= (*j)->getType().sightRange()) {
					currentUnitData.seen = true;
					currentUnitData.seenTime = BWAPI::Broodwar->getFrameCount();
					currentUnitData.seenPosition = new BWAPI::Position((*i)->getPosition().x(), (*i)->getPosition().y());
					eventUnitSeen(*i);
					break;
				}
			}
		}

		// updates uitvoeren
		myUnits.erase(*i);
		myUnits.insert(std::make_pair(*i, currentUnitData));
	}
}

void HighCommand::onRemoveUnit(BWAPI::Unit* unit)
{
	std::map<BWAPI::Unit*, UnitData>::iterator it = myUnits.find(unit);
	myUnits.erase(it);

	BWAPI::Broodwar->printf("onRemoveUnit");
	log("onRemoveUnit: ");
	log(unit->getType().getName().c_str());
	log("\n");
}

void HighCommand::eventHitPointsChanged(BWAPI::Unit* unit, int difference)
{
	BWAPI::Broodwar->printf("eventHitPointsChanged");
	log("eventHitPointsChanged: ");
	log(unit->getType().getName().c_str());
	log("\n");
}

void HighCommand::eventUnitSeen(BWAPI::Unit* unit)
{
	BWAPI::Broodwar->printf("eventSeen");
	log("eventSeen: ");
	log(unit->getType().getName().c_str());
	log("\n");
}

std::string HighCommand::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}
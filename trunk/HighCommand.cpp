#include "HighCommand.h"
#include "EigenUnitDataManager.h"
#include "EnemyUnitDataManager.h"
#include "EigenUnitGroupManager.h"
#include <InformationManager.h>
#include <BWAPI.h>
#include "Util.h"
#include <sstream>
#include "UnitGroup.h"

HighCommand::HighCommand(InformationManager* im)
{
	this->eigenUnitDataManager = new EigenUnitDataManager();
	this->enemyUnitDataManager = new EnemyUnitDataManager(im);
	this->eigenUnitGroupManager = new EigenUnitGroupManager(this);
}

HighCommand::~HighCommand() {
	delete this->eigenUnitDataManager;
	delete this->enemyUnitDataManager;
}

void HighCommand::update(std::set<BWAPI::Unit*> myUnits, std::set<BWAPI::Unit*> enemyUnits)
{
	this->eigenUnitDataManager->update(myUnits, enemyUnits);
	this->enemyUnitDataManager->update(enemyUnits);
}

void HighCommand::onRemoveUnit(BWAPI::Unit* unit)
{
	if(unit->getPlayer() == BWAPI::Broodwar->self()) {
		this->eigenUnitDataManager->onRemoveUnit(unit);
		this->eigenUnitGroupManager->onRemoveUnit(unit);
	} else {
		this->enemyUnitDataManager->onRemoveUnit(unit);
	}
}

BWAPI::Unit* HighCommand::getNearestHatchery(BWAPI::Position pos)
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);

	BWAPI::Unit* bestHatchery = NULL;
	double bestDistance = -1.00;

	for(std::set<BWAPI::Unit*>::iterator it = hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		if(bestDistance == -1.00)
		{
			bestHatchery = *it;
			bestDistance = pos.getDistance((*it)->getPosition());
		}
		else if(pos.getDistance((*it)->getPosition()) < bestDistance)
		{
			bestHatchery = *it;
			bestDistance = pos.getDistance((*it)->getPosition());
		}
	}

	return bestHatchery;
}
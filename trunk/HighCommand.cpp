#include "HighCommand.h"
#include "EigenUnitDataManager.h"
#include "EnemyUnitDataManager.h"
#include "EigenUnitGroupManager.h"
#include <InformationManager.h>
#include <BuildOrderManager.h>
#include <BaseManager.h>
#include <BWAPI.h>
#include "Util.h"
#include <sstream>
#include "UnitGroup.h"
#include "TaskManager.h"
#include "WantBuildManager.h"
#include "MicroManager.h"

HighCommand::HighCommand(InformationManager* im, BuildOrderManager* bom, BaseManager* ba)
{
	this->eigenUnitDataManager = new EigenUnitDataManager();
	this->enemyUnitDataManager = new EnemyUnitDataManager(im);
	this->eigenUnitGroupManager = new EigenUnitGroupManager(this, this->eigenUnitDataManager, this->taskManager);
	this->taskManager = new TaskManager(this->eigenUnitGroupManager);
	this->wantBuildManager = new WantBuildManager(this->enemyUnitDataManager, bom, ba);
	this->microManager = new MicroManager(bom, this->enemyUnitDataManager, this->taskManager, this, this->eigenUnitDataManager);
	this->thisAlgorithmBecomingSkynetCost = 999999999;
	this->tick = 1;
}

HighCommand::~HighCommand() {
	delete this->eigenUnitDataManager;
	delete this->enemyUnitDataManager;
	delete this->eigenUnitGroupManager;
	delete this->taskManager;
	delete this->wantBuildManager;
	delete this->microManager;
}

void HighCommand::update(std::set<BWAPI::Unit*> myUnits, std::set<BWAPI::Unit*> enemyUnits)
{
	this->eigenUnitDataManager->update(myUnits, enemyUnits);
	this->enemyUnitDataManager->update(enemyUnits);

	if(this->tick == 5)
	{
	}

	this->microManager->doMicro((&(this->eigenUnitGroupManager->unitGroups)));
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
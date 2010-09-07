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
#include "Task.h"
#include <time.h>
#include <BWTA.h>

HighCommand::HighCommand(InformationManager* im, BuildOrderManager* bom, BaseManager* ba)
{
	this->eigenUnitDataManager = new EigenUnitDataManager();
	this->enemyUnitDataManager = new EnemyUnitDataManager(im);
	this->eigenUnitGroupManager = new EigenUnitGroupManager(this, this->eigenUnitDataManager, this->taskManager);
	this->taskManager = new TaskManager(this->eigenUnitGroupManager, this->enemyUnitDataManager);
	this->wantBuildManager = new WantBuildManager(this->enemyUnitDataManager, bom, ba, this);
	this->microManager = new MicroManager(bom, this->enemyUnitDataManager, this->taskManager, this, this->eigenUnitDataManager, this->wantBuildManager);
	this->thisAlgorithmBecomingSkynetCost = 999999999;
	this->tick = 1;
	this->wantBuildManager->doLists();

	// crash
	log("locaties halen\n");
	std::set<BWTA::BaseLocation*> locs = BWTA::getBaseLocations();
	for each(BWTA::BaseLocation* loc in locs)
	{
		log("loc maken\n");
		BWAPI::Position startpos = loc->getPosition();
		log(this->wantBuildManager->intToString(startpos.x()).append(" ").append(this->wantBuildManager->intToString(startpos.y())).c_str());
		log("ug* maken\n");
		UnitGroup* lords = this->eigenUnitGroupManager->overlordUG;
		log("task maken\n");
		Task t = Task(1, 1, startpos, lords);
		log("task inserten\n");
		this->taskManager->insertTask(t);
	}

	log("\n\n\n\nNEW GAME\n\n\n\n");

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	log("\n");
	log(asctime(timeinfo));
	log("\n");

	this->hatchery = *UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery).begin();

	BWAPI::Broodwar->setLocalSpeed(0);
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

	this->taskManager->update();

	if(this->tick == 5)
	{
	}

	this->microManager->doMicro(this->eigenUnitGroupManager->unitGroups);
	this->wantBuildManager->update();

	if(this->tick == 5)
	{
		this->wantBuildManager->doLists();
	}

	BWAPI::Position pos1 = this->hatchery->getPosition();
	BWAPI::Position pos2 = this->wantBuildManager->nearestUnit(pos1, UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone))->getPosition();
	BWAPI::Broodwar->drawLineMap(pos1.x(), pos1.y(), pos2.x(), pos2.y(), BWAPI::Colors::Orange);

	this->tick++;
	if(this->tick == 10)
	{
		this->tick = 0;
	}

	log("bouwdrones: ");
	for each(BWAPI::Unit* drone in this->wantBuildManager->bouwdrones)
	{
		log(this->wantBuildManager->intToString(drone->getID()).append(" ").c_str());
	}
	log("\n");
}

void HighCommand::onRemoveUnit(BWAPI::Unit* unit)
{
	log("HC onRemoveUnit\n");
	if(unit->getPlayer() == BWAPI::Broodwar->self()) {
		this->eigenUnitDataManager->onRemoveUnit(unit);
		this->eigenUnitGroupManager->onRemoveUnit(unit);
		if(unit->getType() == BWAPI::UnitTypes::Zerg_Drone)
		{
			log("drone verdwenen\n");
			if(this->wantBuildManager->bouwdrones.count(unit) > 0)
			{
				log("zat in bouwgroep\n");
				this->wantBuildManager->bouwdrones.erase(this->wantBuildManager->bouwdrones.find(unit));
			}
		}
	} else {
		this->enemyUnitDataManager->onRemoveUnit(unit);
	}
}

void HighCommand::onUnitShow(BWAPI::Unit* unit)
{
	if(unit->getPlayer() == BWAPI::Broodwar->self())
	{
		this->eigenUnitGroupManager->assignUnit(unit);
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
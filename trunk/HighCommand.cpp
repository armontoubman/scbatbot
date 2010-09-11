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
#include "PlanAssigner.h"
#include "Task.h"
#include <time.h>
#include <BWTA.h>

HighCommand::HighCommand(InformationManager* im, BuildOrderManager* bom, BaseManager* ba)
{
	this->eigenUnitDataManager = new EigenUnitDataManager();
	this->enemyUnitDataManager = new EnemyUnitDataManager(im);
	this->eigenUnitGroupManager = new EigenUnitGroupManager(this, this->eigenUnitDataManager, this->taskManager);
	this->taskManager = new TaskManager(this->eigenUnitGroupManager, this->enemyUnitDataManager, this, this->planAssigner);
	this->wantBuildManager = new WantBuildManager(this->enemyUnitDataManager, bom, ba, this, this->microManager);
	this->microManager = new MicroManager(bom, this->enemyUnitDataManager, this->taskManager, this, this->eigenUnitDataManager, this->wantBuildManager);
	this->planAssigner = new PlanAssigner(this, this->taskManager, this->eigenUnitGroupManager, this->enemyUnitDataManager, this->microManager);
	this->thisAlgorithmBecomingSkynetCost = 999999999;
	this->tick = 1;
	this->wantBuildManager->doLists();
	this->taskManager->update();
	this->planAssigner->update();

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
	delete this->planAssigner;
}

void HighCommand::update(std::set<BWAPI::Unit*> myUnits, std::set<BWAPI::Unit*> enemyUnits)
{
	this->hatchery = *UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive).begin();
	log("HC::update eigenUnitDataManager\n");
	this->eigenUnitDataManager->update(myUnits, enemyUnits);
	log("HC::update enemyUnitDataManager\n");
	this->enemyUnitDataManager->update(enemyUnits);

	if(this->tick == 5)
	{
		log("HC::update taskManager\n");
		this->taskManager->update();
		log("HC::update planAssigner\n");
		log(this->wantBuildManager->intToString(this->planAssigner->plan.size()).append("\n").c_str());
		this->planAssigner->update();
		log(this->wantBuildManager->intToString(this->planAssigner->plan.size()).append("\n").c_str());
	}

	log("HC::update doMicro\n");
	this->microManager->doMicro(this->eigenUnitGroupManager->unitGroups);
	log("HC::update wantBuildManager\n");
	this->wantBuildManager->update();

	if(this->tick == 5)
	{
		log("HC::update doLists\n");
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

	/*log("bouwdrones: ");
	for each(BWAPI::Unit* drone in this->wantBuildManager->bouwdrones)
	{
		log(this->wantBuildManager->intToString(drone->getID()).append(" ").c_str());
	}
	log("\n");*/
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
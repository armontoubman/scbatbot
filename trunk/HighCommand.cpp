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

	BWAPI::Broodwar->setLocalSpeed(0); // WEGHALEN IN FINAL

	this->hatchery = *UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery).begin();

	log("\n\n\n\nNEW GAME\n\n\n\n");

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	log("\n");
	log(asctime(timeinfo));
	log("\n");

	this->eigenUnitDataManager = new EigenUnitDataManager(this);
	this->enemyUnitDataManager = new EnemyUnitDataManager(im);
	this->eigenUnitGroupManager = new EigenUnitGroupManager(this, this->eigenUnitDataManager, this->taskManager, this->planAssigner);
	this->taskManager = new TaskManager(this->eigenUnitGroupManager, this->enemyUnitDataManager, this, this->planAssigner);
	this->wantBuildManager = new WantBuildManager(this->enemyUnitDataManager, bom, ba, this, this->microManager);
	this->microManager = new MicroManager(bom, this->enemyUnitDataManager, this->taskManager, this, this->eigenUnitDataManager, this->wantBuildManager);
	this->planAssigner = new PlanAssigner(this, this->taskManager, this->eigenUnitGroupManager, this->enemyUnitDataManager, this->microManager);
	this->thisAlgorithmBecomingSkynetCost = 999999999;
	this->tick = 1;
	this->wantBuildManager->doLists();
	this->taskManager->update();
	//this->planAssigner->update();
	this->hcplan = this->planAssigner->maakPlan();
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
	log("eudm map: ");
	log(this->wantBuildManager->intToString(this->enemyUnitDataManager->enemyUnitsMap.size()).append("\n").c_str());
	
	log("eiudm map: ");
	log(this->wantBuildManager->intToString(this->eigenUnitDataManager->myUnitsMap.size()).append("\n").c_str());

	if(UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive).size() == 0)
	{
		BWAPI::Broodwar->leaveGame();
	}
	else {
		if(this->hatchery->getHitPoints() < 100) this->hatchery = *UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive).begin();
	}
	
	log("HC::update eigenUnitDataManager\n");
	this->eigenUnitDataManager->update(myUnits, enemyUnits);
	log("HC::update enemyUnitDataManager\n");
	this->enemyUnitDataManager->update(enemyUnits);
	log("HC::update eigenUnitGroupManager\n");
	this->eigenUnitGroupManager->update();

	if(this->tick == 5)
	{
		log("HC::update taskManager\n");
		this->taskManager->update();
		log("tasklist: ");
		log(this->wantBuildManager->intToString(this->taskManager->tasklist.size()).append("\n").c_str());
		log("HC::update planAssigner\n");
		log("hcplan: ");
		log(this->wantBuildManager->intToString(this->hcplan.size()).append("\n").c_str());
		//this->planAssigner->update();
		this->hcplan = this->planAssigner->maakPlan();
		log("hcplan: ");
		log(this->wantBuildManager->intToString(this->hcplan.size()).append("\n").c_str());
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

	log("begin hc cirkeltjes\n");
	///////////////////
	//// print berichten op units
	/////////////////
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	for each(BWAPI::Unit* unit in allUnits)
	{
		std::string unitmsg;

		//order
		//unitmsg = unit->getOrder().getName()

		//UG
		UnitGroup* group = this->eigenUnitGroupManager->findUnitGroupWithUnit(unit);
		unitmsg = this->eigenUnitGroupManager->getName(group);

		BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), unitmsg.c_str());
	}
	////////////////////

	///////////////
	///// print tasks en ug centers
	////////////////
	for each(std::pair<UnitGroup*, Task> paar in this->hcplan)
	{
		BWAPI::Broodwar->drawLineMap(paar.first->getCenter().x(), paar.first->getCenter().y(), paar.second.position.x(), paar.second.position.y(), BWAPI::Colors::Green);
		BWAPI::Broodwar->drawCircleMap(paar.first->getCenter().x(), paar.first->getCenter().y(), 5*32, BWAPI::Colors::Green);
		BWAPI::Broodwar->drawCircleMap(paar.second.position.x(), paar.second.position.y(), 3*32, BWAPI::Colors::Green);
		BWAPI::Broodwar->drawTextMap(paar.second.position.x(), paar.second.position.y(), std::string("\ngo ").append(this->taskManager->getName(paar.second.type)).c_str());
	}
	for each(Task t in this->taskManager->tasklist)
	{
		BWAPI::Broodwar->drawCircleMap(t.position.x(), t.position.y(), 1*32, BWAPI::Colors::Purple);
		BWAPI::Broodwar->drawTextMap(t.position.x(), t.position.y(), this->taskManager->getName(t.type).c_str());
	}
	/////////////
	log("einde hc cirkeltjes\n");

	log("einde hc update\n");
}

void HighCommand::onRemoveUnit(BWAPI::Unit* unit)
{
	log("HC onRemoveUnit\n");
	if(unit->getPlayer() == BWAPI::Broodwar->self()) {
		this->eigenUnitDataManager->onRemoveUnit(unit);
		this->eigenUnitGroupManager->onRemoveUnit(unit);
		/*if(unit->getType() == BWAPI::UnitTypes::Zerg_Drone)
		{
			log("drone verdwenen\n");
			if(this->wantBuildManager->bouwdrones.count(unit) > 0)
			{
				log("zat in bouwgroep\n");
				this->wantBuildManager->bouwdrones.erase(this->wantBuildManager->bouwdrones.find(unit));
			}
		}*/
	} else {
		this->enemyUnitDataManager->onRemoveUnit(unit);
	}
}

void HighCommand::onUnitShow(BWAPI::Unit* unit)
{
	if(unit->getPlayer() == BWAPI::Broodwar->self())
	{
		//this->eigenUnitGroupManager->assignUnit(unit);
	}
}

BWAPI::Unit* HighCommand::getNearestHatchery(BWAPI::Position pos)
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive);

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

void HighCommand::chatBuildList()
{
	BWAPI::Broodwar->printf(this->wantBuildManager->bouwlistString(this->wantBuildManager->buildList).c_str());
}

void HighCommand::chatWantList()
{
	BWAPI::Broodwar->printf(this->wantBuildManager->bouwlistString(this->wantBuildManager->wantList).c_str());
}
#pragma once
#include "HighCommand.h"

#include "EigenUnitDataManager.h"
#include "EnemyUnitDataManager.h"
#include "MicroManager.h"
#include "EigenUnitGroupManager.h"
#include "TaskManager.h"
#include "TaskAssigner.h"
#include "WantBuildManager.h"
#include "ResourceManager.h"

#include <BWAPI.h>
#include "Util.h"
#include "UnitGroup.h"
#include <time.h>
#include <boost/format.hpp>

HighCommand::HighCommand()
{

	//Broodwar->setLocalSpeed(0); // WEGHALEN IN FINAL
	//this->startLog();

	this->eiudm = new EigenUnitDataManager(this);
	this->eudm = new EnemyUnitDataManager(this);
	this->mm = new MicroManager(this);
	this->eiugm = new EigenUnitGroupManager(this);
	this->tm = new TaskManager(this);
	this->tp = new TaskAssigner(this);
	this->wbm = new WantBuildManager(this);
	this->rm = new ResourceManager(this);

}

HighCommand::~HighCommand() {
	delete this->eiudm;
	delete this->eudm;
	delete this->mm;
	delete this->eiugm;
	delete this->tm;
	delete this->tp;
	delete this->wbm;
	delete this->rm;
}

void HighCommand::startLog()
{
	log("\n\n\n\nNEW GAME\n\n");

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	log("\n");
	log(asctime(timeinfo));
	log("\n");
}

void HighCommand::update()
{
	this->eiugm->update();
	this->rm->update();
	this->eudm->update();

	this->tm->update();

	this->drawFPS();
	this->drawTasks();
	this->drawUnits();
}

void HighCommand::tic()
{
	this->c = clock();
}

float HighCommand::toc()
{
	return ((static_cast<float>(clock()-(this->c))/CLOCKS_PER_SEC)*1000);
}

void HighCommand::logtimes()
{
	std::string output = "";
	output.append(floatToString(times[0])).append(",");
	output.append(floatToString(times[1])).append(",");
	output.append(floatToString(times[2])).append(",");
	output.append(floatToString(times[3])).append(",");
	output.append(floatToString(times[4])).append(",");
	output.append(floatToString(times[5])).append(",");
	output.append(floatToString(times[6])).append(",");
	output.append(floatToString(times[7])).append("\n");
	log(output.c_str());
}

void HighCommand::onStart()
{
	this->rm->onStart();
}

std::string HighCommand::chat()
{
	std::string s;
	s = boost::str( boost::format( "HC: %d FPS / %d avg FPS" ) % Broodwar->getFPS() % Broodwar->getAverageFPS() );
	return s;
}

void HighCommand::onUnitDiscover(Unit* u)
{
	this->eudm->onUnitDiscover(u);
}

void HighCommand::onUnitEvade(Unit* u)
{
	this->eudm->onUnitEvade(u);
}

void HighCommand::onUnitShow(Unit* u)
{
}

void HighCommand::onUnitHide(Unit* u)
{
}

void HighCommand::onUnitCreate(Unit* u)
{
	this->eiugm->onUnitCreate(u);
}

void HighCommand::onUnitDestroy(Unit* u)
{
	this->eudm->onUnitDestroy(u);
	this->rm->onUnitDestroy(u);
	this->eiugm->onUnitDestroy(u);
}

void HighCommand::onUnitMorph(Unit* u)
{
	this->eudm->onUnitMorph(u);
	this->eiugm->onUnitMorph(u);
	this->rm->onUnitMorph(u);
}

void HighCommand::onUnitRenegade(Unit* u)
{
	this->eiudm->onUnitRenegade(u);
	this->eiugm->onUnitRenegade(u);
}

void HighCommand::drawFPS()
{
	Broodwar->drawTextScreen(100,0,"%d FPS",Broodwar->getFPS());
}

void HighCommand::drawTasks()
{
	std::map<TaskType, std::list<Task>> tasklists = this->tm->getTasklists();
	for each(std::pair<TaskType, std::list<Task>> typepair in tasklists)
	{
		for each(Task t in typepair.second)
		{
			int x = t.getPosition().x();
			int y = t.getPosition().y();
			Broodwar->drawCircleMap(x,y,32,Colors::White,false);
			Broodwar->drawTextMap(x,y,t.getTypeName().c_str());
		}
	}
}

void HighCommand::drawUnits()
{
	for each(Unit* u in Broodwar->self()->getUnits())
	{
		int x = u->getPosition().x();
		int y = u->getPosition().y();
		Broodwar->drawTextMap(x,y,intToString(u->getID()).c_str());
	}
}
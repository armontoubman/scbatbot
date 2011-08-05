#pragma once
#include "ResourceManager.h"

#include <BWAPI.h>
#include <BWTA.h>
#include "HighCommand.h"
#include "UnitGroup.h"
#include <boost/format.hpp>
#include "Util.h"
#include "Task.h"

using namespace BWAPI;
using namespace BWTA;

ResourceManager::ResourceManager(HighCommand* h)
{
	this->hc = h;
}

void ResourceManager::update()
{
	std::set<Unit*> minerals = Broodwar->getMinerals();
	std::set<Unit*> geysers = Broodwar->getGeysers();
	std::set<Unit*> extractors = UnitGroup::getUnitGroup(Broodwar->self()->getUnits())(Extractor);
	this->clearTasklists();
	int owner;
	for each(Unit* mineral in minerals)
	{
		owner = resourceOwner(mineral);
		this->mineralmap[mineral].owner = owner;
		if(owner == 1) { this->createTask(GatherMineralsTask, mineral); }
	}
	for each(Unit* geyser in geysers)
	{
		this->geysermap[geyser].owner = resourceOwner(geyser);
	}
	for each(Unit* extractor in extractors)
	{
		this->extractormap[extractor].owner = resourceOwner(extractor);
		if(extractor->isCompleted()) { createTask(GatherGasTask, extractor); }
	}
}

void ResourceManager::onStart()
{
}

int ResourceManager::resourceOwner(Unit* resource)
{
	TilePosition p = resource->getTilePosition();
	BaseLocation* b = getNearestBaseLocation(p);
	if(b == NULL) { return 0; }
	TilePosition bp = b->getTilePosition();
	UnitGroup myUnits = UnitGroup::getUnitGroup(Broodwar->self()->getUnits())(GetTilePosition, bp)(isResourceDepot);
	if(myUnits.empty())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

std::string ResourceManager::chat()
{
	std::string s;
	int m = this->mineralmap.size();
	int g = this->geysermap.size();
	int e = this->extractormap.size();
	int mm = this->countOurs(this->mineralmap);
	int gg = this->countOurs(this->geysermap);
	s = boost::str( boost::format( "RM: %d/%d minerals, %d/%d geysers, %d extractors" ) %mm %m  %gg %g  %e  );
	return s;
}

void ResourceManager::onUnitDestroy(Unit* u)
{
	if(u->getType().isResourceContainer())
	{
		this->removeMineral(u);
		this->removeGeyser(u);
		this->removeExtractor(u);
	}
}

void ResourceManager::onUnitMorph(Unit* u)
{
	if(u->getPlayer() == Broodwar->self() && u->getType() == UnitTypes::Zerg_Extractor)
	{
		this->removeGeyser(u);
	}
}

int ResourceManager::countOurs(ResourceMap resources)
{
	int count = 0;
	for each(std::pair<Unit*, ResourceData> resourcepair in resources)
	{
		if(resourcepair.second.owner == 1)
		{
			count++;
		}
	}
	return count;
}

void ResourceManager::createTask(TaskType tasktype, Unit* resource)
{
	if(tasktype == GatherMineralsTask)
	{
		this->gathermineralstasklist.push_back(Task(tasktype, resource->getPosition(), resource));
	}
	if(tasktype == GatherGasTask)
	{
		this->gathergastasklist.push_back(Task(tasktype, resource->getPosition(), resource));
	}
}

std::list<Task> ResourceManager::getTasklist(TaskType tasktype)
{
	if(tasktype == GatherMineralsTask)
	{
		return this->gathermineralstasklist;
	}
	if(tasktype == GatherGasTask)
	{
		return this->gathergastasklist;
	}
	return this->gathermineralstasklist;
}

void ResourceManager::removeMineral(Unit* u)
{
	if(this->mineralmap.count(u) > 0)
	{
		this->mineralmap.erase_return_void(this->mineralmap.find(u));
	}
}

void ResourceManager::removeGeyser(Unit* u)
{
	if(this->geysermap.count(u) > 0)
	{
		this->geysermap.erase_return_void(this->geysermap.find(u));
	}
}

void ResourceManager::removeExtractor(Unit* u)
{
	if(this->extractormap.count(u) > 0)
	{
		this->extractormap.erase_return_void(this->extractormap.find(u));
	}
}

void ResourceManager::clearTasklists()
{
	this->gathergastasklist.clear();
	this->gathermineralstasklist.clear();
}
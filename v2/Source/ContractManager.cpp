#include "ContractManager.h"

#include <BWAPI.h>
#include "HighCommand.h"
#include "BuildingPlacer.h"
#include "Util.h"

ContractManager::ContractManager(HighCommand* h)
{
	this->hc = h;
}

void ContractManager::update()
{
	for each(Contract c in this->contractList)
	{
		if(c.drone == NULL)
		{
			c.drone = this->findAvailableDrone();
		}
	}
}

bool ContractManager::isContracted(BWAPI::Unit* drone)
{
	for each(Contract c in this->contractList)
	{
		if(c.drone == drone)
		{
			return true;
		}
	}
	return false;
}

int ContractManager::count(BWAPI::UnitType unittype)
{
	int result = 0;
	for each(Contract c in this->contractList)
	{
		if(c.unittype == unittype)
		{
			result = result+1;
		}
	}
	return result;
}

int ContractManager::countExpand()
{
	return this->count(BWAPI::UnitTypes::None);
}

void ContractManager::newContract(BWAPI::UnitType unittype)
{
	Contract c = Contract();
	c.unittype = unittype;
	c.drone = this->findAvailableDrone();
	this->contractList.push_back(c);
}

void ContractManager::newExpandContract()
{
	Contract c = Contract();
	c.unittype = BWAPI::UnitTypes::None;
	c.drone = this->findAvailableDrone();
	this->contractList.push_back(c);
}

BWAPI::Unit* ContractManager::findAvailableDrone()
{
	UnitGroup drones = (*this->hc->eiugm->getDroneUG());
	BWAPI::Unit* drone = NULL;
	for each(BWAPI::Unit* d in drones)
	{
		if(!d->isCarryingGas() && !d->isCarryingMinerals() && !this->isContracted(d))
		{
			return d;
		}
	}
	return drone;
}

BWAPI::TilePosition ContractManager::findPositionFor(BWAPI::UnitType unittype)
{
	if(unittype == BWAPI::UnitTypes::None)
	{
		return this->findPositionForExpand();
	}
	if(unittype == BWAPI::UnitTypes::Zerg_Extractor)
	{
		UnitGroup bases = allEigenUnits()(Hatchery, Lair, Hive);
		UnitGroup geysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getGeysers());
		for each(BWAPI::Unit* base in bases)
		{
			UnitGroup near = geysers.inRadius(dist(10), base->getPosition());
			if(near.size() > 0)
			{
				return (*near.begin())->getTilePosition();
			}
		}
	}
	return BuildingPlacer().getBuildLocationNear(this->hc->home, unittype);
}

BWAPI::TilePosition ContractManager::findPositionForExpand()
{
	std::set<BWTA::BaseLocation*> baselocations = BWTA::getBaseLocations();
	std::set<BWAPI::TilePosition> tilepositions;
	std::set<BWTA::BaseLocation*>::iterator it = baselocations.begin();
	UnitGroup bases = allEigenUnits()(Hatchery, Lair, Hive);
	for each(BWTA::BaseLocation* bl in baselocations)
	{
		if(bases(GetTilePosition, bl->getTilePosition()).size() == 0)
		{
			tilepositions.insert(bl->getTilePosition());
		}
	}
	if(tilepositions.size() > 0)
	{
		return getNearestTilePosition(this->hc->home, tilepositions);
	}
	else
	{
		return BWAPI::TilePositions::None;
	}
}

void ContractManager::onUnitDestroy(BWAPI::Unit* u)
{
	if(u->getType() == BWAPI::UnitTypes::Zerg_Drone && u->getPlayer() == BWAPI::Broodwar->self())
	{
		for each(Contract c in this->contractList)
		{
			if(c.drone == u)
			{
				c.drone = NULL;
				break;
			}
		}
	}
}

void ContractManager::onUnitMorph(BWAPI::Unit* u)
{
	if(u->getType().isBuilding() && u->getPlayer() == BWAPI::Broodwar->self())
	{

		std::set<BWTA::BaseLocation*> baselocations = BWTA::getBaseLocations();
		std::set<BWAPI::TilePosition> tilepositions;
		for each(BWTA::BaseLocation* bl in baselocations)
		{
			tilepositions.insert(bl->getTilePosition());
		}
		if(tilepositions.count(u->getTilePosition()) > 0)
		{
			this->removeContract(BWAPI::UnitTypes::None);
		}
		else
		{
			this->removeContract(u->getType());
		}
	}
}

void ContractManager::removeContract(BWAPI::UnitType unittype)
{
	Contract toRemove;
	for each(Contract c in this->contractList)
	{
		if(c.unittype == unittype)
		{
			toRemove = c;
			break;
		}
	}
	this->contractList.remove(toRemove);
}

Contract ContractManager::getContract(BWAPI::Unit* drone)
{
	for each(Contract c in this->contractList)
	{
		if(c.drone == drone)
		{
			return c;
		}
	}
	return Contract();
}

std::list<Contract> ContractManager::getContractList()
{
	return this->contractList;
}

int ContractManager::getTotalGas()
{
	int result = 0;
	for each(Contract c in this->contractList)
	{
		if(c.unittype == BWAPI::UnitTypes::None)
		{
			result = result + BWAPI::UnitTypes::Zerg_Hatchery.gasPrice();
		}
		else
		{
			result = result + c.unittype.gasPrice();
		}
	}
	return result;
}

int ContractManager::getTotalMinerals()
{
	int result = 0;
	for each(Contract c in this->contractList)
	{
		if(c.unittype == BWAPI::UnitTypes::None)
		{
			result = result + BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
		}
		else
		{
			result = result + c.unittype.mineralPrice();
		}
	}
	return result;
}
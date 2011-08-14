#include "ConstructionManager.h"

#include <BWAPI.h>
#include "HighCommand.h"
#include "Util.h"
#include "Product.h"
#include "BuildingPlacer.h"

ConstructionManager::ConstructionManager(HighCommand* h)
{
	this->hc = h;
	this->gas = 0;
	this->minerals = 0;
	this->supplyTotal = 0;
	this->supplyUsed = 0;
}

void ConstructionManager::update()
{
	this->gas = Broodwar->self()->gas();
	this->minerals = Broodwar->self()->minerals();
	this->supplyTotal = Broodwar->self()->supplyTotal();
	this->supplyUsed = Broodwar->self()->supplyUsed();

	this->gas = this->gas - this->hc->ctm->getTotalGas();
	this->minerals = this->minerals - this->hc->ctm->getTotalMinerals();

	Product top;
	int products = this->hc->pm->getBuildList()->size();
	for(int i=0; i<products; i++)
	{
		top = this->hc->pm->getBuildList()->top();
		if(process(top))
		{
			buy(top);
			this->hc->pm->getBuildList()->removeTop();
		}
		else
		{
			break;
		}
	}
}

bool ConstructionManager::requirementsSatisfied(Product p)
{
	if(p.type == BuildProduct)
	{
		return requirementsSatisfied(p.buildtype);
	}
	if(p.type == TechProduct)
	{
		return requirementsSatisfied(p.techtype);
	}
	if(p.type == UpgradeProduct)
	{
		return requirementsSatisfied(p.upgradetype);
	}
	if(p.type == ExpandProduct)
	{
		return requirementsSatisfied(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	return false;
}

bool ConstructionManager::requirementsSatisfied(BWAPI::UnitType unittype)
{
	std::map<BWAPI::UnitType, int> reqs = unittype.requiredUnits();
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	for each(std::pair<BWAPI::UnitType, int> req in reqs)
	{
		if(allUnits(GetType, req.first)(isCompleted).size() == 0)
		{
			reqsMet = false;
		}
	}
	if(unittype == BWAPI::UnitTypes::Zerg_Lurker || unittype == BWAPI::UnitTypes::Zerg_Lurker_Egg)
	{
		if(!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))
		{
			reqsMet = false;
		}
	}
	return reqsMet;
}

bool ConstructionManager::requirementsSatisfied(BWAPI::TechType techtype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	reqsMet = allUnits(GetType, techtype.whatResearches())(isCompleted).size() > 0;
	return reqsMet;
}

bool ConstructionManager::requirementsSatisfied(BWAPI::UpgradeType upgradetype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	reqsMet = allUnits(GetType, upgradetype.whatUpgrades())(isCompleted).size() > 0;
	return reqsMet;
}

bool ConstructionManager::process(Product p)
{
	if(p.type == BuildProduct)
	{
		return processBuild(p);
	}
	if(p.type == TechProduct)
	{
		return processTech(p);
	}
	if(p.type == UpgradeProduct)
	{
		return processUpgrade(p);
	}
	if(p.type == ExpandProduct)
	{
		return processExpand(p);
	}
	return false;
}

bool ConstructionManager::processBuild(Product p)
{
	if(p.buildtype.isBuilding())
	{
		return processBuilding(p);
	}
	else
	{
		return processUnit(p);
	}
}

bool ConstructionManager::processTech(Product p)
{
	if(!this->canAfford(p) || !this->requirementsSatisfied(p))
	{
		return false;
	}
	BWAPI::UnitType typeofresearcher = p.techtype.whatResearches();
	UnitGroup researchers = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, typeofresearcher)(isCompleted);
	if(researchers.size() == 0)
	{
		return false;
	}
	bool beingdone = false;
	for each(BWAPI::Unit* unit in researchers)
	{
		if(unit->isResearching() && unit->getTech() == p.techtype)
		{
			beingdone = true;
		}
	}
	if(!beingdone)
	{
		for each(BWAPI::Unit* unit in researchers)
		{
			if(!unit->isResearching())
			{
				unit->research(p.techtype);
				return true;
			}
		}
	}
	return false;
}

bool ConstructionManager::processUpgrade(Product p)
{
	if(!this->canAfford(p) || !this->requirementsSatisfied(p))
	{
		return false;
	}
	BWAPI::UnitType typeofresearcher = p.upgradetype.whatUpgrades();
	UnitGroup researchers = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, typeofresearcher)(isCompleted);
	if(researchers.size() == 0)
	{
		return false;
	}
	bool beingdone = false;
	for each(BWAPI::Unit* unit in researchers)
	{
		if(unit->isUpgrading() && unit->getUpgrade() == p.upgradetype)
		{
			beingdone = true;
		}
	}
	if(!beingdone)
	{
		for each(BWAPI::Unit* unit in researchers)
		{
			if(!unit->isUpgrading())
			{
				unit->upgrade(p.upgradetype);
				return true;
			}
		}
	}
	return false;
}

bool ConstructionManager::processExpand(Product p)
{
	if(!this->canAfford(p) || !this->requirementsSatisfied(p))
	{
		return false;
	}
	this->hc->ctm->newExpandContract();
	return true;
}

bool ConstructionManager::processBuilding(Product p)
{
	if(!this->canAfford(p) || !this->requirementsSatisfied(p))
	{
		return false;
	}
	if(p.buildtype == BWAPI::UnitTypes::Zerg_Lair || p.buildtype == BWAPI::UnitTypes::Zerg_Hive)
	{
		if(p.buildtype == BWAPI::UnitTypes::Zerg_Lair)
		{
			UnitGroup hatcheries = allEigenUnits()(Hatchery)(isCompleted);
			if(hatcheries.size() > 0)
			{
				BWAPI::Unit* hatchery = *hatcheries.begin();
				hatchery->morph(BWAPI::UnitTypes::Zerg_Lair);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			UnitGroup lairs = allEigenUnits()(Lair)(isCompleted);
			if(lairs.size() > 0)
			{
				BWAPI::Unit* lair = *lairs.begin();
				lair->morph(BWAPI::UnitTypes::Zerg_Hive);
				return true;
			}
			return false;
		}
	}
	else
	{
		this->hc->ctm->newContract(p.buildtype);
		return true;
	}
}

bool ConstructionManager::processUnit(Product p)
{
	if(!this->canAfford(p) || !this->requirementsSatisfied(p))
	{
		return false;
	}

	if(p.buildtype == BWAPI::UnitTypes::Zerg_Lurker)
	{
		UnitGroup hydras = allEigenUnits()(Hydralisk);
		if(hydras.size() > 0)
		{
			if(hydras(isIdle).size() > 0)
			{
				BWAPI::Unit* hydra = *hydras(isIdle).begin();
				hydra->morph(BWAPI::UnitTypes::Zerg_Lurker);
				return true;
			}
			else
			{
				BWAPI::Unit* hydra = *hydras.begin();
				hydra->morph(BWAPI::UnitTypes::Zerg_Lurker);
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		UnitGroup larva = allEigenUnits()(Larva);
		if(larva.size() > 0)
		{
			BWAPI::Unit* larv = *larva.begin();
			larv->morph(p.buildtype);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool ConstructionManager::canAfford(Product p)
{
	if(p.type == ExpandProduct)
	{
		return BWAPI::UnitTypes::Zerg_Hatchery.gasPrice() <= this->gas && BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice() <= this->minerals;
	}
	return p.gasPrice() <= this->gas && p.mineralPrice() <= this->minerals;
}

void ConstructionManager::buy(Product p)
{
	this->gas = this->gas - p.gasPrice();
	this->minerals = this->minerals - p.mineralPrice();
}
#include "WantBuildManager.h"
#include <BWAPI.h>
#include <UnitGroup.h>
#include "EnemyUnitDataManager.h"
#include "BuildItem.h"
#include "BuildList.h"
#include "MicroManager.h"
#include "BuildOrderManager.h"
#include "BaseManager.h"

WantBuildManager::WantBuildManager(EnemyUnitDataManager* e, BuildOrderManager* b, BaseManager* ba)
{
	this->eudm = e;
	this->bom = b;
	this->bm = ba;
}

int WantBuildManager::nrOfEnemy(BWAPI::UnitType unittype)
{
	return this->eudm->count(unittype);
}

int WantBuildManager::nrOfOwn(BWAPI::UnitType unittype)
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, unittype).size();
}

bool WantBuildManager::wantListIsEmpty()
{
	return this->wantList.isEmpty();
}

bool WantBuildManager::buildListIsEmpty()
{
	return this->buildList.isEmpty();
}

void WantBuildManager::addWant(BWAPI::UnitType unittype)
{
	addWant(unittype, 1);
}

void WantBuildManager::addWant(BWAPI::UnitType unittype, int amount)
{
	for(int i=0; i<amount; i++)
	{
		this->wantList.addItem(BuildItem(unittype, 1));
	}
}

void WantBuildManager::addWant(BWAPI::TechType techtype)
{
	this->wantList.addItem(BuildItem(techtype, 1));
}

void WantBuildManager::addWant(BWAPI::UpgradeType upgradetype)
{
	this->wantList.addItem(BuildItem(upgradetype, 1));
}

void WantBuildManager::addBuild(BWAPI::UnitType unittype)
{
	addBuild(unittype, 1);
}

void WantBuildManager::addBuild(BWAPI::UnitType unittype, int amount)
{
	for(int i=0; i<amount; i++)
	{
		this->buildList.addItem(BuildItem(unittype, 1));
	}
}

void WantBuildManager::addBuildTop(BWAPI::UnitType unittype)
{
	this->buildList.addItemTop(BuildItem(unittype, 1));
}

void WantBuildManager::addBuild(BWAPI::TechType techtype)
{
	this->buildList.addItem(BuildItem(techtype, 1));
}

void WantBuildManager::addBuild(BWAPI::UpgradeType upgradetype)
{
	this->buildList.addItem(BuildItem(upgradetype, 1));
}

void WantBuildManager::wantExpand()
{
	this->wantList.addItem(BuildItem().expand());
}

void WantBuildManager::buildExpand()
{
	this->buildList.addItem(BuildItem().expand());
}

bool WantBuildManager::wantListContains(BWAPI::UnitType unittype)
{
	return this->wantList.count(unittype) > 0;
}

bool WantBuildManager::buildListContains(BWAPI::UnitType unittype)
{
	return this->buildList.count(unittype) > 0;
}

int WantBuildManager::nrOfEnemyMilitaryUnits()
{
	return this->eudm->getUG().not(isBuilding).not(Overlord).not(Drone).not(SCV).not(Probe).not(Observer).size();
}

int WantBuildManager::nrOfOwnMilitaryUnits()
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).not(isBuilding).not(Overlord).not(Drone).not(SCV).not(Probe).not(Observer).size();
}

bool WantBuildManager::photonCannonNearBase()
{
	bool result = false;
	std::map<BWAPI::Unit*,EnemyUnitData> data = this->eudm->getData();
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	for(std::map<BWAPI::Unit*,EnemyUnitData>::iterator it=data.begin(); it!=data.end(); it++)
	{
		if(it->first->getType() == BWAPI::UnitTypes::Protoss_Photon_Cannon)
		{
			for(std::set<BWAPI::Unit*>::iterator hit=hatcheries.begin(); hit!=hatcheries.end(); hit++)
			{
				if(it->first->getDistance(*hit) < 15.00)
				{
					return true;
				}
			}
		}
	}
	return result;
}

int WantBuildManager::countEggsMorphingInto(BWAPI::UnitType unittype)
{
	int totaal = 0;
	UnitGroup eggs = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Egg);
	for(std::set<BWAPI::Unit*>::iterator it=eggs.begin(); it!=eggs.end(); it++)
	{
		if((*it)->getBuildType() == unittype)
		{
			totaal++;
		}
	}
	return totaal;
}

bool WantBuildManager::wantListIsCompleted()
{
	/*
	BWAPI::UnitType buildtype; // 1
	BWAPI::TechType researchtype; // 2
	BWAPI::UpgradeType upgradetype; // 3
	*/
	std::set<BWAPI::UnitType> algehad;
	for(std::list<BuildItem>::iterator it=this->wantList.buildlist.begin(); it!=this->wantList.buildlist.end(); it++)
	{
		if(it->typenr == 1)
		{
			if(!algehad.count(it->buildtype) > 0)
			{
				int verwachteaantal = wantList.count(it->buildtype);
				algehad.insert(it->buildtype);
				if(UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, it->buildtype).size() < verwachteaantal)
				{
					return false;
				}
			}
		}
		else if(it->typenr == 2)
		{
			if(!BWAPI::Broodwar->self()->hasResearched(it->researchtype))
			{
				return false;
			}
		}
		else if(it->typenr == 3)
		{
			if(BWAPI::Broodwar->self()->getUpgradeLevel(it->upgradetype) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

UnitGroup WantBuildManager::getHatcheriesWithMinerals()
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)  // stond eerst it!=minerals.end(), lijkt me beetje raar als de rest mit staat? ***
		{
			if((*it)->getDistance(*mit) <= 8.00)
			{
				result.insert(*it);
				break;
			}
		}
	}
	return result;
}

UnitGroup WantBuildManager::getUnusedMineralsNearHatcheries()
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)
		{
			if((*it)->getDistance(*mit) <= 8.00 && !(*mit).isBeingGathered())
			{
				result.insert(*mit);
			}
		}
	}
	return result;
}

BWTA::BaseLocation* WantBuildManager::getNaturalExpansion()
{
	double minDist;
	BWTA::BaseLocation* natural=NULL;
	BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
	for(std::set<BWTA::BaseLocation*>::const_iterator b=BWTA::getBaseLocations().begin();b!=BWTA::getBaseLocations().end();b++)
	{
		if (*b==home) continue;
		double dist=home->getGroundDistance(*b);
		if (dist>0)
		{
			if (natural==NULL || dist<minDist)
			{
				minDist=dist;
				natural=*b;
			}
		}
	}
	return natural;
}

int WantBuildManager::nrOfEnemyBases()
{
	if(BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss)
	{
		return nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus);
	}
	if(BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
	{
		return nrOfEnemy(BWAPI::UnitTypes::Terran_Command_Center);
	}
	if(BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
	{
		return nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	return 0;
}

void WantBuildManager::update()
{
	// Actual building of items
	BuildItem b = buildList.top();
	if(b.typenr != 4 && !(BWAPI::Broodwar->self()->gas() < b.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
	{
		if(b.typenr == 1)
		{
			if(!BWAPI::Broodwar->canMake(NULL, b.buildtype))
			{
				buildList.removeTop();
				return;
			}
		}
		if(b.typenr == 2)
		{
			if(!BWAPI::Broodwar->canResearch(NULL, b.researchtype))
			{
				buildList.removeTop();
				return;
			}
		}
		if(b.typenr == 3)
		{
			if(!BWAPI::Broodwar->canUpgrade(NULL, b.upgradetype))
			{
				buildList.removeTop();
				return;
			}
		}
	}
	else if(b.typenr != 4)
	{
		if(b.typenr == 1)
		{
			if(BWAPI::Broodwar->canMake(NULL, b.buildtype))
			{
				this->bom->build(1, b.buildtype, 1);
				buildList.removeTop();
				return;
			}
		}
		if(b.typenr == 2)
		{
			if(BWAPI::Broodwar->canResearch(NULL, b.researchtype))
			{
				this->bom->research(b.researchtype, 1);
				buildList.removeTop();
				return;
			}
		}
		if(b.typenr == 3)
		{
			if(BWAPI::Broodwar->canUpgrade(NULL, b.upgradetype))
			{
				this->bom->upgrade(BWAPI::Broodwar->self()->getUpgradeLevel(b.upgradetype)+1, b.upgradetype, 1);
				buildList.removeTop();
				return;
			}
		}
	}
	else if(b.typenr == 4)
	{
		if(BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice())
		{
			this->bm->expand();
			buildList.removeTop();
			return;
		}
	}
}

void WantBuildManager::doLists()
{
	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();

	int stap = 1;

	if(enemyRace == BWAPI::Races::Protoss)
	{
		
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 4);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Overlord);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 2);
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
			stap = 2;
		}
		if( stap == 2)
		{
			if(	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus) == 2)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Forge) == 0)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) < 4))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			else 
			{
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 2)	&& photonCannonNearBase()	)
				{
					wantExpand();
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus) == 2)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Forge) >= 1)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) < 9)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 10)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) == 2)	)
				{
					wantExpand();
				}
				if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Stargate) >= 1)
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk, 10);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7) || ((nrOfEnemy(BWAPI::UnitTypes::Protoss_Gateway) > 2)  && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0)))
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 1) && photonCannonNearBase()	)
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk, 10);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0))
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon,2)))
				{
					addWant(BWAPI::UnitTypes::Zerg_Zergling, 20);
					if(	nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) < 2)
					{
						addWant(BWAPI::UnitTypes::Zerg_Hatchery);
					}
					else
					{
						addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
						addWant(BWAPI::UnitTypes::Zerg_Extractor);
					}
				}
				if(  nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > 2 && !wantListContains(BWAPI::UnitTypes::Zerg_Extractor))
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					wantExpand();
					stap = 3;
				}
			}
		}
		if( stap == 3)
		{
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addWant(BWAPI::TechTypes::Lurker_Aspect);
				addWant(BWAPI::UpgradeTypes::Pneumatized_Carapace);
				stap = 4;
			}
			if( wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && !wantListContains(BWAPI::UnitTypes::Zerg_Spire))
			{
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if( wantListContains(BWAPI::UnitTypes::Zerg_Spire) && !wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den))
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0) || ((nrOfEnemy(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7)) || (nrOfEnemy(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) > 0))
			{
				addWant(BWAPI::UnitTypes::Zerg_Spire);
				addWant(BWAPI::UnitTypes::Zerg_Mutalisk, 11);
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 7 )
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk, 20);
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 9)
			{
				addWant(BWAPI::TechTypes::Lurker_Aspect);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > 2) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 10) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) > 10) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 40)) && wantListContains(BWAPI::UnitTypes::Zerg_Spire) && wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)  > 5*nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery))
			{
				stap = 4;
			}
		}
		if( stap == 4)
		{
			addWant(BWAPI::UnitTypes::Zerg_Hive);
			addWant(BWAPI::UnitTypes::Zerg_Defiler_Mound);
			addWant(BWAPI::UnitTypes::Zerg_Ultralisk);
		}
		// reinforcements -->
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 9)
		{
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0) || this->eudm->getUG()(isFlyer).size() >4)
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 9)
				{
					if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0 )
					{
						if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)))
						{
							addBuild(BWAPI::UnitTypes::Zerg_Zergling);
						}
						if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
						{
							addBuild(BWAPI::UnitTypes::Zerg_Defiler);
						}
						if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
						{
							addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
						}
					}
					else
					{
						if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) )
						{
							addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
						}
						else
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
					}
				}
				else if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
			} 
			else
			{
				if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
				{
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling < 26) || ( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) ))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
					if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Defiler);
					}
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
					}
				}
				else if (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
			}
			
		}
		else if( nrOfEnemyMilitaryUnits() < 6)
		{
			if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling < 26) || ( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Defiler);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
				}
			}
			else
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
			}
		}
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 4)
		{
			if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling < 26) || ( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Defiler);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
				}
			}
			else
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
			}
		}
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0)
		{
			if(nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
		}
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker) > 0))
		{
			if(nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 5)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Lurker);
			}
			else
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}

		}
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker) < 8))
		{
			addBuild(BWAPI::UnitTypes::Zerg_Lurker);
		}
		if( (((BWAPI::Broodwar->self()->minerals()>300 || BWAPI::Broodwar->self()->gas()>300) && nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus)) || nrOfOwnMilitaryUnits() <5 || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits())) && !buildList.containsUnits() )
		{
			if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling < 26) || ( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Defiler);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
				}
			}
			if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 10) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			else 
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
			}
		}
	}
	else if(enemyRace == BWAPI::Races::Terran)
	{
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 4);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Overlord);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 2); //addWant(BWAPI::UnitTypes::drones, 11)?
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3); //totaal 6???
			stap = 2;
		}
		if( stap == 2)
		{
			wantExpand();
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Barracks) > 1)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Terran_Factory) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Terran_Vulture) > 0) )
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Starport) > 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			if( wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && !wantListContains(BWAPI::UnitTypes::Zerg_Spire) && nrOfEnemy(BWAPI::UnitTypes::Terran_Barracks) < 2)
			{
				if(nrOfEnemy(BWAPI::UnitTypes::Terran_Command_Center) > 1)
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
				else if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) < 3 && !buildList.containsExpand())
				{
					wantExpand();
				}
			}
			if( wantListIsCompleted())
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				stap = 3;
			}
		}
		if( stap == 3)
		{
			addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Marine) > 8)
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addWant(BWAPI::TechTypes::Lurker_Aspect);
			}
			if((nrOfEnemy(BWAPI::UnitTypes::Terran_Goliath) > 4) && nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) == 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				if(nrOfEnemy(BWAPI::UnitTypes::Terran_Science_Vessel) > 0)
				{
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
				else
				{
					addWant(BWAPI::TechTypes::Lurker_Aspect);
				}
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) > 4)
			{
				if(nrOfEnemy(BWAPI::UnitTypes::Terran_Goliath) < 6)
				{
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
				else
				{
					addWant(BWAPI::UnitTypes::Zerg_Zergling,30);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				}
			}
			if((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > 2) && wantListIsCompleted() ) 
			{
				stap = 4;
			}
		}
		if( stap == 4)
		{
			addWant(BWAPI::UnitTypes::Zerg_Hive);
			addWant(BWAPI::UnitTypes::Zerg_Defiler);
			addWant(BWAPI::UnitTypes::Zerg_Ultralisk);
		}
		//reinforcements
		if( nrOfEnemy(BWAPI::UnitTypes::Terran_Marine) > 9)
		{
			if( nrOfEnemy(BWAPI::UnitTypes::Terran_Science_Vessel) > 0)
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
				{
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
					if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Defiler);
					}
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
					}

				}
				else
				{
					if(nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
					{
						if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 7)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
						else
						{
							if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))
							{
								addBuild(BWAPI::UnitTypes::Zerg_Lurker);
							}
							else
							{
								addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
							}
						}
					}
					else if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
					}
				}
			}
			else
			{
				if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))
				{
					if( this->eudm->getUG()(isFlyer).size() > 3)
					{
						if(nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) == 0)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
						else
						{
							addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
						}
					}
					else
					{
						addBuild(BWAPI::UnitTypes::Zerg_Lurker);
					}
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
			}
		}
		else
		{
			if( (nrOfEnemy(BWAPI::UnitTypes::Terran_Goliath) < 4) && (nrOfEnemy(BWAPI::UnitTypes::Terran_Missile_Turret) < 6) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
				{
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
					if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Defiler);
					}
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
					}
				}
				else
				{
					if( (nrOfEnemy(BWAPI::UnitTypes::Terran_Science_Vessel) < 3) && BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 2) && (nrOfEnemy(BWAPI::UnitTypes::Terran_Vulture) < 5) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Lurker);
					}
					else
					{
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
				}
			}
		}
		if( (((BWAPI::Broodwar->self()->minerals()>300 || BWAPI::Broodwar->self()->gas()>300) && nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus)) || nrOfOwnMilitaryUnits() <5 || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits())) && !buildList.containsUnits() )
		
		{
			if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling < 26) || ( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Defiler);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
				}
			}
			if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 10) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			else 
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
			}
		}
	}
	else if(enemyRace == BWAPI::Races::Zerg)
	{
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 4);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Overlord);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Drone, 2); //addWant(BWAPI::UnitTypes::drones, 11)?
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
			stap = 2;
		}
		if( stap == 2)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 8) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Zergling) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) < 2))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Sunken_Colony);
			}
			if( ((nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery) == 0 || nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery) == 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) == 0)) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Mutalisk) > 3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) == 0) )
			{
				addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
				addWant(BWAPI::UnitTypes::Zerg_Spore_Colony);
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0 && !buildList.containsExpand())
			{
				wantExpand();
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Zerg_Spire) > 0)
			{
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery) == 1) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 7) )
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if ( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Spire) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) < 3) )
			{
				if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 7) && !buildList.containsExpand())
				{
					wantExpand();
				}
				else
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
			}
			if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) > 2)
			{
				stap = 3;
			}

		}
		if( stap == 3)
		{
			addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			if(!buildList.containsExpand())
			{
				wantExpand();
			}
		}			
		// reinforcements
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)<5) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 6) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) <1)) && BWAPI::Broodwar->self()->gas() < 70)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			else
			{
				if(nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) >= 11 && nrOfEnemy(BWAPI::UnitTypes::Zerg_Hydralisk) > &)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
				}
				else
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
			}
		}
		else 
		{
			bool spireExists = false;
			BWAPI::Unit* lolspire = NULL;
			if(UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Spire).size() > 0)
			{
				spireExists = true;
				lolspire = *UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Spire).begin();
			}
			if (spireExists && lolspire->isBeingConstructed())
			{
				if(lolspire->getRemainingBuildTime() <= 0.5 * lolspire->getType().buildTime())
				{
					// dont do anything (eggs sparen)
				}
				else
				{
					if ( ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >10) || ((nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >4))) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) / nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)) < 7))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Drone);
					}
					else
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
			else
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
				{
					if (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) >= ( 3*nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) + 5*nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
					else
					{
						addBuild(BWAPI::UnitTypes::Zerg_Drone);
					}
				}
				else
				{
					if (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) >= ( 3*nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) + 5*nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
					}
					else
					{
						addBuild(BWAPI::UnitTypes::Zerg_Drone);
					}
				}
			}
		}
	}

	// upgrades

	int zerglingtotaal = this->buildList.count(BWAPI::UnitTypes::Zerg_Zergling);
	zerglingtotaal += UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Zergling).size();
	zerglingtotaal += countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Zergling);
	if( zerglingtotaal > 7 && nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) == 0)
	{
		addWant(BWAPI::UpgradeTypes::Metabolic_Boost);
	}
	
	if( ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) + nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker)) * 2) >47 )
	{
		addWant(BWAPI::UpgradeTypes::Zerg_Missile_Attacks); // research ranged ground dmg
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 11 )
	{
		addWant(BWAPI::UpgradeTypes::Muscular_Augments); // research hydralisk speed
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 19 )
	{
		addWant(BWAPI::UpgradeTypes::Grooved_Spines); // research range
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 39 )
	{
		addWant(BWAPI::UpgradeTypes::Zerg_Melee_Attacks); // research melee ground damage
	}

	if( BWAPI::Broodwar->self()->supplyUsed() > 180) // >90 supply required (dubbel vanwege werking API)
	{
		addWant(BWAPI::UpgradeTypes::Zerg_Carapace); // upgrade ground armor
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hive) > 0 )
	{
		addWant(BWAPI::UpgradeTypes::Zerg_Melee_Attacks); // upgrade melee ground damage
		addWant(BWAPI::UpgradeTypes::Zerg_Carapace); // upgrade ground armor
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern) > 0 )
		{
			addWant(BWAPI::UpgradeTypes::Anabolic_Synthesis); // research ultralisk speed
			addWant(BWAPI::UpgradeTypes::Chitinous_Plating); // research ultralisk armor
		}
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0 )
		{
			addWant(BWAPI::TechTypes::Consume); // research sacrifice geval
		}
	}

	// vangnetten/algemeen

	if( (BWAPI::Broodwar->self()->supplyUsed() >= BWAPI::Broodwar->self()->supplyTotal()) && ( countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord) == 0) && (buildList.top().typenr == 1 && buildList.top().buildtype != BWAPI::UnitTypes::Zerg_Overlord) )
	{
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}
	
	if( (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) > BWAPI::Broodwar->self()->supplyTotal() ) //next 3 items in buildqueue increases the supply required > supplyprovided
	{
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}
	
	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) < ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery) * 8) + (nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) * 3) ) ) // not sufficient drones
	{
		addBuild(BWAPI::UnitTypes::Zerg_Drone);
	}
	
	UnitGroup hatcheries = getHatcheriesWithMinerals();
	UnitGroup geysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getGeysers());
	UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor);
	for(std::set<BWAPI::Unit*>::iterator hit=hatcheries.begin(); hit!=hatcheries.end(); hit++)
	{
		if(geysers.inRadius(8.00, (*hit)->getPosition()).size() > 0 && extractors.inRadius(8.00, (*hit)->getPosition()).size() == 0 && buildList.count(BWAPI::UnitTypes::Zerg_Extractor)+wantList.count(BWAPI::UnitTypes::Zerg_Extractor) < hatcheries.size())
		{
			addWant(BWAPI::UnitTypes::Zerg_Extractor);
		}
	}

	BWTA::BaseLocation* natural = getNaturalExpansion();
	int enemiesNearNatural = 0;
	if(natural != NULL)
	{
		enemiesNearNatural = this->eudm->getUG().inRadius(10.00, natural->getPosition()).size();
	}

	if( (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss || BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
		&& (nrOfEnemyBases() *2 >= nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)) && !buildList.containsExpand() && enemiesNearNatural == 0)
	{
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 3 && BWAPI::Broodwar->self()->minerals() > 250 && enemiesNearNatural == 0 && !buildList.containsExpand())
	{
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 3 && BWAPI::Broodwar->self()->minerals() > 250 && enemiesNearNatural > 0 && !buildList.containsExpand())
	{
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}

	//emergency
	// geskipt volgens Ben

	//Generieke rule:
	for(std::list<BuildItem>::iterator it=wantList.buildlist.begin(); it!=wantList.buildlist.end(); it++)
	{
		if((*it).typenr == 1)
		{
			int wantAantal = wantList.count((*it).buildtype);
			int buildAantal = buildList.count((*it).buildtype);
			int hebAantal = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType,(*it).buildtype).size();
			if(wantAantal < buildAantal+hebAantal)
			{
				addBuild((*it).buildtype);
			}
		}
		if((*it).typenr == 2)
		{
			if(buildList.count((*it).researchtype) == 0 && !BWAPI::Broodwar->self()->hasResearched((*it).researchtype))
			{
				addBuild((*it).researchtype);
			}
		}
		if((*it).typenr == 3)
		{
			if(buildList.count((*it).upgradetype) == 0 && !BWAPI::Broodwar->self()->getUpgradeLevel((*it).upgradetype))
			{
				addBuild((*it).upgradetype);
			}
		}
	}
}
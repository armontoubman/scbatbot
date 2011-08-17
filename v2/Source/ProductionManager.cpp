#pragma once
#include "ProductionManager.h"

#include <BWAPI.h>

#include "HighCommand.h"
#include "ProductList.h"
#include "Util.h"

ProductionManager::ProductionManager(HighCommand* h)
{
	this->hc = h;
	this->step = 1;
}

ProductList* ProductionManager::getWantList()
{
	return &(this->wantList);
}

ProductList* ProductionManager::getBuildList()
{
	return &(this->buildList);
}

void ProductionManager::update()
{
	updateLists();
}

/* update lists */

void ProductionManager::updateLists()
{
	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();
	if(enemyRace == BWAPI::Races::Protoss)
	{
		this->updateListsAgainstProtoss();
		this->updateListsAgainstProtossReinforcements();
	}
	else if(enemyRace == BWAPI::Races::Terran)
	{
		this->updateListsAgainstTerran();
		this->updateListsAgainstTerranReinforcements();
	}
	else if(enemyRace == BWAPI::Races::Zerg)
	{
		this->updateListsAgainstZerg();
		this->updateListsAgainstZergReinforcements();
	}
	updateListsUpgrades();
	updateListsGeneral();
	updateListsGenericRule();
}

void ProductionManager::updateListsAgainstProtoss()
{
	if(	this->step == 1 && this->buildList.isEmpty() && this->wantList.isEmpty()) 
	{
		addBuild(BWAPI::UnitTypes::Zerg_Drone, 5);
		//addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
		addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
		addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
		addBuild(BWAPI::UnitTypes::Zerg_Drone, 2); // *7*
		addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
		addWant(BWAPI::UnitTypes::Zerg_Hatchery);
		this->step = 2;
	}
	if(this->step == 2)
	{
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>8)
		{
			buildExpand();
		}
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>1) && buildList.count(BWAPI::UnitTypes::Zerg_Lair)==0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)==0) // *** nog geen bezig!
		{
			addBuild(BWAPI::UnitTypes::Zerg_Lair);
			addWant(BWAPI::UnitTypes::Zerg_Spire);
		}
		/*if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 4) && photonCannonNearBase() )
		{
			buildExpand();
		}*/
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Stargate) > 0)
		{
			addWant(BWAPI::UnitTypes::Zerg_Extractor);
			addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			this->step = 3;
		}
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2) ) // *7*
		{
			addWant(BWAPI::UnitTypes::Zerg_Extractor);
			addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			addBuild(BWAPI::UnitTypes::Zerg_Hydralisk, 10);
		}
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle)>0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2)) // deze was gecomment
		{
			addWant(BWAPI::UnitTypes::Zerg_Extractor);
			addWant(BWAPI::UnitTypes::Zerg_Spire);
		}
		if(  (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Spire)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
		{
			this->step = 3;
		}
	}
	if(this->step == 3)
	{
		if (buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>16 && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))<5) && nrOfOwn(BWAPI::UnitTypes::Zerg_Spire)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
		}
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))>2)
		{
			addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			//addWant(BWAPI::TechTypes::Lurker_Aspect); // fixe
			//addWant(BWAPI::UpgradeTypes::Pneumatized_Carapace); // fixe
		}
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot)+nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon)) > 13)
		{
			addWant(BWAPI::TechTypes::Lurker_Aspect);
		}
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>3) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk)+nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) > 20) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 30)) && nrOfExtraDronesRequired() < 3)
		{
			this->step = 4;
		}
	}
	if(this->step == 4)
	{
		addWant(BWAPI::UnitTypes::Zerg_Queens_Nest);
		addWant(BWAPI::UnitTypes::Zerg_Hive);
		addWant(BWAPI::UnitTypes::Zerg_Defiler_Mound);
		addWant(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
	}
}

void ProductionManager::updateListsAgainstProtossReinforcements()
{
	if(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 6)
	{
	
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0) || this->eudm->mapAirSize(this->eudm->enemyUnitsMap) >4)
		{
		
			if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 9 || nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)>0)
			{
			
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0 )
				{
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Defiler);
					}
					if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
					}
				}
				else
				{
					if( ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk))< 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3 )
					{
					
						addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
					}
					else
					{
					
						if (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
						else
						{
							if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2)
							{
								addBuild(BWAPI::UnitTypes::Zerg_Zergling);
							}
						}
					}
				}
			}
			else
			{
				if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
				{
				
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
			}
		} 
		else
		{
		
			if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
			{
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Defiler);
				}
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
				{
					addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
				}
			}
			else
			{
				if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<4)
				{
				
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
	}
	else if(nrOfEnemyMilitaryUnits() < 6)
	{
	
		if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Defiler);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
			}
		}
		else
		{
		
			if( (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)+nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3)
			{
			
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else
			{
				if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
				{
				
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
					{
					
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
	}

	if(nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 4)
	{
	
		if(nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Defiler);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
			}
		}
		else
		{
		
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
			{
			
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			else
			{
			
				if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				}
			}
		}
	}
	if(nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0)
	{
	
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2)
		{
			addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
		}
		else if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4))
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
		}
	}

	if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0) && (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))) // *7*fix
	{
	
		if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 5) && (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<3))
		{
			addBuild(BWAPI::UnitTypes::Zerg_Lurker);
		}
		else
		{
			if (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3 && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
		}
	
	}
	if( (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect)) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker) < 8) && (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<2)) // *7*fix
	{
	
		addBuild(BWAPI::UnitTypes::Zerg_Lurker);
	}
	if( ((BWAPI::Broodwar->self()->minerals()>200 || BWAPI::Broodwar->self()->gas()>200) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive) > nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus))) || nrOfOwnMilitaryUnits() <7 || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits()) )
	{
		if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Defiler);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
			}
		}
		if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 10) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3))
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
		}
		else 
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)) < 11) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3 )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else
			{
				if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den)>0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)>6 && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
				else
				{
					if (nrOfOwn(BWAPI::UnitTypes::Zerg_Spawning_Pool)>0 && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<1)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
	}
}

void ProductionManager::updateListsAgainstTerran()
{
}

void ProductionManager::updateListsAgainstTerranReinforcements()
{
}

void ProductionManager::updateListsAgainstZerg()
{
}

void ProductionManager::updateListsAgainstZergReinforcements()
{
}

void ProductionManager::updateListsUpgrades()
{
}

void ProductionManager::updateListsGeneral()
{
	if( (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) >= (BWAPI::Broodwar->self()->supplyTotal()+(buildList.count(BWAPI::UnitTypes::Zerg_Overlord)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)) && buildList.count(BWAPI::UnitTypes::Zerg_Overlord)<2 && (BWAPI::Broodwar->self()->supplyTotal() < 400) ) //next 3 items in buildqueue increases the supply required > supplyprovided
	{
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord);
	}

	if (BWAPI::Broodwar->self()->supplyUsed() > 38 && BWAPI::Broodwar->self()->supplyUsed()+16 < (BWAPI::Broodwar->self()->supplyTotal()+(buildList.count(BWAPI::UnitTypes::Zerg_Overlord)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)) && buildList.count(BWAPI::UnitTypes::Zerg_Overlord)<2 && (BWAPI::Broodwar->self()->supplyTotal() < 400) )
	{
		addBuild(BWAPI::UnitTypes::Zerg_Overlord);
	}
	if( nrOfExtraDronesRequired()+2 > (buildList.count(BWAPI::UnitTypes::Zerg_Drone)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Drone)) && buildList.count(BWAPI::UnitTypes::Zerg_Drone)<6 ) // not sufficient drones
	{
		addBuild(BWAPI::UnitTypes::Zerg_Drone, nrOfExtraDronesRequired()+2);
	}
	
	UnitGroup hatcheries = getOwnBasesWithMinerals()(isCompleted);
	UnitGroup geysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getGeysers());
	UnitGroup extractors = allEigenUnits()(Extractor);
	for each(BWAPI::Unit* hatchery in hatcheries)
	{
		if(geysers.inRadius(dist(10), hatchery->getPosition()).size() > 0 && extractors.inRadius(dist(10), hatchery->getPosition()).size() == 0 && nrOfExtraDronesRequired()<3 && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>9 && nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor)+buildList.count(BWAPI::UnitTypes::Zerg_Extractor)+this->hc->ctm->count(BWAPI::UnitTypes::Zerg_Extractor) < hatcheries.size())
		{
			addBuild(BWAPI::UnitTypes::Zerg_Extractor);
		}
	}

	if( buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss || BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
		&& (nrOfEnemyBases()*2 >= nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) && nrOfEnemyBases()>1 && !buildList.containsExpand() && this->hc->ctm->countExpand() == 0 && nrOfExtraDronesRequired()<3)
	{
		if (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>15)
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
		}
		else
		{
			buildExpand();
		}
	}
	if( BWAPI::Broodwar->self()->minerals() >= 1500 )
	{
		buildExpand();
		addBuild(BWAPI::UnitTypes::Zerg_Zergling,5);
	}
	if( BWAPI::Broodwar->self()->gas() >= 1000 )
	{
		addBuild(BWAPI::UnitTypes::Zerg_Mutalisk,3);
		addBuild(BWAPI::UnitTypes::Zerg_Defiler,3);
		addBuild(BWAPI::UnitTypes::Zerg_Hydralisk,3);
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 2 && BWAPI::Broodwar->self()->minerals() >= 500 && !buildList.containsExpand() && this->hc->ctm->countExpand() == 0 && nrOfExtraDronesRequired()<3)
	{
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 2 && BWAPI::Broodwar->self()->minerals() >= 550 && !buildList.containsExpand() && buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && this->hc->ctm->count(BWAPI::UnitTypes::Zerg_Hatchery) == 0 && this->hc->ctm->countExpand() == 0 && nrOfExtraDronesRequired()<4 )
	{
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	if(!buildList.containsExpand() && nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<5 && buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && this->hc->ctm->count(BWAPI::UnitTypes::Zerg_Hatchery) == 0 && BWAPI::Broodwar->self()->minerals() >= 500 && BWAPI::Broodwar->self()->gas()<200 )
	{
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	if(wantList.count(BWAPI::UnitTypes::Zerg_Spire) == 1 || wantList.count(BWAPI::TechTypes::Lurker_Aspect) == 1)
	{
		addWant(BWAPI::UnitTypes::Zerg_Extractor);
		addWant(BWAPI::UnitTypes::Zerg_Lair);
	}

	if((((BWAPI::Broodwar->self()->minerals()>300 || BWAPI::Broodwar->self()->gas()>300) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) == 2)) || (nrOfOwnMilitaryUnits() <7) || ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) > 2) || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits())))
	{
		if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 26) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < (nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk) * 7)) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<6) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler)+buildList.count(BWAPI::UnitTypes::Zerg_Defiler)) < (1 + ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) + nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) / 17) ) )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Defiler);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20)  && (buildList.count(BWAPI::UnitTypes::Zerg_Ultralisk)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Ultralisk);
			}
		}
		if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 10) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3) )
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hydralisk,3);
		}
		else 
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+(buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk))) < 11) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk,3);
			}
			else
			{
				if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 6) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk,3);
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<1)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling,3);
					}
				}
			}
		}
	}
}

void ProductionManager::updateListsGenericRule()
{
	for each(Product product in this->wantList.productList)
	{
		if(product.type == BuildProduct)
		{
			int wantAmount = this->wantList.count(product.buildtype);
			int buildAmount = this->buildList.count(product.buildtype);
			int haveAmount = allEigenUnits()(GetType, product.buildtype).size();
			int contractAmount = 0;
			if(product.buildtype.isBuilding())
			{
				contractAmount = this->hc->ctm->count(product.buildtype);
			}
			if(product.buildtype == BWAPI::UnitTypes::Zerg_Lair)
			{
				haveAmount = haveAmount + allEigenUnits()(Hive).size();
			}
			if(wantAmount > (buildAmount + haveAmount + contractAmount))
			{
				if(product.buildtype == BWAPI::UnitTypes::Zerg_Lair || product.buildtype == BWAPI::UnitTypes::Zerg_Hive)
				{
					if(allEigenUnits()(Lair, Hive, Hatchery)(isMorphing).size() == 0)
					{
						addBuild(product.buildtype);
					}
				}
				else
				{
					addBuild(product.buildtype);
				}
			}
		}
		if(product.type == TechProduct)
		{
		}
		if(product.type == UpgradeProduct)
		{
		}
	}
}

/* end update lists */

/* ProductList interface */

void ProductionManager::addWant(BWAPI::UnitType unittype)
{
	if (!this->wantListContains(unittype))
	{
		addWant(unittype, 1);
	}
}

void ProductionManager::addWant(BWAPI::UnitType unittype, int amount)
{
	for(int i=0; i<amount; i++)
	{
		this->wantList.addProduct(Product(unittype, 1));
	}
}

void ProductionManager::addWant(BWAPI::TechType techtype)
{
	if (this->wantList.count(techtype)<1)
	{
		this->wantList.addProduct(Product(techtype, 1));
	}

}

void ProductionManager::addWant(BWAPI::UpgradeType upgradetype)
{
	if (this->wantList.count(upgradetype)<1)
	{
		this->wantList.addProduct(Product(upgradetype, 1));
	}
}

void ProductionManager::addBuild(BWAPI::UnitType unittype)
{
	addBuild(unittype, 1);
}

void ProductionManager::addBuild(BWAPI::UnitType unittype, int amount)
{
	for(int i=0; i<amount; i++)
	{
		this->buildList.addProduct(Product(unittype, 1));
	}
}

void ProductionManager::addBuildTop(BWAPI::UnitType unittype)
{
	this->buildList.addProductTop(Product(unittype, 1));
}

void ProductionManager::addBuild(BWAPI::TechType techtype)
{
	if (this->buildList.count(techtype)<1)
	{
		this->buildList.addProduct(Product(techtype, 1));
	}
}

void ProductionManager::addBuild(BWAPI::UpgradeType upgradetype)
{
	if (this->buildList.count(upgradetype)<1)
	{
		this->buildList.addProduct(Product(upgradetype, 1));
	}
}

bool ProductionManager::wantListContains(BWAPI::UnitType unittype)
{
	return this->wantList.count(unittype) > 0;
}

bool ProductionManager::buildListContains(BWAPI::UnitType unittype)
{
	return this->buildList.count(unittype) > 0;
}

/* end ProductList interface */

/* intel */

int ProductionManager::nrOfEnemy(BWAPI::UnitType unittype)
{
	return this->hc->eudm->count(unittype);
}

int ProductionManager::nrOfOwn(BWAPI::UnitType unittype)
{
	return allEigenUnits()(GetType, unittype).size();
}

int ProductionManager::nrOfEnemyBases()
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
		return nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive);
	}
	return 0;
}

int ProductionManager::nrOfOwnBasesWithMinerals()
{
	int result = 0;

	UnitGroup bases = allEigenUnits()(Hatchery, Lair, Hive);
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());

	for each(BWAPI::Unit* base in bases)
	{
		UnitGroup mineralsNearBase = minerals.inRadius(dist(10), base->getPosition());
		if(mineralsNearBase.size() > 0)
		{
			result = result+1;
		}
	}

	return result;
}

UnitGroup ProductionManager::getOwnBasesWithMinerals()
{
	UnitGroup result;

	UnitGroup bases = allEigenUnits()(Hatchery, Lair, Hive);
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());

	for each(BWAPI::Unit* base in bases)
	{
		UnitGroup mineralsNearBase = minerals.inRadius(dist(10), base->getPosition());
		if(mineralsNearBase.size() > 0)
		{
			result.insert(base);
		}
	}

	return result;
}

int ProductionManager::nrOfOwnMilitaryUnits()
{
	return filterMilitary(allEigenUnits()).size();
}

int ProductionManager::nrOfEnemyMilitaryUnits()
{
	return 0;
}

int ProductionManager::countEggsMorphingInto(BWAPI::UnitType unittype)
{
	int totaal = 0;
	UnitGroup eggs = allEigenUnits()(Egg);
	for each(BWAPI::Unit* egg in eggs)
	{
		if(egg->getBuildType() == unittype)
		{
			totaal++;
		}
	}
	return totaal;
}

/* end intel */

void ProductionManager::buildExpand()
{
	if(this->buildList.countExpand() == 0)
	{
		this->buildList.addProduct(Product().expand());
	}
}

int ProductionManager::nrOfExtraDronesRequired()
{
	int result = 0;

	UnitGroup bases = allEigenUnits()(Hatchery, Lair, Hive);
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());

	for each(BWAPI::Unit* base in bases)
	{
		UnitGroup mineralsNearBase = minerals.inRadius(dist(10), base->getPosition());
		result = result + mineralsNearBase.size();
	}

	result = result + allEigenUnits()(Extractor)(isCompleted).size()*3;
	result = result - allEigenUnits()(isWorker).not(isMorphing).not(isConstructing).size();

	return result;
}
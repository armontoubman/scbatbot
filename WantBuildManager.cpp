#include "WantBuildManager.h"
#include <BWAPI.h>
#include <UnitGroup.h>
#include "EnemyUnitDataManager.h"
#include "BuildItem.h"
#include "BuildList.h"
#include "MicroManager.h"
#include "BuildOrderManager.h"
#include "BaseManager.h"
#include "Util.h"
#include <sstream>
#include "HighCommand.h"
#include "BuildingPlacer.h"

WantBuildManager::WantBuildManager(EnemyUnitDataManager* e, BuildOrderManager* b, BaseManager* ba, HighCommand* h, MicroManager* m)
{
	this->eudm = e;
	this->bom = b;
	this->bm = ba;
	this->hc = h;
	this->mm = m;
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
	// voeg want toe, als het er nog niet in zit
	if (!this->wantListContains(unittype))
	{
		addWant(unittype, 1);
	}
}

void WantBuildManager::addWant(BWAPI::UnitType unittype, int amount)
{
	// hier mag het wel extra worden toegevoegd
	for(int i=0; i<amount; i++)
	{
		this->wantList.addItem(BuildItem(unittype, 1));
	}
}

void WantBuildManager::addWant(BWAPI::TechType techtype)
{
	if (this->wantList.count(techtype)<1)
	{
		this->wantList.addItem(BuildItem(techtype, 1));
	}

}

void WantBuildManager::addWant(BWAPI::UpgradeType upgradetype)
{
	if (this->wantList.count(upgradetype)<1)
	{
		this->wantList.addItem(BuildItem(upgradetype, 1));
	}
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
		log(std::string("addBuild ").append(unittype.getName()).append("\n").c_str());
	}
}

void WantBuildManager::addBuildTop(BWAPI::UnitType unittype)
{
	this->buildList.addItemTop(BuildItem(unittype, 1));
}

void WantBuildManager::addBuild(BWAPI::TechType techtype)
{
	if (this->wantList.count(techtype)<1)
	{
		this->buildList.addItem(BuildItem(techtype, 1));
	}
}

void WantBuildManager::addBuild(BWAPI::UpgradeType upgradetype)
{
	if (this->wantList.count(upgradetype)<1)
	{
		this->buildList.addItem(BuildItem(upgradetype, 1));
	}
}

void WantBuildManager::wantExpand()
{
	this->wantList.addItem(BuildItem().expand());
}

void WantBuildManager::buildExpand()
{
	if(this->buildList.countExpand() == 0)
	{
		this->buildList.addItem(BuildItem().expand());
	}
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
				if(it->first->getDistance(*hit) < dist(15.00))
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
			if(!(algehad.count(it->buildtype) > 0))
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
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)  // stond eerst it!=minerals.end(), lijkt me beetje raar als de rest mit staat? ***
		{
			if((*it)->getDistance(*mit) <= dist(8.00))
			{
				result.insert(*it);
				break;
			}
		}
	}
	return result;
}

int WantBuildManager::dronesRequiredAll()
{
	int amount = 0;
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	log(std::string("dronesreq mins: ").append(intToString(minerals.size())).append("\n").c_str());
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	log(std::string("dronesreq hatch: ").append(intToString(hatcheries.size())).append("\n").c_str());

	// minerals per basis
	for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)
	{
		for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
		{
			// CRASH HIER
			//UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(10.00), (*it)->getPosition());
			//UnitGroup enemiesinrange = this->mm->enemiesInRange((*it)->getPosition(), dist(10.00), 0);
			//if ((enemiesinrange.size() > 0 && this->mm->amountCanAttackGround(enemiesinrange) < 5) || (allies.size()>2))
			{
				if((*it)->getDistance(*mit) <= dist(10.00))
				{
					amount++;
					break;
				}
			}
		}
	}
	log(std::string("minsperbasis: ").append(intToString(amount)).append("\n").c_str());
	// extractors
	amount += (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted).size()*3);
	log(std::string("plusextractors: ").append(intToString(amount)).append("\n").c_str());
	// aantal workers al aan de slag
	amount -= UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isWorker).not(isMorphing).not(isConstructing).size();
	log(std::string("minus workers: ").append(intToString(amount)).append("\n").c_str());
	if (amount < 0)
	{
		return 0;
	}
	return amount;
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
	log("WantBuildManager::update()\n");
	logBuildList(this->buildList);
	// Actual building of items
	if(buildList.size() > 0)
	{
		BuildItem b = buildList.top();

		// check of gebouw al aant bouwe is
		if(b.typenr == 1 || b.typenr == 4)
		{
			if(b.buildtype.isBuilding() || b.typenr == 4)
			{
				UnitGroup bezig = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, b.buildtype)(isBeingConstructed);
				bezig = bezig + (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, b.buildtype)(Extractor).not(isCompleted));
				if(b.typenr == 4)
				{
					std::set<BWTA::BaseLocation*> baselocs = BWTA::getBaseLocations();
					UnitGroup exps = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery)(isBeingConstructed);
					for each(BWAPI::Unit* exp in exps)
					{
						for each(BWTA::BaseLocation* baseloc in baselocs)
						{
							if(baseloc->getTilePosition() == exp->getTilePosition())
							{
								bezig.insert(exp);
							}
						}
					}
				}
				// bezig kan size > 1 hebben
				for each(BWAPI::Unit* lolgebouw in bezig)
				{
					log("bezig met: ");
					log(b.buildtype.getName().append("\n").c_str());
					if(b.typenr == 1 && b.buildtype == lolgebouw->getType() && lolgebouw->getRemainingBuildTime() / lolgebouw->getType().buildTime() >= 0.9)
					{
						log(b.buildtype.getName().append(" ").c_str());
						log("started and removed from top\n");
						buildList.removeTop();
						b = buildList.top();
						return;
					}
					if(b.typenr == 4 && lolgebouw->getType() == BWAPI::UnitTypes::Zerg_Hatchery && lolgebouw->getRemainingBuildTime() / lolgebouw->getType().buildTime() >= 0.9)
					{
						log("expand started and removed from top\n");
						buildList.removeTop();
						b = buildList.top();
						return;
					}
				}
			}
		}
		//einde remove build als aant bouwe is

		log(std::string(intToString(b.typenr)).append("=b.typenr\n").c_str());
		if(b.typenr == 1)
		{
			if(!requirementsSatisfied(b.buildtype) || (BWAPI::Broodwar->self()->gas() < b.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
			{
				log("can't make\n\t");
				log(b.buildtype.getName().append("\n").c_str());
				log("remove\n");
				buildList.removeTop();
				log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
				return;
			}
			else
			{
				if(buildList.count(BWAPI::UnitTypes::Zerg_Drone)>2 && b.buildtype!=BWAPI::UnitTypes::Zerg_Drone && b.buildtype!=BWAPI::UnitTypes::Zerg_Overlord && !isBeingHandled(b)) // conditie die checkt of b niet al in maak is
				{
					log(b.buildtype.getName().append(" ").c_str());
					log("drones voorrang\n");
					buildList.removeTop();
					return;
				}
				else
				{
					if(canBeMade(b.buildtype))
					{
						if(!b.buildtype.isBuilding())
						{
							log("can make\n\t");
							log(b.buildtype.getName().append("\n").c_str());
							(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Larva).begin())->morph(b.buildtype);
							buildList.removeTop();
							log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
							return;
						}
						else
						{
							log("can make\n\t");
							log(b.buildtype.getName().append("\n").c_str());
							if(b.buildtype == BWAPI::UnitTypes::Zerg_Lair)
							{
								if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
								{
									log("probeer hatchery naar lair te morphen...\n");
									this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Lair);
									log("gelukt.\n");
									buildList.removeTop();
									return;
								}
								else
								{
									UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
									if(hatcheries.size() == 0)
									{
										log("kan geen lair maken, geen hatcheries\n");
										buildList.removeTop();
									}
									else
									{
										log("probeer hatchery naar lair te morphen...\n");
										(*hatcheries.begin())->morph(BWAPI::UnitTypes::Zerg_Lair);
										log("gelukt.\n");
										buildList.removeTop();
										return;
									}
								}
							}
							else
							{
								if(b.buildtype == BWAPI::UnitTypes::Zerg_Hive)
								{
									if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Lair)
									{
										log("probeer lair naar hive te morphen...\n");
										this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Hive);
										log("gelukt.\n");
										buildList.removeTop();
										return;
									}
									else
									{
										UnitGroup lairs = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Lair);
										if(lairs.size() == 0)
										{
											log("kan geen hive maken, geen lairs\n");
											buildList.removeTop();
										}
										else
										{
											log("probeer lair naar hive te morphen...\n");
											(*lairs.begin())->morph(BWAPI::UnitTypes::Zerg_Hive);
											log("gelukt.\n");
											buildList.removeTop();
											return;
										}
									}
								}
								else
								{
									bool albezig = isBeingHandled(b);
									if(albezig == false)
									{
										log("bouwen maar\n");
										bool gogo = false;
										BWAPI::TilePosition lokatie;
										if(b.buildtype == BWAPI::UnitTypes::Zerg_Extractor)
										{
											lokatie = placeFoundExtractor();
											if(UnitGroup::getUnitGroup(BWAPI::Broodwar->unitsOnTile(lokatie.x(), lokatie.y()))(Vespene_Geyser).size() == 1
												&& UnitGroup::getUnitGroup(BWAPI::Broodwar->unitsOnTile(lokatie.x(), lokatie.y()))(Extractor, Refinery, Assimilator).size() == 0)
											{
												gogo = true;
											}
										}
										else
										{
											lokatie = placeFound(b.buildtype);
											gogo = true;
										}
										if(gogo)
										{
											bouwStruc(lokatie, b.buildtype);
											//buildList.removeTop(); // crash
											log("bouwen gelukt\n");
										}
										else
										{
											log("bouwen mislukt, geen locatie\n");
											buildList.removeTop();
										}
									}
									else
									{
										//buildList.removeTop(); // crash
										log("wordt al gemaakt\n");
									}
									log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
									return;
								}
							}
						}
					}
				}
			}
		}
		log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
		if(b.typenr == 2)
		{
			if(!requirementsSatisfied(b.researchtype) || (BWAPI::Broodwar->self()->gas() < b.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
			{
				buildList.removeTop();
				return;
			}
			else
			{
				if(canBeMade(b.researchtype) && !BWAPI::Broodwar->self()->hasResearched(b.researchtype))
				{
					this->bom->research(b.researchtype, 1);
					buildList.removeTop();
					return;
				} 
			}
		}
		if(b.typenr == 3)
		{
			if(!requirementsSatisfied(b.upgradetype) || (BWAPI::Broodwar->self()->gas() < b.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
			{
				buildList.removeTop();
				return;
			}
			else
			{
				if(canBeMade(b.upgradetype) && (b.upgradetype.maxRepeats() > BWAPI::Broodwar->self()->getUpgradeLevel(b.upgradetype)))
				{
					this->bom->upgrade(BWAPI::Broodwar->self()->getUpgradeLevel(b.upgradetype)+1, b.upgradetype, 1);
					buildList.removeTop();
					return;
				}
			}
		}
		if(b.typenr == 4)
		{
			if(requirementsSatisfied(BWAPI::UnitTypes::Zerg_Hatchery) && canBeMade(BWAPI::UnitTypes::Zerg_Hatchery))
			{
				doExpand();
				//buildList.removeTop();
				return;
			}
		}
		// als eerste een gebouw is en 2e een unit = oke, 1=unit+2=gebouw, wordt wat lastiger moet je weer het 'buildstuff' doen, mogelijk aparte methode gewoon voor maken. Als je beide build hebt, dan wordt het maybe nog erger, moge we nog meer checks doen... zoals pak geen drone die al buildorder heeft. Maja zo vaak bouw je geen 2 dinge tegelijk als zerg.
		if (buildList.size()>1)
		{
			BuildItem v = buildList.getSecond();
			if (v.typenr == 1)
			{
				if(!requirementsSatisfied(v.buildtype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					log("can't make second\n\t");
					log(v.buildtype.getName().append("\n").c_str());
					log("remove second\n");
					buildList.removeSecond();
					log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
					return;
				}
				else
				{
					if (bothCanBeMade(b.buildtype, v.buildtype))
					{
						log("can make second\n\t");
						log(v.buildtype.getName().append("\n").c_str());
						(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Larva).begin())->morph(v.buildtype);
						buildList.removeSecond();
						log(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
						return;
					}
				}
			}
			if (v.typenr == 2 && ((b.typenr == 1) || b.typenr == 4))
			{
				if(!requirementsSatisfied(v.researchtype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					buildList.removeSecond();
					return;
				}
				else
				{
					if(bothCanBeMade(b.buildtype, v.researchtype) && !BWAPI::Broodwar->self()->hasResearched(v.researchtype))
					{
						this->bom->research(v.researchtype, 1);
						buildList.removeSecond();
						return;
					} 
				}
			}
			if(v.typenr == 3 && ((b.typenr == 1) || b.typenr == 4))
			{
				if(!requirementsSatisfied(v.upgradetype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					buildList.removeSecond();
					return;
				}
				else
				{
					if(bothCanBeMade(b.buildtype, v.upgradetype) && (v.upgradetype.maxRepeats() > BWAPI::Broodwar->self()->getUpgradeLevel(v.upgradetype)))
					{
						this->bom->upgrade(BWAPI::Broodwar->self()->getUpgradeLevel(v.upgradetype)+1, v.upgradetype, 1);
						buildList.removeSecond();
						return;
					}
				}
			}
		}
	}
	else
	{
		log("buildlist is leeg\n");
	}
}

void WantBuildManager::doLists()
{
	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();

	stap = 1;

	log("doLists\n\n");

	if(enemyRace == BWAPI::Races::Protoss)
	{
		
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			log("doLists stap 1 lege lijsten\n");
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 5);
			//addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 2); // *7*
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
			stap = 2;
		}
		if( stap == 2)
		{
			log("doLists stap 2\n");
			if(	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Forge) == 0)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) < 4) && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<4)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
				log("dolists p first cond\n");
			}
			else 
			{
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 2) && photonCannonNearBase() ) // *7* goedgekeurd, check buildexpand ligt in methode zelf
				{
					buildExpand();
					log("dl p 1-1\n");
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Forge) >= 1)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) < 9)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 10)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2)	) // nakijken
				{
					buildExpand();
					log("dl p 1-2\n");
				}
				if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Stargate) > 0)
				{
					log("dl p 1-3\n");
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk,10);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7) || ((nrOfEnemy(BWAPI::UnitTypes::Protoss_Gateway) > 2)  && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0)))
				{
					log("dl p 1-4\n");
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 5)) // *7*
				{
					log("dl p 1-5\n");
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk, 10);
					stap = 3;
				}
				//if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0) || (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle)>0)) // deze was gecomment
				{
					log("dl p 1-6\n");
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					stap = 3;
				}
				if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0)	&& (nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 3)) // *7*fix
				{
					log("dl p 1-7\n");
					addBuild(BWAPI::UnitTypes::Zerg_Zergling, 20);
					if(	(((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))+wantList.count(BWAPI::UnitTypes::Zerg_Hatchery)+wantList.count(BWAPI::UnitTypes::Zerg_Lair)+wantList.count(BWAPI::UnitTypes::Zerg_Hive)) < 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den)==0)) // *7*
					{
						log("dl p 1-7-1\n");
						addWant(BWAPI::UnitTypes::Zerg_Hatchery,1);
					}
					else
					{
						log("dl p 1-7-2\n");
						addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
						addWant(BWAPI::UnitTypes::Zerg_Extractor);
					}
				}
				if(  (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>2) && !wantListContains(BWAPI::UnitTypes::Zerg_Extractor))
				{
					log("dl p 1-8\n");
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
					stap = 3;
				}
			}
		}
		if( stap == 3)
		{
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addWant(BWAPI::TechTypes::Lurker_Aspect); // fixe
				addWant(BWAPI::UpgradeTypes::Pneumatized_Carapace); // fixe
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
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk, 11);
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 9 )
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk, 20);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot)+nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon)) > 13)
			{
				addWant(BWAPI::TechTypes::Lurker_Aspect);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk)+nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) > 15) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 40) && wantListContains(BWAPI::UnitTypes::Zerg_Spire) && wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)  > (3*(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)))))
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
		log("dl p r start\n");
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 9)
		{
			log("dl p r 1\n");
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0) || this->eudm->getUG()(isFlyer).size() >4)
			{
				log("dl p r 1-1\n");
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 9)
				{
					log("dl p r 1-2\n");
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
						if( ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk))< 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) )
						{
							log("dl p r 1-2-2-m\n");
							addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
						}
						else
						{
							log("dl p r 1-2-2-h\n");
							if (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
							{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
							}
						}
					}
				}
				else
				{
					if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
					{
						log("dl p r 1-1-2\n");
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
				}
			} 
			else
			{
				log("dl p r 2\n");
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
				else if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<4)
				{
					log("dl p r 3\n");
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
			}
		}
		else if( nrOfEnemyMilitaryUnits() < 6)
		{
			log("dl p r 3\n");
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
				log("dl p r 3-2\n");
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2)
				{
					log("dl p r 3-2-1\n");
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
					{
						log("dl p r 3-2-2\n");
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
		log("dl p r elseifuit\n");
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 4)
		{
			log("dl p r 4\n");
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
				log("dl p r 4-2\n");
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
				{
					log("dl p r 4-2-1\n");
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
				else
				{
					log("dl p r 4-2-2\n");
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0)
		{
			log("dl p r 5\n");
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
		}
		log("dl p r 5 -> 6 check\n");
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0) && (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))) // *7*fix
		{
			log("dl p r 6\n");
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 5) && (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Lurker);
			}
			else
			{
				if (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
			}

		}
		if( (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect)) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker) < 8) && (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<2)) // *7*fix
		{
			log("dl p r 7\n");
			addBuild(BWAPI::UnitTypes::Zerg_Lurker);
		}
		if( (((BWAPI::Broodwar->self()->minerals()>200 || BWAPI::Broodwar->self()->gas()>200) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) > nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus)) || nrOfOwnMilitaryUnits() <7 || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits())) && !buildList.containsUnits() )
		{
			log("dl p r 8\n");
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
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			else 
			{
				log("dl p r 8b-2\n");
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+(buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk))) < 11) )
				{
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					if ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 6) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3) )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<1)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Zergling);
						}
					}
				}
			}
		}
		log("dl p klaar\n");
	}
	else if(enemyRace == BWAPI::Races::Terran)
	{
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 5);
			//addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 2);
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
			stap = 2;
		}
		if( stap == 2)
		{
			if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<3)
			{
				buildExpand();
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Barracks) > 1)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Vulture) > 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Starport) > 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
			if( wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && !wantListContains(BWAPI::UnitTypes::Zerg_Spire))
			{
				if(nrOfEnemy(BWAPI::UnitTypes::Terran_Command_Center) > 1)
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
				else
				{
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<3) && !buildList.containsExpand() && buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0)
					{
						buildExpand();
					}
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
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Marine) > 8)
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addWant(BWAPI::TechTypes::Lurker_Aspect);
			}
			if((nrOfEnemy(BWAPI::UnitTypes::Terran_Goliath) > 4) && ((nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + nrOfEnemy(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)) == 0))
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
			if((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>2) && wantListIsCompleted() ) 
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
		if( nrOfEnemy(BWAPI::UnitTypes::Terran_Marine) > 8)
		{
			if( nrOfEnemy(BWAPI::UnitTypes::Terran_Science_Vessel) > 0)
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0 && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk)>7) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)>4) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Scourge)>3)))
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
					if(nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
					{
						if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) < 5 && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
						if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect) && buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<3)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Lurker);
						}
						else
						{
							if (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
							{
								addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
							}
						}
					}
					else if ( nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2)
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
							if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den)>0 && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
							{
								addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
							}
						}
						else
						{
							if (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3)
							{
								addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
							}
						}
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<3)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Lurker);
						}
					}
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2 && nrOfOwn(BWAPI::UnitTypes::Zerg_Spire)>0 )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<1 && nrOfOwn(BWAPI::UnitTypes::Zerg_Spawning_Pool)>0)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Zergling);
						}
					}
				}
			}
		}
		else
		{
			if( (nrOfEnemy(BWAPI::UnitTypes::Terran_Goliath) < 4) && (nrOfEnemy(BWAPI::UnitTypes::Terran_Missile_Turret) < 6) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0)
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
					if( (nrOfEnemy(BWAPI::UnitTypes::Terran_Science_Vessel) < 3) && BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 2) && (nrOfEnemy(BWAPI::UnitTypes::Terran_Vulture) < 5) && (buildList.count(BWAPI::UnitTypes::Zerg_Lurker)<3))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Lurker);
					}
					else
					{
						if (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den)>0 && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
						}
					}
				}
			}
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
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 ) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)+buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)) < 11) )
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
	else if(enemyRace == BWAPI::Races::Zerg)
	{
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 5);
			//addWant(BWAPI::UnitTypes::Zerg_Drone, 9);
			addBuild(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addWant(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			addBuild(BWAPI::UnitTypes::Zerg_Drone, 2);
			addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
			stap = 2;
		}
		if( stap == 2)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 8) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Zergling) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) < 2))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Sunken_Colony);
			}
			if( (((nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 0 || (nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Mutalisk) == 0)) && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2 && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)<10 )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			//if( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Mutalisk) > 3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) == 0) )
			//{
			//	addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
			//	addWant(BWAPI::UnitTypes::Zerg_Spore_Colony);
			//}
			if( nrOfEnemy(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
			{
				buildExpand();
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Spire) > 0) || ( ((nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 1) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 7) ))
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if ( ((nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Spire) > 0) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) < 3) )
			{
				if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 7) && !buildList.containsExpand())
				{
					buildExpand();
				}
				else
				{
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
					addWant(BWAPI::UnitTypes::Zerg_Spire);
				}
			}
			if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) > 2)
			{
				stap = 3;
			}

		}
		if( stap == 3)
		{
			addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			if(!buildList.containsExpand())
			{
				buildExpand();
			}
		}			
		// reinforcements
		UnitGroup completedspires = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Spire);
		bool atleastonecompletedspire = false;
		for each(BWAPI::Unit* spire in completedspires)
		{
			if(spire->isCompleted())
			{
				atleastonecompletedspire = true;
			}
		}
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0 && atleastonecompletedspire)
		{
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)<5) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 6) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) <1)) && (BWAPI::Broodwar->self()->gas() < 70) && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			else
			{
				if(nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) >= 11 && nrOfEnemy(BWAPI::UnitTypes::Zerg_Hydralisk) > 7 && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
				{
					addBuild(BWAPI::UnitTypes::Zerg_Zergling, 3);
				}
				else
				{
					if (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<3)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
					}
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
					// dont do anything (eggs sparen), mogelijk werkt het niet eens, lol.. alle moeite voor nix :D
				}
				else
				{
					if ( ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >10) || ((nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >4))) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) / (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))) < 7) && buildList.count(BWAPI::UnitTypes::Zerg_Drone)<3 )
					{
						addBuild(BWAPI::UnitTypes::Zerg_Drone);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<1)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Zergling);
						}
					}
				}
			}
			else
			{
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
				{
					if (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) >= ( 3*nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) + 5*nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<3))
					{
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Drone)<2)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Drone);
						}
					}
				}
				else
				{
					if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) >= ( 2*nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) + 4*(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)))) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)<17)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Drone)<1)
						{
							addBuild(BWAPI::UnitTypes::Zerg_Drone);
						}
					}
				}
			}
		}
	}

	// upgrades

	int zerglingtotaal = this->buildList.count(BWAPI::UnitTypes::Zerg_Zergling);
	zerglingtotaal += UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Zergling).size();
	zerglingtotaal += countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Zergling);
	log("dl g zlingtotal\n");
	if (zerglingtotaal > 10)
	{
		log("dl g zlingtotal satis\n");
		addWant(BWAPI::UpgradeTypes::Metabolic_Boost);
	}
	
	if( ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) + nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker)) * 2) >47 && buildList.count(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)<1 )
	{
		log("dl upgr missile\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Missile_Attacks); // research ranged ground dmg
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 11 && buildList.count(BWAPI::UpgradeTypes::Muscular_Augments)<1 )
	{
		log("dl upgr muscu\n");
		addWant(BWAPI::UpgradeTypes::Muscular_Augments); // research hydralisk speed
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}
	log("dl upgr muscu->spines\n");
	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 19 && buildList.count(BWAPI::UpgradeTypes::Grooved_Spines)<1 )
	{
		log("dl upgr spines\n");
		addWant(BWAPI::UpgradeTypes::Grooved_Spines); // research range
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)+nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) > 39 && buildList.count(BWAPI::UpgradeTypes::Zerg_Melee_Attacks)<1 )
	{
		log("dl upgr melee\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Melee_Attacks); // research melee ground damage
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( BWAPI::Broodwar->self()->supplyUsed() > 180 && buildList.count(BWAPI::UpgradeTypes::Zerg_Carapace)<1 ) // >90 supply required (dubbel vanwege werking API)
	{
		log("dl upgr cara\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Carapace); // upgrade ground armor
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}
	log("dl upgr cara->burrow\n");

	if( BWAPI::Broodwar->self()->supplyUsed() > 90 && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 15 || nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20) && buildList.count(BWAPI::TechTypes::Burrowing)<1 )
	{
		log("dl upgr burrow\n");
		addWant(BWAPI::TechTypes::Burrowing);
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hive) > 0 )
	{
		log("dl hive tech\n");
		if (buildList.count(BWAPI::UpgradeTypes::Zerg_Melee_Attacks)<1)
		{
			addWant(BWAPI::UpgradeTypes::Zerg_Melee_Attacks); // upgrade melee ground damage
			addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
		}
		if (buildList.count(BWAPI::UpgradeTypes::Zerg_Carapace)<1)
		{
			addWant(BWAPI::UpgradeTypes::Zerg_Carapace); // upgrade ground armor
			addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
		}
		if (buildList.count(BWAPI::UpgradeTypes::Adrenal_Glands)<1)
		{
			addWant(BWAPI::UpgradeTypes::Adrenal_Glands); // upgrade ground armor
			addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
		}
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern) > 0 )
		{
			if (buildList.count(BWAPI::UpgradeTypes::Anabolic_Synthesis)<1)
			{
				addWant(BWAPI::UpgradeTypes::Anabolic_Synthesis); // research ultralisk speed
			}
			if (buildList.count(BWAPI::UpgradeTypes::Chitinous_Plating)<1)
			{
				addWant(BWAPI::UpgradeTypes::Chitinous_Plating); // research ultralisk armor
			}
		}
		if( nrOfOwn(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0 )
		{
			if (buildList.count(BWAPI::TechTypes::Consume)<1)
			{
				addWant(BWAPI::TechTypes::Consume); // research sacrifice geval
			}
		}
	}

	// vangnetten/algemeen
	log("dl v start\n");
	/*log(std::string("overlord eggs: ").append(intToString(countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord))).append("\n").c_str());
	if( BWAPI::Broodwar->self()->supplyUsed() >= (BWAPI::Broodwar->self()->supplyTotal()+(buildList.count(BWAPI::UnitTypes::Zerg_Overlord)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord))) && (buildList.top().typenr == 1 && buildList.top().buildtype != BWAPI::UnitTypes::Zerg_Overlord) && (BWAPI::Broodwar->self()->supplyTotal() < 400)) // voorkomt dat het overlords spamt als het al op max bevindt
	{
		log("dl v buildtopoverlord1\n");
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}*/
	
	if( (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) > (BWAPI::Broodwar->self()->supplyTotal()+(countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)) && buildList.top().buildtype != BWAPI::UnitTypes::Zerg_Overlord && (BWAPI::Broodwar->self()->supplyTotal() < 400) ) //next 3 items in buildqueue increases the supply required > supplyprovided
	{
		log("dl v buildtopoverlord2\n");
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}
	
	if( dronesRequiredAll() > (buildList.count(BWAPI::UnitTypes::Zerg_Drone)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Drone)) && buildList.count(BWAPI::UnitTypes::Zerg_Drone)<6 ) // not sufficient drones
	{
		log("drone build\n");	
		addBuild(BWAPI::UnitTypes::Zerg_Drone);
	}
	
	UnitGroup hatcheries = getHatcheriesWithMinerals()(isCompleted);
	UnitGroup geysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getGeysers());
	UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor);
	log("dl v extrac\n");
	for(std::set<BWAPI::Unit*>::iterator hit=hatcheries.begin(); hit!=hatcheries.end(); hit++)
	{
		if(geysers.inRadius(dist(10.00), (*hit)->getPosition()).size() > 0 && extractors.inRadius(dist(10.00), (*hit)->getPosition()).size() == 0 && buildList.count(BWAPI::UnitTypes::Zerg_Extractor)+wantList.count(BWAPI::UnitTypes::Zerg_Extractor) < hatcheries.size())
		{
			log("dl v addbuild extrac\n");
			addBuild(BWAPI::UnitTypes::Zerg_Extractor);
		}
	}
	log("dl v startcheck hatcheryexpands\n");
	BWTA::BaseLocation* natural = getNaturalExpansion();
	int enemiesNearNatural = 0;
	if(natural != NULL)
	{
		enemiesNearNatural = this->eudm->getEnemyUnitsInRadius(dist(10.00), natural->getPosition()).size();
	}

	if( (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss || BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
		&& (nrOfEnemyBases() *2 >= nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)) && !buildList.containsExpand() && enemiesNearNatural == 0)
	{
		log("dl v expand 1\n");
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 3 && BWAPI::Broodwar->self()->minerals() > 250 && enemiesNearNatural == 0 && !buildList.containsExpand())
	{
		log("dl v expand 2\n");
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 3 && BWAPI::Broodwar->self()->minerals() > 250 && enemiesNearNatural > 0 && !buildList.containsExpand())
	{
		log("dl v extrahatch req\n");
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}

	if(wantList.count(BWAPI::UnitTypes::Zerg_Spire) == 1 || wantList.count(BWAPI::TechTypes::Lurker_Aspect) == 1) // toegevoegd
	{
		log("dl v lair want\n");
		addWant(BWAPI::UnitTypes::Zerg_Extractor);
		addWant(BWAPI::UnitTypes::Zerg_Lair);
	}

	if( BWAPI::Broodwar->self()->minerals() > 300 && wantList.count(BWAPI::UnitTypes::Zerg_Hydralisk_Den) == 0 && wantList.count(BWAPI::UnitTypes::Zerg_Spire) == 0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >10) // toegevoegd
	{
		log("dl v expand 3\n");
		if(UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery)(isBeingConstructed).size() + UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery,Lair,Hive).size() <3)
		{
			log("dl v expand 3.1\n");
			buildExpand();
		}
	}

	//emergency
	// geskipt volgens Ben

	//Generieke rule:
	log("dl start generiek\n");
	log("wantlist size: ");
	log(this->intToString(wantList.size()).append("\n").c_str());
	log("buildlist size: ");
	log(this->intToString(buildList.size()).append("\n").c_str());
	for(std::list<BuildItem>::iterator it=wantList.buildlist.begin(); it!=wantList.buildlist.end(); it++)
	{
		if((*it).typenr == 1)
		{
			int wantAantal = wantList.count((*it).buildtype);
			int buildAantal = buildList.count((*it).buildtype);
			int hebAantal = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType,(*it).buildtype).size();
			log("for wantlist type==1\n");
			if(wantAantal > (buildAantal+hebAantal))
			{
				log("dl generiek buildenunit\n");
				log((*it).buildtype.getName().append("\n").c_str());
				addBuild((*it).buildtype);
				log("na de addbuild\n");
			}
			log("na de if\n");
		}
		if((*it).typenr == 2)
		{
			if(buildList.count((*it).researchtype) == 0 && !BWAPI::Broodwar->self()->hasResearched((*it).researchtype))
			{
				log("dl generiek research\n");
				addBuild((*it).researchtype);
			}
		}
		if((*it).typenr == 3)
		{
			if(buildList.count((*it).upgradetype) == 0 && BWAPI::Broodwar->self()->getUpgradeLevel((*it).upgradetype  < (*it).upgradetype.maxRepeats()))
			{
				log("dl generiek upgrade\n");
				addBuild((*it).upgradetype);
			}
		}
	}
}

double WantBuildManager::dist(int d)
{
	return d*32;
}

void WantBuildManager::logx(std::string func, int id, std::string msg)
{
	log(std::string(func).append(intToString(id)).append(std::string(msg)).c_str());
}

std::string WantBuildManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

bool WantBuildManager::canBeMade(BWAPI::UnitType unittype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	return ((!unittype.isBuilding() && allUnits(Larva).size() > 0) || unittype.isBuilding()) && unittype.mineralPrice() <= BWAPI::Broodwar->self()->minerals() && unittype.gasPrice() <= BWAPI::Broodwar->self()->gas() && unittype.supplyRequired() <= (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed());
}

bool WantBuildManager::bothCanBeMade(BWAPI::UnitType unittype, BWAPI::UnitType unittypetwo)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	return (unittype.isBuilding() && !unittypetwo.isBuilding() && allUnits(Larva).size() > 0 && (unittype.mineralPrice()+unittypetwo.mineralPrice()) <= BWAPI::Broodwar->self()->minerals() && (unittype.gasPrice()+unittypetwo.gasPrice()) <= BWAPI::Broodwar->self()->gas() && unittypetwo.supplyRequired() <= (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed()));
}

bool WantBuildManager::canBeMade(BWAPI::TechType techtype)
{
	return techtype.mineralPrice() <= BWAPI::Broodwar->self()->minerals() && techtype.gasPrice() <= BWAPI::Broodwar->self()->gas();
}

bool WantBuildManager::bothCanBeMade(BWAPI::UnitType unittype, BWAPI::TechType techtype)
{
	return (((unittype.mineralPrice()+techtype.mineralPrice()) <= BWAPI::Broodwar->self()->minerals()) && ((unittype.gasPrice()+techtype.gasPrice()) <= BWAPI::Broodwar->self()->gas()));
}

bool WantBuildManager::canBeMade(BWAPI::UpgradeType upgradetype)
{
	int lvl = BWAPI::Broodwar->self()->getUpgradeLevel(upgradetype);
	return (((upgradetype.mineralPriceBase()+(lvl*upgradetype.mineralPriceFactor())) <= BWAPI::Broodwar->self()->minerals()) && ((upgradetype.gasPriceBase()+(lvl*upgradetype.gasPriceFactor())) <= BWAPI::Broodwar->self()->gas()));
}

bool WantBuildManager::bothCanBeMade(BWAPI::UnitType unittype, BWAPI::UpgradeType researchtype)
{
	int lvl = BWAPI::Broodwar->self()->getUpgradeLevel(researchtype);
	return (((unittype.mineralPrice()+researchtype.mineralPriceBase()+(lvl*researchtype.mineralPriceFactor())) <= BWAPI::Broodwar->self()->minerals()) && ((unittype.gasPrice()+researchtype.gasPriceBase()+(lvl*researchtype.gasPriceFactor())) <= BWAPI::Broodwar->self()->gas()));
}

bool WantBuildManager::requirementsSatisfied(BWAPI::UnitType unittype)
{
	std::map<BWAPI::UnitType, int> reqs = unittype.requiredUnits();
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	for each(std::pair<BWAPI::UnitType, int> req in reqs)
	{
		if(allUnits(GetType, req.first).size() == 0)
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

bool WantBuildManager::requirementsSatisfied(BWAPI::TechType techtype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	reqsMet = allUnits(GetType, techtype.whatResearches()).size() > 0;
	return reqsMet;
}

bool WantBuildManager::requirementsSatisfied(BWAPI::UpgradeType upgradetype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	bool reqsMet = true;
	reqsMet = allUnits(GetType, upgradetype.whatUpgrades()).size() > 0;
	return reqsMet;
}

BWAPI::TilePosition WantBuildManager::placeFound(BWAPI::UnitType unittype)
{
	log("placeFound\n");
	return BuildingPlacer().getBuildLocationNear(this->hc->hatchery->getTilePosition(), unittype);
}

BWAPI::TilePosition WantBuildManager::placeFoundExtractor()
{
	std::set<BWAPI::Unit*> geysers;
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive);
	for each(BWAPI::Unit* hatchery in hatcheries)
	{
		//std::set<BWAPI::Unit*> extrageysers = BWTA::getNearestBaseLocation(hatchery->getTilePosition())->getGeysers();
		std::set<BWAPI::Unit*> extrageysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits())(Vespene_Geyser).inRadius(dist(9), hatchery->getPosition());
		geysers.insert(extrageysers.begin(), extrageysers.end());
	}
	if(geysers.size() > 0)
	{
		log("geysers gevonden\n");
		return (*geysers.begin())->getTilePosition();
	}
	else
	{
		log("geen geysers gevonden\n");
		return (*hatcheries.begin())->getTilePosition();
	}
}

void WantBuildManager::bouwStruc(BWAPI::TilePosition tilepos, BWAPI::UnitType unittype)
{
	log("bouwStruc\n");
	BWAPI::Unit* drone = pickBuildDrone(tilepos);
	if(drone != NULL)
	{
		log("drone != NULL\n");
		drone->build(tilepos, unittype);
		//this->bouwdrones.insert(drone);
	}
}

BWAPI::Unit* WantBuildManager::pickBuildDrone(BWAPI::TilePosition tilepos)
{
	log("pickBuildDrone\n");
	UnitGroup alldrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone);
	UnitGroup idledrones = alldrones(isIdle);
	if(idledrones.size() == 0)
	{
		log("idledrones.size() == 0\n");
		UnitGroup notcarrying = alldrones.not(isCarryingMinerals).not(isCarryingGas);
		if(notcarrying.size() == 0)
		{
			log("notcarrying.size() == 0\n");
			if(alldrones.size() == 0)
			{
				log("alldrones.size() == 0\n");
				return NULL;
			}
			else
			{
				log("alldrones.size() > 0\n");
				BWAPI::Unit* chosenOne = nearestUnit(BWAPI::Position(tilepos), alldrones);
				return chosenOne;
			}
		}
		else
		{
			log("notcarrying.size() > 0\n");
			BWAPI::Unit* chosenOne = nearestUnit(BWAPI::Position(tilepos), notcarrying);
			return chosenOne;
		}
	}
	else
	{
		log("idledrones.size() > 0\n");
		BWAPI::Unit* chosenOne = nearestUnit(BWAPI::Position(tilepos), idledrones);
		return chosenOne;
	}
}

BWAPI::Unit* WantBuildManager::nearestUnit(BWAPI::Position pos, UnitGroup ug)
{
	double besteAfstand = -1.00;
	BWAPI::Unit* besteUnit = NULL;

	for(std::set<BWAPI::Unit*>::iterator it = ug.begin(); it != ug.end(); it++)
	{
		if(besteAfstand == -1)
		{
			besteAfstand = (*it)->getDistance(pos);
			besteUnit = (*it);
		}
		else
		{
			if((*it)->getDistance(pos) < besteAfstand)
			{
				besteAfstand = (*it)->getDistance(pos);
				besteUnit = (*it);
			}
		}
	}

	return besteUnit;
}

void WantBuildManager::doExpand()
{
	log("doExpand\n");
	BWAPI::TilePosition tilepos = this->hc->hatchery->getTilePosition();
	std::set<BWTA::BaseLocation*> baselocations = BWTA::getBaseLocations();
	std::map<BWAPI::Unit*, EnemyUnitData> enemies = this->eudm->getData();
	int distance = -1;
	
	std::set<BWTA::BaseLocation*> kandidaten;
	for each(BWTA::BaseLocation* baselocation in baselocations)
	{
		bool isConnected = false;
		bool enemyOpLocatie = false;
		bool friendlyOpLocatie = false;

		// enemy op deze lokatie?
		for each(std::pair<BWAPI::Unit*, EnemyUnitData> paar in enemies)
		{
			if(BWAPI::TilePosition(paar.second.position) == baselocation->getTilePosition())
			{
				enemyOpLocatie = true;
			}
		}

		// friendly op deze lokatie?
		if(BWAPI::Broodwar->unitsOnTile(baselocation->getTilePosition().x(), baselocation->getTilePosition().y()).size() > 0)
		{
			friendlyOpLocatie = true;
		}

		isConnected = BWTA::isConnected(this->hc->hatchery->getTilePosition(), baselocation->getTilePosition());

		if(isConnected && !enemyOpLocatie && !friendlyOpLocatie)
		{
			kandidaten.insert(baselocation);
		}
	}
	BWTA::BaseLocation* dichtstbijzijnde = NULL;
	for each(BWTA::BaseLocation* kandidaat in kandidaten)
	{
		if(distance == -1)
		{
			dichtstbijzijnde = kandidaat;
			distance = this->hc->hatchery->getDistance(kandidaat->getPosition());
		}
		else if(this->hc->hatchery->getDistance(kandidaat->getPosition()) < distance)
		{
			dichtstbijzijnde = kandidaat;
			distance = this->hc->hatchery->getDistance(kandidaat->getPosition());
		}
	}
	if(dichtstbijzijnde != NULL)
	{
		tilepos = dichtstbijzijnde->getTilePosition();
	}
	// else { nog steeds hatchery locatie, zie begin }
	
	BWAPI::Unit* drone = pickBuildDrone(tilepos);
	if(drone != NULL && tilepos != this->hc->hatchery->getTilePosition() && BWTA::isConnected(drone->getTilePosition(), tilepos))
	{
		log("drone != NULL\n");

		UnitGroup bezig = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery)(isBeingConstructed);
		// bezig kan size > 1 hebben
		bool underconstruction = false;
		for each(BWAPI::Unit* lolgebouw in bezig)
		{
			if(lolgebouw->getRemainingBuildTime() / lolgebouw->getType().buildTime() < 0.9)
			{
				underconstruction = true;
			}
		}
		//(ugdateralheengaat == 0 && pickeddronegaaternietheen) || (ugdateralheengaat == 1 && (pickeddronehasmove order to position || pickeddrone has build order at position))
		UnitGroup gaateralheen = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(GetTargetPosition, BWAPI::Position(tilepos));
		bool droneonderweg = drone->getOrder() == BWAPI::Orders::PlaceBuilding || drone->getTargetPosition() == BWAPI::Position(tilepos);
		if(!underconstruction && (
			(gaateralheen.size() == 0 && !droneonderweg)
			|| (gaateralheen.size() == 1 && droneonderweg)
			))
		{
			if(!BWAPI::Broodwar->isVisible(tilepos))
			{
				log("expand plek niet visible, move\n");
				drone->move(BWAPI::Position(tilepos));
			}
			else
			{
				log("expand plek visible, build\n");
				drone->build(tilepos, BWAPI::UnitTypes::Zerg_Hatchery);
			}
			BWAPI::Broodwar->drawTextMap(drone->getPosition().x(), drone->getPosition().y(), std::string("\nexpand").c_str());
		}
		if(underconstruction)
		{
			log("wordt al een hatchery gebouwd, stop\n");
			drone->stop();
		}
		//this->bouwdrones.insert(drone);
	}
}

bool WantBuildManager::isBeingHandled(BuildItem b)
{
	UnitGroup drones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	for each(BWAPI::Unit* drone in drones)
	{
		//if(drone->getOrder() == BWAPI::Orders::PlaceBuilding)
		if(drone->getBuildType() == b.buildtype)
		{
			return true;
		}
	}
	return false;
}

void WantBuildManager::logBuildList(BuildList bl)
{
	log("BUILDLIST: ");
	for each(BuildItem bi in bl.buildlist)
	{
		log("[");
		log(intToString(bi.typenr).append(" ").c_str());
		if(bi.typenr == 1)
		{
			log(bi.buildtype.getName().c_str());
		}
		if(bi.typenr == 2)
		{
			log(bi.researchtype.getName().c_str());
		}
		if(bi.typenr == 3)
		{
			log(bi.upgradetype.getName().c_str());
		}
		if(bi.typenr == 4)
		{
			log("expand");
		}
		log("]");
	}
	log("\n");
}

void WantBuildManager::logc(const char* msg)
{
	if(false)
	{
		log(msg);
	}
}
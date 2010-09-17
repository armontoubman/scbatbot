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

WantBuildManager::WantBuildManager(EnemyUnitDataManager* e, HighCommand* h, MicroManager* m)
{
	this->eudm = e;
	this->hc = h;
	this->mm = m;

	this->stap = 1;
}

int WantBuildManager::nrOfEnemy(BWAPI::UnitType unittype)
{
	return this->eudm->count(unittype);
}

int WantBuildManager::nrOfOwn(BWAPI::UnitType unittype)
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, unittype)(isCompleted).size();
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
		logc(std::string("addBuild ").append(unittype.getName()).append("\n").c_str());
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
	logc(std::string("dronesreq mins: ").append(intToString(minerals.size())).append("\n").c_str());
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	logc(std::string("dronesreq hatch: ").append(intToString(hatcheries.size())).append("\n").c_str());

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
	logc(std::string("minsperbasis: ").append(intToString(amount)).append("\n").c_str());
	// extractors
	amount += (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted).size()*3);
	logc(std::string("plusextractors: ").append(intToString(amount)).append("\n").c_str());
	// aantal workers al aan de slag
	amount -= UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isWorker).not(isMorphing).not(isConstructing).size();
	logc(std::string("minus workers: ").append(intToString(amount)).append("\n").c_str());
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
	logc("WantBuildManager::update()\n");
	logc("WANTLIST: ");
	logBuildList(this->wantList);
	logc("BUILDLIST: ");
	logBuildList(this->buildList);
	logc("stap: ");
	logc(intToString(this->stap).append("\n").c_str());
	// Actual building of items
	if(buildList.size() > 0)
	{
		BuildItem b = buildList.top();
		if (buildList.count(BWAPI::UnitTypes::Zerg_Overlord)>0 && b.typenr != 4 && !b.buildtype.isBuilding() && b.buildtype != BWAPI::UnitTypes::Zerg_Overlord)
		{
			logc("remove voor overlord\n");
			buildList.removeTop();
			b = buildList.top();
			return;
		}
		// check of gebouw al aant bouwe is
		if((b.typenr == 1 && b.buildtype.isBuilding()) || b.typenr == 4)
		{
			UnitGroup bezig = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, b.buildtype)(isBeingConstructed);
			bezig = bezig + (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, b.buildtype).not(isCompleted));
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
				logc("bezig met: ");
				logc(b.buildtype.getName().append("\n").c_str());
				if(b.typenr == 1 && b.buildtype == lolgebouw->getType() && lolgebouw->getRemainingBuildTime() / lolgebouw->getType().buildTime() >= 0.9)
				{
					logc(b.buildtype.getName().append(" ").c_str());
					logc("started and removed from top\n");
					buildList.removeTop();
					b = buildList.top();
					return;
				}
				if(b.typenr == 1 && b.buildtype == lolgebouw->getType() && b.buildtype == BWAPI::UnitTypes::Zerg_Extractor)
				{
					logc(b.buildtype.getName().append(" ").c_str());
					logc("started and removed from top\n");
					buildList.removeTop();
					b = buildList.top();
					return;
				}
				if(b.typenr == 4 && lolgebouw->getType() == BWAPI::UnitTypes::Zerg_Hatchery && lolgebouw->getRemainingBuildTime() / lolgebouw->getType().buildTime() >= 0.9)
					// maybe herkent ie niet zerg_hatchery als iets dat bouwt...
				{
					logc("expand started and removed from top\n");
					buildList.removeTop();
					b = buildList.top();
					return;
				}
			}
		}
		//einde remove build als aant bouwe is

		logc(std::string(intToString(b.typenr)).append("=b.typenr\n").c_str());
		if(b.typenr == 1)
		{
			if(!requirementsSatisfied(b.buildtype) || (BWAPI::Broodwar->self()->gas() < b.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
			{
				logc("can't make\n\t");
				logc(b.buildtype.getName().append("\n").c_str());
				logc("remove\n");
				buildList.removeTop();
				logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
				return;
			}
			else
			{
				if(buildList.count(BWAPI::UnitTypes::Zerg_Drone)>3 && b.buildtype!=BWAPI::UnitTypes::Zerg_Drone && b.buildtype!=BWAPI::UnitTypes::Zerg_Overlord && !isBeingHandled(b)) // conditie die checkt of b niet al in maak is
				{
					logc(b.buildtype.getName().append(" ").c_str());
					logc("drones voorrang\n");
					buildList.removeTop();
					return;
				}
				else
				{
					if(canBeMade(b.buildtype))
					{
						if(!b.buildtype.isBuilding())
						{
							if(b.buildtype == BWAPI::UnitTypes::Zerg_Lurker)
							{
								(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hydralisk).begin())->morph(b.buildtype);
								buildList.removeTop();
								return;
							}
							else
							{
								if (b.buildtype == BWAPI::UnitTypes::Zerg_Overlord && (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) < (BWAPI::Broodwar->self()->supplyTotal()+(countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)))
								{
									buildList.removeTop();
									return;
								}
								logc("can make\n\t");
								logc(b.buildtype.getName().append("\n").c_str());
								(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Larva).begin())->morph(b.buildtype);
								buildList.removeTop();
								logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
								return;
							}
						}
						else
						{
							logc("can make\n\t");
							logc(b.buildtype.getName().append("\n").c_str());
							if(b.buildtype == BWAPI::UnitTypes::Zerg_Lair)
							{
								if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
								{
									logc("probeer hatchery naar lair te morphen...\n");
									this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Lair);
									logc("gelukt.\n");
									buildList.removeTop();
									return;
								}
								else
								{
									UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
									if(hatcheries.size() == 0 || nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)>0)
									{
										logc("kan geen lair maken, geen hatcheries\n");
										buildList.removeTop();
										return;
									}
									else
									{
										logc("probeer hatchery naar lair te morphen...\n");
										(*hatcheries.begin())->morph(BWAPI::UnitTypes::Zerg_Lair);
										logc("gelukt.\n");
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
										logc("probeer lair naar hive te morphen...\n");
										this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Hive);
										logc("gelukt.\n");
										buildList.removeTop();
										return;
									}
									else
									{
										UnitGroup lairs = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Lair);
										if(lairs.size() == 0 || nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>0)
										{
											logc("kan geen hive maken, geen lairs\n");
											buildList.removeTop();
											return;
										}
										else
										{
											logc("probeer lair naar hive te morphen...\n");
											(*lairs.begin())->morph(BWAPI::UnitTypes::Zerg_Hive);
											logc("gelukt.\n");
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
										logc("bouwen maar\n");
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
											logc("bouwen gelukt\n");
										}
										else
										{
											logc("bouwen mislukt, geen locatie\n");
											buildList.removeTop();
											return;
										}
									}
									else
									{
										//buildList.removeTop(); // crash
										logc("wordt al gemaakt\n");
									}
									logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
									return;
								}
							}
						}
					}
				}
			}
		}
		logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
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
					this->eigenResearch(b.researchtype);
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
					this->eigenUpgrade(b.upgradetype);
					buildList.removeTop();
					return;
				}
			}
		}
		logc("buildlist bf 4\n");
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
			logc("buildlist bs start\n");
			BuildItem v = buildList.getSecond();
			if (v.typenr == 1 && b.typenr == 1 && !v.buildtype.isBuilding() && b.buildtype.isBuilding())
			{
				if(!requirementsSatisfied(v.buildtype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					logc("can't make second\n\t");
					logc(v.buildtype.getName().append("\n").c_str());
					logc("remove second\n");
					buildList.removeSecond();
					logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
					return;
				}
				else
				{
					if (bothCanBeMade(b.buildtype, v.buildtype))
					{
						if(b.buildtype == BWAPI::UnitTypes::Zerg_Lurker)
						{
							logc("can make\n\t");
							logc(v.buildtype.getName().append("\n").c_str());
							(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hydralisk).begin())->morph(v.buildtype);
							buildList.removeSecond();
							return;
						}
						else
						{
							if (v.buildtype == BWAPI::UnitTypes::Zerg_Overlord && (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) < (BWAPI::Broodwar->self()->supplyTotal()+(countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)))
							{
								buildList.removeTop();
								return;
							}
							logc("can make\n\t");
							logc(v.buildtype.getName().append("\n").c_str());
							(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Larva).begin())->morph(v.buildtype);
							buildList.removeSecond();
							logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
							return;
						}
					}
				}
			}
			logc("buildlist bs pre 1, lair\n");
			if (v.typenr == 1 && (v.buildtype==BWAPI::UnitTypes::Zerg_Lair || v.buildtype == BWAPI::UnitTypes::Zerg_Hive) && (b.buildtype.isBuilding() || b.typenr == 4))
			{
				if (v.buildtype==BWAPI::UnitTypes::Zerg_Lair)
				{
					if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
					{
						logc("probeer hatchery naar lair te morphen... sec\n");
						this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Lair);
						logc("gelukt. sec\n");
						buildList.removeSecond();
						return;
					}
					else
					{
						UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
						if(hatcheries.size() == 0 || nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)>0)
						{
							logc("kan geen lair maken, geen hatcheries sec\n");
							buildList.removeSecond();
							return;
						}
						else
						{
							logc("probeer hatchery naar lair te morphen... sec\n");
							(*hatcheries.begin())->morph(BWAPI::UnitTypes::Zerg_Lair);
							logc("gelukt. sec\n");
							buildList.removeSecond();
							return;
						}
					}
				}
				else
				{
					if(v.buildtype == BWAPI::UnitTypes::Zerg_Hive)
					{
						if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Lair)
						{
							logc("probeer lair naar hive te morphen... second\n");
							this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Hive);
							logc("gelukt second.\n");
							buildList.removeSecond();
							return;
						}
						else
						{
							UnitGroup lairs = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Lair);
							if(lairs.size() == 0 || nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>0)
							{
								logc("kan geen hive maken, geen lairs second\n");
								buildList.removeSecond();
								return;
							}
							else
							{
								logc("probeer lair naar hive te morphen... second\n");
								(*lairs.begin())->morph(BWAPI::UnitTypes::Zerg_Hive);
								logc("gelukt. sec\n");
								buildList.removeSecond();
								return;
							}
						}
					}
				}
			}
			logc("buildlist bs pre 4, 1\n");
			if (v.typenr == 1 && b.typenr == 4)
			{
				if(!requirementsSatisfied(v.buildtype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					logc("can't make second\n\t");
					logc(v.buildtype.getName().append("\n").c_str());
					logc("remove second\n");
					buildList.removeSecond();
					logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
					return;
				}
				else
				{
					if (bothCanBeMadeExpand(v.buildtype))
					{
						if(v.buildtype == BWAPI::UnitTypes::Zerg_Lurker)
						{
							logc("can make\n\t");
							logc(v.buildtype.getName().append("\n").c_str());
							(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hydralisk).begin())->morph(v.buildtype);
							buildList.removeSecond();
							return;
						}
						else
						{
							logc("can make\n\t");
							logc(v.buildtype.getName().append("\n").c_str());
							(*UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Larva).begin())->morph(v.buildtype);
							buildList.removeSecond();
							logc(std::string(intToString(buildList.buildlist.size()).append(" ").append(intToString(wantList.buildlist.size())).append("\n")).c_str());
							return;
						}
					}
				}
			}
			if (v.typenr == 2 && b.typenr == 1)
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
						logc("research second\n");
						this->eigenResearch(v.researchtype);
						buildList.removeSecond();
						return;
					} 
				}
			}
			logc("buildlist bs pre 4, 2\n");
			if (v.typenr == 2 && b.typenr == 4)
			{
				if(!requirementsSatisfied(v.researchtype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					buildList.removeSecond();
					return;
				}
				else
				{
					if(bothCanBeMadeExpand(v.researchtype) && !BWAPI::Broodwar->self()->hasResearched(v.researchtype))
					{
						logc("research second 2\n");
						this->eigenResearch(v.researchtype);
						buildList.removeSecond();
						return;
					} 
				}
			}
			if(v.typenr == 3 && b.typenr == 1)
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
						logc("upgrade second\n");
						this->eigenUpgrade(v.upgradetype);
						buildList.removeSecond();
						return;
					}
				}
			}
			if(v.typenr == 3 && b.typenr == 4)
			{
				if(!requirementsSatisfied(v.upgradetype) || (BWAPI::Broodwar->self()->gas() < v.gasPrice() && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).size() == 0))
				{
					buildList.removeSecond();
					return;
				}
				else
				{
					if(bothCanBeMadeExpand(v.upgradetype) && (v.upgradetype.maxRepeats() > BWAPI::Broodwar->self()->getUpgradeLevel(v.upgradetype)))
					{
						logc("upgrade second 2\n");
						this->eigenUpgrade(v.upgradetype);
						buildList.removeSecond();
						return;
					}
				}
			}
			if (v.typenr == 4 && (buildList.count(BWAPI::UnitTypes::Zerg_Spawning_Pool) || buildList.count(BWAPI::UnitTypes::Zerg_Spire) || buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk_Den)))
			{
				logc("geen expand als er nog tech inbuildlist staat\n");
				buildList.removeSecond();
				return;
			}
			if (((b.typenr == 1 && b.buildtype.isBuilding()) || (b.typenr == 4)) && ((v.typenr == 4) || (v.typenr == 1 && b.buildtype.isBuilding())))
			{
				logc("beide buildings\n");
				buildList.removeSecond();
				return;
			}
		}
	}
	else
	{
		logc("buildlist is leeg\n");
	}
	logc("buildlist update done\n");
}

void WantBuildManager::doLists()
{
	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();

	logc("doLists\n\n");

	if(enemyRace == BWAPI::Races::Protoss)
	{
		
		if(	stap == 1 && (buildListIsEmpty()) && (wantListIsEmpty())) 
		{
			logc("doLists stap 1 lege lijsten\n");
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
			logc("doLists stap 2\n");
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>8)
			{
				logc("dl p 1-nexpand\n");
				buildExpand();
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>1) && buildList.count(BWAPI::UnitTypes::Zerg_Lair)==0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)==0) // *** nog geen bezig!
			{
				logc("dl p 1 spire\n");
				addBuild(BWAPI::UnitTypes::Zerg_Lair); // *** pak een hatchery en morph het naar lair
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 4) && photonCannonNearBase() ) // *7* goedgekeurd, check buildexpand ligt in methode zelf
			{
				buildExpand();
				logc("dl p 1-1\n");
			}
			/*if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Nexus) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Forge) >= 1)	&&	(nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) < 9)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 10)	&&	(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2)	) // nakijken
			{
				buildExpand();
				logc("dl p 1-2\n");
			}*/
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Stargate) > 0)
			{
				logc("dl p 1-3\n");
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				stap = 3;
			}
			/*if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 5) || ((nrOfEnemy(BWAPI::UnitTypes::Protoss_Gateway) > 2)  && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0)))
			{
				logc("dl p 1-4\n");
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
				stap = 3;
			}*/
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2) ) // *7*
			{
				logc("dl p 1-5\n");
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk, 10);
			}
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle)>0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)==2)) // deze was gecomment
			{
				logc("dl p 1-6\n");
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			/*if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0)	&& (nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 3)) // *7*fix
			{
				logc("dl p 1-7\n");
				addBuild(BWAPI::UnitTypes::Zerg_Zergling, 20);
				if(	(((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))+wantList.count(BWAPI::UnitTypes::Zerg_Hatchery)+wantList.count(BWAPI::UnitTypes::Zerg_Lair)+wantList.count(BWAPI::UnitTypes::Zerg_Hive)) < 2) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den)==0)) // *7*
				{
					logc("dl p 1-7-1\n");
					addWant(BWAPI::UnitTypes::Zerg_Hatchery,1);
				}
				else
				{
					logc("dl p 1-7-2\n");
					addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
					addWant(BWAPI::UnitTypes::Zerg_Extractor);
				}
			}
			if(  (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>1))
			{
				logc("dl p 1-8\n");
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}*/
			if(  (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Spire)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
			{
				stap = 3;
			}
		}
		if( stap == 3)
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
			/*if( wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) && !wantListContains(BWAPI::UnitTypes::Zerg_Spire))
			{
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			if( wantListContains(BWAPI::UnitTypes::Zerg_Spire) && !wantListContains(BWAPI::UnitTypes::Zerg_Hydralisk_Den))
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			}*/
			/*if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0) || ((nrOfEnemy(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) > 0) && (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 7)) || (nrOfEnemy(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) > 0))
			{
				addWant(BWAPI::UnitTypes::Zerg_Spire);
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk, 11);
			}
			if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Photon_Cannon) > 9 )
			{
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			}*/
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot)+nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon)) > 13)
			{
				addWant(BWAPI::TechTypes::Lurker_Aspect);
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk)+nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) > 15) || (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 30) && dronesRequiredAll() < 3)
			{
				stap = 4;
			}
		}
		if( stap == 4)
		{
			addWant(BWAPI::UnitTypes::Zerg_Queens_Nest);
			addWant(BWAPI::UnitTypes::Zerg_Hive);
			addWant(BWAPI::UnitTypes::Zerg_Defiler_Mound);
			addWant(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
		}
		// reinforcements -->
		logc("dl p r start\n");
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Zealot) > 6)
		{
			logc("dl p r 1\n");
			if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0) || this->eudm->getUG()(isFlyer).size() >4)
			{
				logc("dl p r 1-1\n");
				if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 9 || nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk)>0)
				{
					logc("dl p r 1-2\n");
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
							logc("dl p r 1-2-2-m\n");
							addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
						}
						else
						{
							logc("dl p r 1-2-2-h\n");
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
						logc("dl p r 1-1-2\n");
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
				}
			} 
			else
			{
				logc("dl p r 2\n");
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
						logc("dl p r 3\n");
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
		else if( nrOfEnemyMilitaryUnits() < 6)
		{
			logc("dl p r 3\n");
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
				logc("dl p r 3-2\n");
				if( (buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)+nrOfOwn(BWAPI::UnitTypes::Zerg_Mutalisk) < 11) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0))
				{
					logc("dl p r 3-2-1\n");
					addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
				}
				else
				{
					if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
					{
						logc("dl p r 1-1-2\n");
						addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
					}
					else
					{
						if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
						{
							logc("dl p r 3-2-2\n");
							addBuild(BWAPI::UnitTypes::Zerg_Zergling);
						}
					}
				}
			}
		}
		logc("dl p r elseifuit\n");
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Dragoon) > 4)
		{
			logc("dl p r 4\n");
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
				logc("dl p r 4-2\n");
				if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4)
				{
					logc("dl p r 4-2-1\n");
					addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
				}
				else
				{
					logc("dl p r 4-2-2\n");
					if (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<3)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
		if( nrOfEnemy(BWAPI::UnitTypes::Protoss_Shuttle) > 0)
		{
			logc("dl p r 5\n");
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) > 0) && buildList.count(BWAPI::UnitTypes::Zerg_Mutalisk)<2)
			{
				addBuild(BWAPI::UnitTypes::Zerg_Mutalisk);
			}
			else if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) && (buildList.count(BWAPI::UnitTypes::Zerg_Hydralisk)<4))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Hydralisk);
			}
		}
		logc("dl p r 5 -> 6 check\n");
		if( (nrOfEnemy(BWAPI::UnitTypes::Protoss_Observatory) == 0) && (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect))) // *7*fix
		{
			logc("dl p r 6\n");
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
			logc("dl p r 7\n");
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
		logc("dl p klaar\n");
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
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<2))
			{
				logc("dl p 1-nexpand\n");
				buildExpand();
			}
			if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)>1) && buildList.count(BWAPI::UnitTypes::Zerg_Lair)==0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)==0) // *** nog geen bezig!
			{
				logc("dl p 1 spire\n");
				addBuild(BWAPI::UnitTypes::Zerg_Lair); // *** pak een hatchery en morph het naar lair
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}
			/*if(nrOfEnemy(BWAPI::UnitTypes::Terran_Barracks) > 1)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Spire);
			}*/
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Vulture) > 0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Spire)==0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
			}
			if(nrOfEnemy(BWAPI::UnitTypes::Terran_Starport) > 0)
			{
				addWant(BWAPI::UnitTypes::Zerg_Extractor);
				addWant(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
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
			if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive))<3)
			{
				buildExpand();
			}
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
			addWant(BWAPI::UnitTypes::Zerg_Queens_Nest);
			addWant(BWAPI::UnitTypes::Zerg_Hive);
			addWant(BWAPI::UnitTypes::Zerg_Defiler_Mound);
			addWant(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
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
			addWant(BWAPI::UnitTypes::Zerg_Lair);
			addWant(BWAPI::UnitTypes::Zerg_Spire);
			stap = 2;
		}
		if( stap == 2)
		{
			/*if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) < 8) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Zergling) > 7) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) < 2))
			{
				addBuild(BWAPI::UnitTypes::Zerg_Sunken_Colony);
			}*/
			if( (((nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 0 || (nrOfEnemy(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Lair)+nrOfEnemy(BWAPI::UnitTypes::Zerg_Hive)) == 2) && (nrOfEnemy(BWAPI::UnitTypes::Zerg_Mutalisk) == 0)) && buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2 && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)<10 )
			{
				addBuild(BWAPI::UnitTypes::Zerg_Zergling);
			}
			//if( (nrOfEnemy(BWAPI::UnitTypes::Zerg_Mutalisk) > 3) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Spire) == 0) )
			//{
			//	addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
			//	addWant(BWAPI::UnitTypes::Zerg_Spore_Colony);
			//}
			/*if( nrOfEnemy(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0)
			{
				buildExpand();
			}*/
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
					if ( ( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >10) || ((nrOfOwn(BWAPI::UnitTypes::Zerg_Sunken_Colony) > 0) && (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >4))))
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
				}
				else
				{
					if ((nrOfOwn(BWAPI::UnitTypes::Zerg_Drone) >= ( 2*nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor) + 4*(nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)))) && (buildList.count(BWAPI::UnitTypes::Zerg_Zergling)<2) && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)<17)
					{
						addBuild(BWAPI::UnitTypes::Zerg_Zergling);
					}
				}
			}
		}
	}

	// upgrades

	int zerglingtotaal = this->buildList.count(BWAPI::UnitTypes::Zerg_Zergling);
	zerglingtotaal += UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Zergling).size();
	zerglingtotaal += countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Zergling);
	logc("dl g zlingtotal\n");
	if (zerglingtotaal > 10)
	{
		logc("dl g zlingtotal satis\n");
		addWant(BWAPI::UpgradeTypes::Metabolic_Boost);
	}
	
	if( ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) + nrOfOwn(BWAPI::UnitTypes::Zerg_Lurker)) * 2) >47 && buildList.count(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)<1 )
	{
		logc("dl upgr missile\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Missile_Attacks); // research ranged ground dmg
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 11 && buildList.count(BWAPI::UpgradeTypes::Muscular_Augments)<1 )
	{
		logc("dl upgr muscu\n");
		addWant(BWAPI::UpgradeTypes::Muscular_Augments); // research hydralisk speed
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}
	logc("dl upgr muscu->spines\n");
	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 19 && buildList.count(BWAPI::UpgradeTypes::Grooved_Spines)<1 )
	{
		logc("dl upgr spines\n");
		addWant(BWAPI::UpgradeTypes::Grooved_Spines); // research range
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( (nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling)+nrOfOwn(BWAPI::UnitTypes::Zerg_Ultralisk)) > 39 && buildList.count(BWAPI::UpgradeTypes::Zerg_Melee_Attacks)<1 )
	{
		logc("dl upgr melee\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Melee_Attacks); // research melee ground damage
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}

	if( BWAPI::Broodwar->self()->supplyUsed() > 180 && buildList.count(BWAPI::UpgradeTypes::Zerg_Carapace)<1 ) // >90 supply required (dubbel vanwege werking API)
	{
		logc("dl upgr cara\n");
		addWant(BWAPI::UpgradeTypes::Zerg_Carapace); // upgrade ground armor
		addWant(BWAPI::UnitTypes::Zerg_Evolution_Chamber);
	}
	logc("dl upgr cara->burrow\n");

	if( BWAPI::Broodwar->self()->supplyUsed() > 90 && (nrOfOwn(BWAPI::UnitTypes::Zerg_Hydralisk) > 15 || nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) > 20) && buildList.count(BWAPI::TechTypes::Burrowing)<1 )
	{
		logc("dl upgr burrow\n");
		addWant(BWAPI::TechTypes::Burrowing);
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Hive) > 0 )
	{
		logc("dl hive tech\n");
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
	logc("dl v start\n");
	/*logc(std::string("overlord eggs: ").append(intToString(countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord))).append("\n").c_str());
	if( BWAPI::Broodwar->self()->supplyUsed() >= (BWAPI::Broodwar->self()->supplyTotal()+(buildList.count(BWAPI::UnitTypes::Zerg_Overlord)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord))) && (buildList.top().typenr == 1 && buildList.top().buildtype != BWAPI::UnitTypes::Zerg_Overlord) && (BWAPI::Broodwar->self()->supplyTotal() < 400)) // voorkomt dat het overlords spamt als het al op max bevindt
	{
		logc("dl v buildtopoverlord1\n");
		addBuildTop(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}*/
	/*if (wantList.count(BWAPI::UnitTypes::Zerg_Lair)>0 && BWAPI::Broodwar->self()->minerals() >= 150 && BWAPI::Broodwar->self()->gas() >= 100 && nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)==0) // && no lair is morphing
	{
		logc("directlairtech\n");
		if(this->hc->hatchery->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
		{
			logc("probeer hatchery naar lair te morphen...\n");
			this->hc->hatchery->morph(BWAPI::UnitTypes::Zerg_Lair);
			logc("gelukt.\n");
			buildList.removeTop();
			return;
		}
		else
		{
			UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
			if(hatcheries.size() == 0)
			{
				logc("kan geen lair maken, geen hatcheries\n");
				buildList.removeTop();
			}
			else
			{
				logc("probeer hatchery naar lair te morphen...\n");
				(*hatcheries.begin())->morph(BWAPI::UnitTypes::Zerg_Lair);
				logc("gelukt.\n");
				buildList.removeTop();
				return;
			}
		}
	}*/
	if( (BWAPI::Broodwar->self()->supplyUsed() + buildList.supplyRequiredForTopThree()) >= (BWAPI::Broodwar->self()->supplyTotal()+(buildList.count(BWAPI::UnitTypes::Zerg_Overlord)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)*16)) && buildList.count(BWAPI::UnitTypes::Zerg_Overlord)<2 && (BWAPI::Broodwar->self()->supplyTotal() < 400) ) //next 3 items in buildqueue increases the supply required > supplyprovided
	{
		logc("dl v buildtopoverlord2\n");
		addBuild(BWAPI::UnitTypes::Zerg_Overlord); // (dus wordt als eerste gedaan)
	}
	
	if( dronesRequiredAll() > (buildList.count(BWAPI::UnitTypes::Zerg_Drone)+countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Drone)) && buildList.count(BWAPI::UnitTypes::Zerg_Drone)<6 ) // not sufficient drones
	{
		logc("drone build\n");	
		addBuild(BWAPI::UnitTypes::Zerg_Drone);
	}
	
	UnitGroup hatcheries = getHatcheriesWithMinerals()(isCompleted);
	UnitGroup geysers = UnitGroup::getUnitGroup(BWAPI::Broodwar->getGeysers());
	UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor);
	logc("dl v extrac\n");
	for(std::set<BWAPI::Unit*>::iterator hit=hatcheries.begin(); hit!=hatcheries.end(); hit++)
	{
		if(geysers.inRadius(dist(10.00), (*hit)->getPosition()).size() > 0 && extractors.inRadius(dist(10.00), (*hit)->getPosition()).size() == 0 && dronesRequiredAll()<3 && nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>9 && nrOfOwn(BWAPI::UnitTypes::Zerg_Extractor)+buildList.count(BWAPI::UnitTypes::Zerg_Extractor) < hatcheries.size())
		{
			logc("dl v addbuild extrac\n");
			addBuild(BWAPI::UnitTypes::Zerg_Extractor);
		}
	}
	logc("dl v startcheck hatcheryexpands\n");
	BWTA::BaseLocation* natural = getNaturalExpansion(); // eigenlijk check je dit bij het bouwen zelf, dan kun je het altijd nog deleten.
	int enemiesNearNatural = 0;
	if(natural != NULL)
	{
		enemiesNearNatural = this->eudm->getEnemyUnitsInRadius(dist(10.00), natural->getPosition()).size();
	}

	if( buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss || BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
		&& (nrOfEnemyBases()*2 >= nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) && nrOfEnemyBases()>1 && !buildList.containsExpand() && enemiesNearNatural == 0 && dronesRequiredAll()<3)
	{
		logc("dl v expand 1\n");
		if (nrOfOwn(BWAPI::UnitTypes::Zerg_Drone)>15)
		{
			addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
		}
		else
		{
			buildExpand();
		}
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 2 && BWAPI::Broodwar->self()->minerals() >= 500 && enemiesNearNatural == 0 && !buildList.containsExpand() && dronesRequiredAll()<3)
	{
		logc("dl v expand 2\n");
		buildExpand();
	}

	if( nrOfOwn(BWAPI::UnitTypes::Zerg_Larva) == 0 && buildList.countUnits() > 2 && BWAPI::Broodwar->self()->minerals() >= 500 && enemiesNearNatural > 0 && !buildList.containsExpand() && buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && dronesRequiredAll()<3 )
	{
		logc("dl v extrahatch req\n");
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	if(!buildList.containsExpand() && nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)<5 && buildList.count(BWAPI::UnitTypes::Zerg_Hatchery)==0 && BWAPI::Broodwar->self()->minerals() >= 500 && BWAPI::Broodwar->self()->gas()<200 )
	{
		logc("dl v extrahatch req 2\n");
		addBuild(BWAPI::UnitTypes::Zerg_Hatchery);
	}
	if(wantList.count(BWAPI::UnitTypes::Zerg_Spire) == 1 || wantList.count(BWAPI::TechTypes::Lurker_Aspect) == 1) // toegevoegd
	{
		logc("dl v lair want\n");
		addWant(BWAPI::UnitTypes::Zerg_Extractor);
		addWant(BWAPI::UnitTypes::Zerg_Lair);
	}

	/*if( BWAPI::Broodwar->self()->minerals() > 400 && wantList.count(BWAPI::UnitTypes::Zerg_Hydralisk_Den) == 0 && wantList.count(BWAPI::UnitTypes::Zerg_Spire) == 0 && nrOfOwn(BWAPI::UnitTypes::Zerg_Zergling) >10) // toegevoegd
	{
		logc("dl v expand 3\n");
		if(UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery)(isBeingConstructed).size() + UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery,Lair,Hive).size() <3  && dronesRequiredAll()<3)
		{
			logc("dl v expand 3.1\n");
			buildExpand();
		}
	}*/

	if((((BWAPI::Broodwar->self()->minerals()>400 || BWAPI::Broodwar->self()->gas()>400) && ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) == 2)) || (nrOfOwnMilitaryUnits() <7) || ((nrOfOwn(BWAPI::UnitTypes::Zerg_Hatchery)+nrOfOwn(BWAPI::UnitTypes::Zerg_Lair)+nrOfOwn(BWAPI::UnitTypes::Zerg_Hive)) > 2) || (nrOfOwnMilitaryUnits() < nrOfEnemyMilitaryUnits())))
	{
		logc("dl v maakunits 8\n");
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
			logc("dl v b 8b-2\n");
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

	//emergency
	// geskipt volgens Ben

	//Generieke rule:
	logc("dl start generiek\n");
	logc("wantlist size: ");
	logc(this->intToString(wantList.size()).append("\n").c_str());
	logc("buildlist size: ");
	logc(this->intToString(buildList.size()).append("\n").c_str());
	for(std::list<BuildItem>::iterator it=wantList.buildlist.begin(); it!=wantList.buildlist.end(); it++)
	{
		if((*it).typenr == 1)
		{
			int wantAantal = wantList.count((*it).buildtype);
			int buildAantal = buildList.count((*it).buildtype);
			int hebAantal = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType,(*it).buildtype).size();
			if ((*it).buildtype == BWAPI::UnitTypes::Zerg_Lair)
			{
				hebAantal = hebAantal + UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hive).size();
			}
			logc("for wantlist type==1\n");
			if(wantAantal > (buildAantal+hebAantal))
			{
				if ((*it).buildtype == BWAPI::UnitTypes::Zerg_Lair || (*it).buildtype == BWAPI::UnitTypes::Zerg_Hive)
				{
					if (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Lair,Hive,Hatchery)(isMorphing).size()==0)
					{
						logc("lair of hive tech\n");
						logc((*it).buildtype.getName().append("\n").c_str());
						addBuild((*it).buildtype);
					}
				}
				else
				{
					logc("dl generiek buildenunit\n");
					logc((*it).buildtype.getName().append("\n").c_str());
					addBuild((*it).buildtype);
					logc("na de addbuild\n");
				}
			}
			logc("na de if\n");
		}
		if((*it).typenr == 2)
		{
			logc("for wantlist type==2\n");
			if(buildList.count((*it).researchtype) == 0 && !BWAPI::Broodwar->self()->hasResearched((*it).researchtype))
			{
				logc("dl generiek research\n");
				addBuild((*it).researchtype);
			}
		}
		if((*it).typenr == 3)
		{
			logc("for wantlist type==3\n");
			if(buildList.count((*it).upgradetype) == 0 && BWAPI::Broodwar->self()->getUpgradeLevel((*it).upgradetype  < (*it).upgradetype.maxRepeats()))
			{
				logc("dl generiek upgrade\n");
				addBuild((*it).upgradetype);
			}
		}
		if((*it).typenr == 4)
		{
			logc("for wantlist type==4\n");
		}
	}
}

double WantBuildManager::dist(int d)
{
	return d*32;
}

void WantBuildManager::logx(std::string func, int id, std::string msg)
{
	logc(std::string(func).append(intToString(id)).append(std::string(msg)).c_str());
}

std::string WantBuildManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

bool WantBuildManager::canBeMade(BWAPI::UnitType unittype)
{
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	return (((!unittype.isBuilding() && allUnits(Larva).size() > 0) || unittype.isBuilding()) && unittype.mineralPrice() <= BWAPI::Broodwar->self()->minerals() && unittype.gasPrice() <= BWAPI::Broodwar->self()->gas() && unittype.supplyRequired() <= ((BWAPI::Broodwar->self()->supplyTotal()+unittype.supplyProvided()) - BWAPI::Broodwar->self()->supplyUsed()));
}

bool WantBuildManager::bothCanBeMade(BWAPI::UnitType unittype, BWAPI::UnitType unittypetwo)
{
	// enkel 1st is building, 2e is unit
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	return (unittype.isBuilding() && !unittypetwo.isBuilding() && allUnits(Larva).size() > 0 && (unittype.mineralPrice()+unittypetwo.mineralPrice()) <= BWAPI::Broodwar->self()->minerals() && (unittype.gasPrice()+unittypetwo.gasPrice()) <= BWAPI::Broodwar->self()->gas() && unittypetwo.supplyRequired() <= ((BWAPI::Broodwar->self()->supplyTotal()+unittypetwo.supplyProvided()) - BWAPI::Broodwar->self()->supplyUsed()));
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
	//int lvl = BWAPI::Broodwar->self()->getUpgradeLevel(upgradetype);
	//return (((upgradetype.mineralPriceBase()+(lvl*upgradetype.mineralPriceFactor())) <= BWAPI::Broodwar->self()->minerals()) && ((upgradetype.gasPriceBase()+(lvl*upgradetype.gasPriceFactor())) <= BWAPI::Broodwar->self()->gas()));
	return ((upgradetype.mineralPriceBase() <= BWAPI::Broodwar->self()->minerals()) && upgradetype.gasPriceBase() <= BWAPI::Broodwar->self()->gas());
}

bool WantBuildManager::bothCanBeMade(BWAPI::UnitType unittype, BWAPI::UpgradeType researchtype)
{
	//int lvl = BWAPI::Broodwar->self()->getUpgradeLevel(researchtype);
	//return (((unittype.mineralPrice()+researchtype.mineralPriceBase()+(lvl*researchtype.mineralPriceFactor())) <= BWAPI::Broodwar->self()->minerals()) && ((unittype.gasPrice()+researchtype.gasPriceBase()+(lvl*researchtype.gasPriceFactor())) <= BWAPI::Broodwar->self()->gas()));
	return (((unittype.mineralPrice()+researchtype.mineralPriceBase())) <= BWAPI::Broodwar->self()->minerals()) && ((unittype.gasPrice()+researchtype.gasPriceBase())) <= BWAPI::Broodwar->self()->gas();
}

bool WantBuildManager::bothCanBeMadeExpand(BWAPI::UnitType unittype)
{
	return (!unittype.isBuilding() &&((unittype.mineralPrice()+300) <= BWAPI::Broodwar->self()->minerals()) && ((unittype.gasPrice())<= BWAPI::Broodwar->self()->gas()) && unittype.supplyRequired() <= ((BWAPI::Broodwar->self()->supplyTotal()+unittype.supplyProvided()) - BWAPI::Broodwar->self()->supplyUsed()));
}

bool WantBuildManager::bothCanBeMadeExpand(BWAPI::TechType techtype)
{
	return (((300+techtype.mineralPrice()) <= BWAPI::Broodwar->self()->minerals()) && ((techtype.gasPrice()) <= BWAPI::Broodwar->self()->gas()));
}

bool WantBuildManager::bothCanBeMadeExpand(BWAPI::UpgradeType researchtype)
{
	//int lvl = BWAPI::Broodwar->self()->getUpgradeLevel(researchtype);
	//return (((300+researchtype.mineralPriceBase()+(lvl*researchtype.mineralPriceFactor())) <= BWAPI::Broodwar->self()->minerals()) && ((researchtype.gasPriceBase()+(lvl*researchtype.gasPriceFactor())) <= BWAPI::Broodwar->self()->gas()));
	return ((300+researchtype.mineralPriceBase()) <= BWAPI::Broodwar->self()->minerals()) && ((researchtype.gasPriceBase()) <= BWAPI::Broodwar->self()->gas());
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
	logc("placeFound\n");
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
		logc("geysers gevonden\n");
		return (*geysers.begin())->getTilePosition();
	}
	else
	{
		logc("geen geysers gevonden\n");
		return (*hatcheries.begin())->getTilePosition();
	}
}

void WantBuildManager::bouwStruc(BWAPI::TilePosition tilepos, BWAPI::UnitType unittype)
{
	logc("bouwStruc\n");
	BWAPI::Unit* drone = pickBuildDrone(tilepos);
	if(drone != NULL)
	{
		logc("drone != NULL\n");
		drone->build(tilepos, unittype);
		//this->bouwdrones.insert(drone);
	}
}

BWAPI::Unit* WantBuildManager::pickBuildDrone(BWAPI::TilePosition tilepos)
{
	logc("pickBuildDrone\n");
	UnitGroup alldrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone);
	UnitGroup idledrones = alldrones(isIdle);
	if(idledrones.size() == 0)
	{
		logc("idledrones.size() == 0\n");
		UnitGroup notcarrying = alldrones.not(isCarryingMinerals).not(isCarryingGas);
		if(notcarrying.size() == 0)
		{
			logc("notcarrying.size() == 0\n");
			if(alldrones.size() == 0)
			{
				logc("alldrones.size() == 0\n");
				return NULL;
			}
			else
			{
				logc("alldrones.size() > 0\n");
				BWAPI::Unit* chosenOne = nearestUnit(BWAPI::Position(tilepos), alldrones);
				return chosenOne;
			}
		}
		else
		{
			logc("notcarrying.size() > 0\n");
			BWAPI::Unit* chosenOne = nearestUnit(BWAPI::Position(tilepos), notcarrying);
			return chosenOne;
		}
	}
	else
	{
		logc("idledrones.size() > 0\n");
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
	logc("doExpand\n");
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
		logc("drone != NULL\n");

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
				logc("expand plek niet visible, move\n");
				drone->move(BWAPI::Position(tilepos));
			}
			else
			{
				logc("expand plek visible, build\n");
				drone->build(tilepos, BWAPI::UnitTypes::Zerg_Hatchery);
			}
			BWAPI::Broodwar->drawTextMap(drone->getPosition().x(), drone->getPosition().y(), std::string("\nexpand").c_str());
		}
		if(underconstruction)
		{
			logc("wordt al een hatchery gebouwd, stop\n");
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
	logc(bouwlistString(bl).c_str());
}

std::string WantBuildManager::bouwlistString(BuildList bl)
{
	std::string result = "";
	for each(BuildItem bi in bl.buildlist)
	{
		result.append("[");
		result.append(intToString(bi.typenr).append(" ").c_str());
		if(bi.typenr == 1)
		{
			result.append(bi.buildtype.getName().c_str());
		}
		if(bi.typenr == 2)
		{
			result.append(bi.researchtype.getName().c_str());
		}
		if(bi.typenr == 3)
		{
			result.append(bi.upgradetype.getName().c_str());
		}
		if(bi.typenr == 4)
		{
			result.append("expand");
		}
		result.append("]");
	}
	result.append("\n");
	return result;
}

void WantBuildManager::logc(const char* msg)
{
	if(true)
	{
		log(msg);
	}
}

void WantBuildManager::eigenResearch(BWAPI::TechType techtype)
{
	log("eigenResearch ");
	log(techtype.getName().c_str());
	log("\n");
	BWAPI::UnitType soortunit = techtype.whatResearches();
	UnitGroup dezesoort = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, soortunit);
	if(dezesoort.size() == 0)
	{
		return;
	}
	bool wordtalgedaan = false;
	for each(BWAPI::Unit* unit in dezesoort)
	{
		if(unit->isResearching() && unit->getTech() == techtype)
		{
			wordtalgedaan = true;
		}
	}
	if(!wordtalgedaan)
	{
		for each(BWAPI::Unit* unit in dezesoort)
		{
			if(!unit->isResearching())
			{
				unit->research(techtype);
				return;
			}
		}
	}
}

void WantBuildManager::eigenUpgrade(BWAPI::UpgradeType upgradetype)
{
	log("eigenResearch ");
	log(upgradetype.getName().c_str());
	log("\n");
	BWAPI::UnitType soortunit = upgradetype.whatUpgrades();
	UnitGroup dezesoort = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, soortunit);
	if(dezesoort.size() == 0)
	{
		return;
	}
	bool wordtalgedaan = false;
	for each(BWAPI::Unit* unit in dezesoort)
	{
		if(unit->isUpgrading() && unit->getUpgrade() == upgradetype)
		{
			wordtalgedaan = true;
		}
	}
	if(!wordtalgedaan)
	{
		for each(BWAPI::Unit* unit in dezesoort)
		{
			if(!unit->isUpgrading())
			{
				unit->upgrade(upgradetype);
				return;
			}
		}
	}
}
#pragma once
#include "MicroManager.h"
#include <BWAPI.h>
#include <set>
#include <map>
#include <UnitGroup.h>
#include <BuildOrderManager.h>
#include "EnemyUnitDataManager.h"
#include "EnemyUnitData.h"
#include "TaskManager.h"
#include "Task.h"
#include "HighCommand.h"
#include "EigenUnitDataManager.h"
#include "Util.h"
#include <sstream>
#include "WantBuildManager.h"

MicroManager::MicroManager()
{
}

MicroManager::MicroManager(BuildOrderManager* b, EnemyUnitDataManager* e, TaskManager* t, HighCommand* h, EigenUnitDataManager* ei, WantBuildManager* w)
{
	this->bom = b;
	this->eudm = e;
	this->tm = t;
	this->hc = h;
	this->eiudm = ei;
	this->wbm = w;
}

BWAPI::Position MicroManager::moveAway(BWAPI::Unit* unit, double radius)
{
	// huidige positie van de unit die gaat moven
	BWAPI::Position current = unit->getPosition();
	// aanliggende posities van de huidige posities
	std::set<BWAPI::Position> mogelijkePosities = sanitizePositions(getAdjacentPositions(current));
	// geen mogelijkheden? return huidige positie
	if(mogelijkePosities.empty()) { return unit->getPosition(); }

	// lijstje position->int om de scores bij te houden en daaruit de beste positie te kiezen
	std::map<BWAPI::Position, int> telling;
	// alle enemies in de gekozen radius
	UnitGroup enemies = this->eudm->getUG().inRadius(radius, unit->getPosition());
	// enemies bekijken en scores toewijzen aan de mogelijke posities
	// voor alle mogelijke posities
	for(std::set<BWAPI::Position>::iterator it = mogelijkePosities.begin(); it != mogelijkePosities.end(); it++)
	{
		// aantal enemies waarvoor iets telt (of unit geraakt kan worden op deze positie)
		int aantal = 0;

		// voor alle enemies in de radius
		for(std::set<BWAPI::Unit*>::iterator iten = enemies.begin(); iten != enemies.end(); iten++)
		{
			int range = 0;
			// als de huidige unit een flyer is
			if(unit->getType().isFlyer()) {
				// range is de range van het airweapon van de enemy
				range = (*iten)->getType().airWeapon().maxRange();
			} else {
				// anders van de groundweapon
				range = (*iten)->getType().groundWeapon().maxRange();
			}
			// als deze positie tot een grotere afstand leidt dan de huidige afstand
			if((*iten)->getDistance(*it) > (*iten)->getDistance(current))
			{
				// score van deze positie omhoog
				aantal++;
			}
			// als de afstand van de enemy tot de positie die nu bekeken wordt buiten de range van zijn wapen valt
			if((*iten)->getDistance(*it) > range)
			{
				// bonus
				aantal++;
			}
		}

		telling.insert(std::pair<BWAPI::Position, int>(BWAPI::Position(it->x(), it->y()), aantal));
	}

	// positie met hoogste aantal eruit vissen

	int besteAantal = -1;
	BWAPI::Position bestePositie = unit->getPosition();

	for(std::map<BWAPI::Position, int>::iterator itlol = telling.begin(); itlol != telling.end(); itlol++)
	{
		if(itlol->second >= besteAantal)
		{
			besteAantal = itlol->second;
			bestePositie = itlol->first;
		}
	}
	return bestePositie;
}

BWAPI::Position MicroManager::moveAway(BWAPI::Unit* unit)
{
	return moveAway(unit, dist(13.00));
}

void MicroManager::moveAway(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it=units.begin(); it!=units.end(); it++)
	{
		(*it)->rightClick(moveAway((*it)));
	}
}

std::set<BWAPI::Position> MicroManager::getAdjacentPositions(BWAPI::Position p)
{
	std::set<BWAPI::Position> result;

	int factor = dist(10); // was 1

	result.insert(BWAPI::Position(p.x()-factor, p.y()+factor));
	result.insert(BWAPI::Position(p.x(), p.y()+factor));
	result.insert(BWAPI::Position(p.x()+factor, p.y()+factor));

	result.insert(BWAPI::Position(p.x()-factor, p.y()));
	result.insert(BWAPI::Position(p.x()+factor, p.y()));

	result.insert(BWAPI::Position(p.x()-factor, p.y()-factor));
	result.insert(BWAPI::Position(p.x(), p.y()-1));
	result.insert(BWAPI::Position(p.x()+factor, p.y()-factor));

	return result;
}

std::set<BWAPI::Position> MicroManager::sanitizePositions(std::set<BWAPI::Position> ps)
{
	std::set<BWAPI::Position> result;

	for(std::set<BWAPI::Position>::iterator it = ps.begin(); it != ps.end(); it++)
	{
		if(
			BWAPI::Broodwar->isWalkable(it->x(), it->y()) &&
			BWAPI::Broodwar->unitsOnTile(it->x(), it->y()).empty() == true
			)
		{
			result.insert(BWAPI::Position(it->x(), it->y()));
		}
	}

	return result;
}

bool MicroManager::overlordSupplyProvidedSoon()
{
	return
		BWAPI::Broodwar->self()->supplyUsed() < BWAPI::Broodwar->self()->supplyTotal() - BWAPI::UnitTypes::Zerg_Overlord.supplyProvided() ||
		this->bom->getPlannedCount(BWAPI::UnitTypes::Zerg_Overlord) > 0;
}

bool MicroManager::enemyInRange(BWAPI::Position p, double radius, int type)
{
	return enemiesInRange(p, radius, type).size() > 0;
}

bool MicroManager::enemyInRange(BWAPI::Position p)
{
	return enemyInRange(p, dist(13.00), 0);
}

UnitGroup MicroManager::enemiesInRange(BWAPI::Position p, double radius, int type) // 0 beide, 1 ground, 2 flyer
{
	UnitGroup enemies = this->eudm->getUG().inRadius(radius, p);
	if(type == 1) {
		enemies = enemies - enemies(isFlyer);
	}
	if(type == 2) {
		enemies = enemies(isFlyer);
	}
	return enemies;
}

bool MicroManager::containsDetector(UnitGroup ug)
{
	if(
		ug(Observer).size() > 0 ||
		ug(Photon_Cannon).size() > 0 ||

		ug(Missile_Turret).size() > 0 ||
		ug(Science_Vessel).size() > 0 ||

		ug(Overlord).size() > 0 ||
		ug(Spore_Colony).size() > 0
		)
	{
		return true;
	}
	else {
		return false;
	}
}

int MicroManager::compareArmySize(UnitGroup x, UnitGroup y)
{
	return x.size() - y.size();
}

BWAPI::Unit* MicroManager::nearestUnit(BWAPI::Position pos, UnitGroup ug)
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

UnitGroup MicroManager::enemiesInSeekRange(BWAPI::Position p, double radius, int type) // 0 beide, 1 ground, 2 flyer
{
	UnitGroup ug = UnitGroup();

	std::map<BWAPI::Unit*, EnemyUnitData> databank = this->eudm->getData();

	for(std::map<BWAPI::Unit*, EnemyUnitData>::iterator it = databank.begin(); it != databank.end(); it++)
	{
		if(p.getDistance(it->second.position) < radius)
		{
			if(type == 0)
			{
				ug.insert(it->first);
			}
			if(type == 1 && !it->first->getType().isFlyer())
			{
				ug.insert(it->first);
			}
			if(type == 2 && it->first->getType().isFlyer())
			{
				ug.insert(it->first);
			}
		}
	}

	return ug;
}

bool MicroManager::alliesCanAttack(BWAPI::Position p, UnitGroup enemies)
{
	UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(9.00), p);
	if(allies.size() == 0) return false;
	bool alliesGroundWeapons = false;
	bool alliesAirWeapons = false;
	bool enemiesAir = false;
	bool enemiesGround = false;

	if(enemies(isFlyer).size() > 0) enemiesAir = true;
	if((enemies - enemies(isFlyer)).size() > 0) enemiesGround = true;

	for(std::set<BWAPI::Unit*>::iterator it = allies.begin(); it != allies.end(); it++)
	{
		if((*it)->getType().groundWeapon().targetsGround())
		{
			alliesGroundWeapons = true;
		}
		if((*it)->getType().airWeapon().targetsAir())
		{
			alliesAirWeapons = true;
		}
	}

	if(enemiesAir && alliesAirWeapons) return true;
	if(enemiesGround && alliesGroundWeapons) return true;
	return false;
}

/*
BWAPI::Unit* MicroManager::harvest(BWAPI::Unit* unit) // returnt een unit terug waarop rechtermuisgeklikt mag worden
{
	UnitGroup mineralDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isGatheringMinerals);
	UnitGroup gasDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(MoveToGas, WaitForGas, HarvestGas, ReturnGas); // has Order gather gas moet er nog bij of juist ipv
	UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted);
	
	if (gasDrones.size() < extractors.size()*3)
	{
		if (mineralDrones.size()<5)
		{
			if (mineralDrones.size()<3 || gasDrones.size() >= extractors.size()*2)
			{
				return mineWhere(unit);
			}
			else
			{
				return gasWhere(unit);
			}
		}
		else
		{
			return gasWhere(unit);
		}
	}
	else
	{
		if (gasDrones.size() > extractors.size()*3) // teveel gas enzo
		{
			if (unit->isCarryingGas())
			{
				return this->hc->getNearestHatchery(unit->getPosition());
			}
			else
			{
				return mineWhere(unit);
			}
			// return de gas terug enzo, rechtermuisklik op nearest hatchery
			// mogelijke probleem hierbij is dat ze allemaal teruggaan, als carryinggas == returngas/gatheringGas
		}
	}
	return mineWhere(unit);
}
*/

void MicroManager::mineWhere(BWAPI::Unit* unit)
{
	if (unit->isGatheringMinerals())
	{
		// unit->getTarget(); // vraag is, beweegt het dan telkens juist weer terug naar de patch of blijft het lekker gatheren? als het telkens terug gaat, is het mogelijk return null te doen? en anders wordt het complexer
	}
	else
	{
		UnitGroup minerals = getUnusedMineralsNearHatcheries(); // pak alle unused minerals die in de nabijheid van een hatchery bevinden
		if(minerals.empty()) // voor als het vol is enzo
		{
			unit->rightClick(this->hc->getNearestHatchery(unit->getPosition()));
		}
		unit->rightClick(nearestUnitInGroup(unit, minerals));
	}
}

void MicroManager::gasWhere(BWAPI::Unit* unit)
{
	if (unit->isGatheringGas())
	{
		//return unit->getTarget();
	}
	else
	{
		UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted);
		UnitGroup result = extractors;
		for(std::set<BWAPI::Unit*>::iterator it=extractors.begin(); it!=extractors.end(); it++)
		{
			if (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone).inRadius(dist(6), (*it)->getPosition()).size()<3)
			{
				result.erase(*it); // ug met een unit ehh
			}
		}
		if (result.empty())
		{
			unit->rightClick(this->hc->getNearestHatchery(unit->getPosition()));
		}
		unit->rightClick(nearestUnitInGroup(unit, result));
	}
}

UnitGroup MicroManager::getHatcheriesWithMinerals()
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
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

UnitGroup MicroManager::getUnusedMineralsNearHatcheries()
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)
		{
			if((*it)->getDistance(*mit) <= dist(8.00) && !(*mit)->isBeingGathered())
			{
				result.insert(*mit);
			}
		}
	}
	return result;
}

UnitGroup* MicroManager::inRadiusUnitGroup(double radius, UnitGroup* ug)
{
	UnitGroup* result= new UnitGroup();
	for(std::set<BWAPI::Unit*>::iterator it=ug->begin(); it!=ug->end(); it++)
	{
		UnitGroup newunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(radius, (*it)->getPosition());
		result->insert(newunits.begin(), newunits.end());
	}
	return result;
}

UnitGroup* MicroManager::inRadiusUnitGroupUnitType(double radius, UnitGroup* ug, BWAPI::UnitType ut)
{
	UnitGroup* result= new UnitGroup();
	for(std::set<BWAPI::Unit*>::iterator it=ug->begin(); it!=ug->end(); it++)
	{
		UnitGroup newunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(radius, (*it)->getPosition())(GetType, ut);
		result->insert(newunits.begin(), newunits.end());
	}
	return result;
}


void MicroManager::doMicro(std::set<UnitGroup*> listUG)
{
	log("\n\ndoMicro\n\n");

	UnitGroup allSelfUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	UnitGroup allEnemyUnits = this->eudm->getUG();

	for(std::set<UnitGroup*>::iterator it=listUG.begin(); it!=listUG.end(); it++)
	{
		/* SCOURGE */
		if((**it)(Scourge).size() > 0)
		{
			BWAPI::Unit* eerste = *((*it)->begin());
			UnitGroup airenemies = enemiesInRange(eerste->getPosition(), dist(7.00), 2);
			UnitGroup allenemies = enemiesInRange(eerste->getPosition(), dist(7.00), 0);
			if(eerste->isUnderStorm() || canAttackAir(allenemies) && airenemies.size() == 0)
			{
				(*it)->rightClick(this->hc->getNearestHatchery(eerste->getPosition())->getPosition());
			}
			else
			{
				if(airenemies.size() > 0)
				{
					if(canAttackAir(allenemies) || airenemies.size() > 2*(*it)->size()) // nakijken
					{
						moveAway(**it);
					}
					else
					{
						(*it)->attackUnit(*airenemies.begin());
					}
				}
				else {
					if(eerste->getDistance(this->tm->findTaskWithUnit(eerste).position) < dist(9.00))
					{
						for(std::set<BWAPI::Unit*>::iterator scourgeit=(*it)->begin(); scourgeit!=(*it)->end(); scourgeit++)
						{
							(*scourgeit)->rightClick(splitup(*scourgeit)); // kijk dit ff na aub
						}
					}
					else
					{
						(*it)->attackMove(this->tm->findTaskWithUnit(eerste).position);
					}
				}
			}
		}
		/* EINDE SCOURGE */

		 /* MUTALISK */
		else if((**it)(Mutalisk).size() > 0)
		{
			BWAPI::Unit* eerste = *((*it)->begin());
			if(eerste->isUnderStorm() || (**it)(Mutalisk).size() < 6) 
			{
				moveToNearestBase(**it);
			}
			else
			{
				if(eerste->getGroundWeaponCooldown() != 0)
				{
					if(canAttackAir(enemiesInRange(eerste->getPosition(), dist(7.00), 0)))
					{
						BWAPI::Position pos = moveAway(eerste);
						(*it)->rightClick(pos);
					}
					else
					{
						if(eerste->getDistance(this->tm->findTaskWithUnit(eerste).position) < dist(9.00))
						{
							moveToNearestBase(**it);
						}
						else
						{
							(*it)->rightClick(this->tm->findTaskWithUnit(eerste).position);	
						}
					}
				}
				else
				{
					if(eerste->getDistance(this->tm->findTaskWithUnit(eerste).position) < dist(9.00))
					{
						BWAPI::Unit* nearestAirEnemy = nearestEnemyThatCanAttackAir(eerste);
						double distanceAE = eerste->getPosition().getDistance(nearestAirEnemy->getPosition());
						BWAPI::Unit* nearestNonBuilding = nearestNonBuildingEnemy(eerste);
						double distanceNB = eerste->getPosition().getDistance(nearestNonBuilding->getPosition());
						if(distanceNB < dist(9.00))
						{
							if(distanceAE < distanceNB)
							{
								(*it)->attackUnit(nearestAirEnemy);
							}
							else
							{
								(*it)->attackUnit(nearestNonBuilding);
							}
						}
						else
						{
							(*it)->attackUnit(nearestUnit(eerste->getPosition(), enemiesInRange(eerste->getPosition(), dist(9.00), 0)));
						}
					}
					else
					{
						(*it)->attackMove(this->tm->findTaskWithUnit(eerste).position);
					}
				}
			}
		}
		/* EINDE MUTALISK */

		/* ZERGLING */
		else if((**it)(Zergling).size() > 0)
		{
			BWAPI::Unit* eerste = *((*it)->begin());
			if((**it).size() == 1)
			{
				logx("doMicro zergling ", eerste, " group.size()=1\n");
				if (amountCanAttackGround(enemiesInRange(eerste->getPosition(),dist(8),0))>1)
				{
					logx("doMicro zergling ", eerste, " enemy in de buurt, moveToNearestBase\n");
					moveToNearestBase(**it);
				}
				else
				{
					Task currentTask = this->tm->findTaskWithUnit(eerste);
					if (currentTask.type == 1) // <-- deze
					{
						logx("doMicro zergling ", eerste, " task.type!=1\n");
						if (eerste->getPosition().getDistance(currentTask.position) < dist(5))
						{
							logx("doMicro zergling ", eerste, " dichtbij task, loop random\n");
							int x = eerste->getPosition().x();
							int y = eerste->getPosition().y();
							int factor = dist(10);
							int newx = x + (((rand() % 30)-15)*factor);
							int newy = y + (((rand() % 30)-15)*factor);
							eerste->rightClick(BWAPI::Position(newx, newy));
						}
						else
						{
							logx("doMicro zergling ", eerste, " move naar task\n");
							(*it)->rightClick(this->tm->findTaskWithUnit(eerste).position);
						}
					}
					else
					{
						logx("doMicro zergling ", eerste, " task.type=1 moveToNearestBase\n"); // ???????????????????
						moveToNearestBase(**it);
					}
				}
			}
			else
			{
				logx("doMicro zergling ", eerste, " groep is groter dan 1\n");
				UnitGroup enemies = enemiesInRange(eerste->getPosition(), dist(10.00), 1);
				UnitGroup allies = allSelfUnits.inRadius(dist(10.00), eerste->getPosition());
				if((**it).size() * 0.8 < enemies.size() && allies.size() < 3)
				{
					logx("doMicro zergling ", eerste, " outnumbered\n");
					moveAway(**it);
				}
				else
				{
					BWAPI::Unit* swarm = nearestSwarm(eerste);
					if(swarm != NULL && swarm->getPosition().getDistance(eerste->getPosition()) < dist(9.00))
					{
						logx("doMicro zergling ", eerste, " swarm in de buurt\n");
						if(!isUnderDarkSwarm(eerste))
						{
							logx("doMicro zergling ", eerste, " naar swarm\n");
							(**it).rightClick(swarm->getPosition());
						}
						else
						{
							logx("doMicro zergling ", eerste, " onder swarm, attack enemy\n"); // wat als geen enemy? null
							(**it).attackUnit(nearestUnit(eerste->getPosition(), enemiesInRange(eerste->getPosition(), dist(10.00), 1)));
						}
					}
					else
					{
						logx("doMicro zergling ", eerste, " geen swarm\n");
						if((**it).size() > 7)
						{
							logx("doMicro zergling ", eerste, " group.size()>7\n");
							if(enemyInRange(eerste->getPosition(), dist(7.00), 1))
							{
								logx("doMicro zergling ", eerste, " enemy in de buurt\n");
								BWAPI::Unit* nearest = nearestUnit(eerste->getPosition(), enemiesInRange(eerste->getPosition(), dist(10.00), 1).not(isBuilding).not(isWorker));
								if(true) // if between nearest ground enemy && UG geen wall bevindt (aka kan er naar toe bewegen enzo), een check voor wall vraagt om watvoor gebouw het is en of ernaast een cliff/gebouw bevindt. Wat ook kan is dat we het gewoon altijd wegbewege en juist mutalisks/hydra korte mette mee laten maken.
								{
									if(canAttackGround(enemiesInRange(eerste->getPosition(), dist(6.00), 2)))
									{
										logx("doMicro zergling ", eerste, " air enemy in de buurt, moveaway\n");
										moveAway(**it);
									}
									else
									{
										logx("doMicro zergling ", eerste, " geen air enemy\n");
										BWAPI::Unit* enemy = nearestUnit(eerste->getPosition(), enemiesInRange(eerste->getPosition(), dist(10.00), 1));
										UnitGroup enemyUG = enemiesInRange(enemy->getPosition(), dist(6.00), 1);
										BWAPI::Position center = enemyUG.getCenter();
										BWAPI::Position eigencenter = (**it).getCenter();
										if(enemyUG.size() > 0 && center.getDistance(eigencenter) < dist(3.00))
										{
											logx("doMicro zergling ", eerste, " attackMove center\n");
											(*it)->attackMove(center);
										}
										else
										{
											logx("doMicro zergling ", eerste, " naar enemy position\n");
											(*it)->rightClick(enemy->getPosition());
										}
									}
								}
								else
								{
									moveAway(**it);
								}
							}
							else
							{
								logx("doMicro zergling ", eerste, " geen enemy in range\n");
								UnitGroup buildings = enemiesInRange(eerste->getPosition(), dist(7.00), 1)(isBuilding);
								UnitGroup workers = enemiesInRange(eerste->getPosition(), dist(10.00), 1)(isWorker);
								if(buildings.size() > 0)
								{
									logx("doMicro zergling ", eerste, " wel buildings\n");
									if(workers.size() > 0)
									{
										logx("doMicro zergling ", eerste, " wel workers\n");
										(*it)->attackUnit(*(workers).begin());
									}
									else
									{
										logx("doMicro zergling ", eerste, " geen workers\n");
										(*it)->attackUnit(*(buildings).begin());
									}
								}
								else
								{
									logx("doMicro zergling ", eerste, " geen buildings\n");
									if(eerste->getDistance(this->tm->findTaskWithUnit(eerste).position) < dist(6.00))
									{
										logx("doMicro zergling ", eerste, " task in de buurt, splitup\n");
										for(std::set<BWAPI::Unit*>::iterator zergit=(**it).begin(); zergit!=(**it).end(); zergit++)
										{
											(*zergit)->rightClick(splitup(*zergit));
										}
									}
									else
									{
										logx("doMicro zergling ", eerste, " move naar task\n");
										(*it)->rightClick(this->tm->findTaskWithUnit(eerste).position);
									}
								}
							}
						}
						else
						{
							logx("doMicro zergling ", eerste, " group.size()<=7\n");
							UnitGroup enemies = enemiesInRange(eerste->getPosition(), dist(7.00), 1);
							if(enemies.size() > 0)
							{
								logx("doMicro zergling ", eerste, " enemies in de buurt\n");
								(*it)->attackMove(enemies.getCenter());
							}
							else
							{
								logx("doMicro zergling ", eerste, " geen enemies in de buurt\n");
								UnitGroup buildings = enemiesInRange(eerste->getPosition(), dist(7.00), 1)(isBuilding);
								UnitGroup workers = enemiesInRange(eerste->getPosition(), dist(10.00), 1)(isWorker);
								if(buildings.size() > 0)
								{
									logx("doMicro zergling ", eerste, " wel buildings\n");
									if(workers.size() > 0)
									{
										logx("doMicro zergling ", eerste, " wel workers\n");
										(*it)->attackUnit(*(workers).begin());
									}
									else
									{
										logx("doMicro zergling ", eerste, " geen workers\n");
										(*it)->attackUnit(*(buildings).begin());
									}
								}
								else
								{
									logx("doMicro zergling ", eerste, " geen buildings\n");
									if(eerste->getDistance(this->tm->findTaskWithUnit(eerste).position) < dist(6.00))
									{
										logx("doMicro zergling ", eerste, " in de buurt van task, splitup\n");
										for(std::set<BWAPI::Unit*>::iterator zergit=(**it).begin(); zergit!=(**it).end(); zergit++)
										{
											(*zergit)->rightClick(splitup(*zergit));
										}
									}
									else
									{
										logx("doMicro zergling ", eerste, " move naar task\n");
										(*it)->rightClick(this->tm->findTaskWithUnit(eerste).position);
									}
								}
							}
						}
					}
				}
			}
		}
		/* EINDE ZERGLING */

		else
		{
			for(std::set<BWAPI::Unit*>::iterator unitit=(*it)->begin(); unitit!=(*it)->end(); unitit++)
			{

				Task currentTask = this->tm->findTaskWithUnit(*unitit);

				/* ULTRALISK */
				if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Ultralisk)
				{
					if((*unitit)->isIrradiated())
					{
						if(allSelfUnits.inRadius(dist(3.00), (*unitit)->getPosition()).size() > 0)
						{
							BWAPI::Position base = BWAPI::Broodwar->enemy()->getStartLocation();
							(*unitit)->attackMove(base);
						}
						else
						{
							UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(7.00), 0);
							if(enemies.size() > 0)
							{
								(*unitit)->attackUnit(*enemies.begin());
							}
							else
							{
								BWAPI::Position base = BWAPI::Broodwar->enemy()->getStartLocation();
								(*unitit)->attackMove(base);
							}
						}
					}
					else
					{
						if((*unitit)->isUnderStorm())
						{
							(*unitit)->attackMove(moveAway(*unitit));
						}
						else
						{
							UnitGroup swarms = UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits())(Dark_Swarm).inRadius(dist(9.00), (*unitit)->getPosition());
							if(swarms.size() > 0)
							{
								if(isUnderDarkSwarm(*unitit))
								{
									// doe niks Game AI lost zelf op
								}
								else
								{
									(*unitit)->rightClick(nearestSwarm(*unitit)->getPosition());
								}
							}
							else
							{
								if((*unitit)->getDistance(currentTask.position) < dist(8.00))
								{
									(*unitit)->attackUnit(*enemiesInRange((*unitit)->getPosition(), dist(8.00), 0).begin());
								}
								else
								{
									(*unitit)->attackMove(currentTask.position);
								}
							}
						}
					}
				}
				/* EINDE ULTRALISK */

				/* LURKER */
				else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Lurker)
				{
					if(!(*unitit)->isBurrowed())
					{
						if((*unitit)->isUnderStorm())
						{
							moveToNearestBase(*unitit);
						}
						else
						{
							if((*unitit)->getPosition().getDistance(currentTask.position) > dist(6.00) && enemiesInRange((*unitit)->getPosition(), dist(13.00), 0).size() == 0)
							{
								(*unitit)->rightClick(currentTask.position);
							}
							else
							{
								(*unitit)->burrow();
							}
						}
					}
					else
					{
						if((*unitit)->isUnderStorm())
						{
							(*unitit)->unburrow();
						}
						else
						{
							UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(6.00), 0);
							if(enemiesInRange((*unitit)->getPosition(), dist(13.00), 0).size() > 0)
							{
								if(!this->eiudm->unitIsSeen(*unitit))
								{
									if(enemies.size() > 3)
									{
										if(enemies(Marine).size() > 0 || enemies(isWorker).size() > 0 || enemies(Zealot).size() > 0 || enemies(Medic).size() > 0 || enemies(Zergling).size() > 0)
										{
											if(nearestUnit((*unitit)->getPosition(), enemies)->getPosition().getDistance((*unitit)->getPosition()) < dist(3.00))
											{
												(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), enemies));
											}
											else
											{
												// hold lurker
												std::set<BWAPI::Unit*> holdset;
												holdset.insert(*unitit);
												UnitGroup olords = allSelfUnits(Overlord)(isIdle);
												if(olords.size() > 0)
												{
													holdset.insert(*olords.begin());
													UnitGroup holdgroup = UnitGroup::getUnitGroup(holdset);
													holdgroup.holdPosition();
												}
											}
										}
										else
										{
											(*unitit)->attackUnit(*enemies.begin());
										}
									}
									else
									{
										UnitGroup structures = allSelfUnits(isBuilding);
										BWAPI::Unit* neareststructure = nearestUnit((*unitit)->getPosition(), structures);
										if((*it)->getCenter().getDistance(neareststructure->getPosition()) < dist(10.00))
										{
											(*unitit)->attackUnit(*enemies.begin());
										}
									}
								}
								else
								{
									(*unitit)->stop(); // mogelijk moet dit anders als de micro zo vaak hier langs komt dat hij gewoon niet eens aanvalt
								}
							}
							else
							{
								if((*unitit)->getPosition().getDistance(currentTask.position) > dist(6.00))
								{
									(*unitit)->unburrow();
								}
								else
								{
									// hold lurker
									std::set<BWAPI::Unit*> holdset;
									holdset.insert(*unitit);
									UnitGroup olords = allSelfUnits(Overlord)(isIdle);
									if(olords.size() > 0)
									{
										holdset.insert(*olords.begin());
										UnitGroup holdgroup = UnitGroup::getUnitGroup(holdset);
										holdgroup.holdPosition();
									}
								}
							}
						}
					}
				}
				/* EINDE LURKER */

				/* DEFILER */
				else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Defiler)
				{
					if((*unitit)->isUnderStorm())
					{
						moveToNearestBase(*unitit);
					}
					else
					{
						if(allSelfUnits.inRadius(dist(10.00), (*unitit)->getPosition()).not(Defiler).size() > 3)
						{
							UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(9.00), 0);
							bool atleastoneunderswarm = false;
							for(std::set<BWAPI::Unit*>::iterator swarmit=enemies.begin(); swarmit!=enemies.end(); swarmit++)
							{
								if(isUnderDarkSwarm(*swarmit))
								{
									atleastoneunderswarm = true;
								}
							}
							if(!atleastoneunderswarm)
							{
								int energy = (*unitit)->getEnergy();
								int energynodig = BWAPI::TechTypes::Dark_Swarm.energyUsed();
								if(energy < energynodig)
								{
									UnitGroup zerglings = allSelfUnits(Zergling);
									if(zerglings.size() > 0)
									{
										BWAPI::Unit* slachtoffer = nearestUnitInGroup((*unitit), zerglings);
										(*unitit)->useTech(BWAPI::TechTypes::Consume, slachtoffer);
									}
									else
									{
										(*unitit)->rightClick(nearestSwarm(*unitit)->getPosition());
									}
								}
								else
								{
									BWAPI::Unit* nenuds = nearestEnemyNotUnderDarkSwarm(*unitit);
									BWAPI::Unit* swarm = nearestSwarm(nenuds);
									if(nenuds != NULL && swarm != NULL)
									{
										if(nenuds->getPosition().getDistance(swarm->getPosition()) > dist(5.00))
										{
											int x = abs(nenuds->getPosition().x() - swarm->getPosition().x());
											int y = abs(nenuds->getPosition().y() - swarm->getPosition().y());
											BWAPI::Position pos = BWAPI::Position(x, y);
											(*unitit)->useTech(BWAPI::TechTypes::Dark_Swarm, pos);
										}
										else
										{
											(*unitit)->useTech(BWAPI::TechTypes::Dark_Swarm, nenuds);
										}
									}
								}
							}
							else
							{
								// niks
							}
						}
						else
						{
							UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(9.00), 0);
							if(enemies.size() > 0)
							{
								UnitGroup buildings = allSelfUnits(isBuilding).inRadius(dist(5.00), (*unitit)->getPosition());
								if(buildings.size() > 0)
								{
									(*unitit)->useTech(BWAPI::TechTypes::Dark_Swarm, (*unitit));
								}
								else
								{
									moveToNearestBase(*unitit);
								}
							}
							else
							{
								(*unitit)->rightClick(currentTask.position);
							}
						}
					}
				}
				/* EINDE DEFILER */

				/* OVERLORD */
				else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Overlord)
				{
					//logx("\n\ndoMicro overlord ", (*unitit), "\n"); 
					if((*unitit)->isUnderStorm())
					{
						//logx("doMicro overlord ", (*unitit), " under storm moveAway\n");
						(*unitit)->rightClick(moveAway(*unitit));
					}
					else
					{
						Task t = currentTask;
						
						//logx("doMicro overlord ", (*unitit), std::string(" task.type=").append(intToString(t.type)).append("\n").c_str());
						if(t.type == 1 || t.type == 4)
						{
							//logx("doMicro overlord ", (*unitit), " type=1||4\n");
							BWAPI::Unit* nearAir = nearestEnemyThatCanAttackAir(*unitit);
							// de volgende if heeft geen else, hij gaat er niet in, maar is dan klaar met de micro
							if(nearAir != NULL && (*unitit)->getPosition().getDistance(nearAir->getPosition()) < dist(8.00) && t.type == 1)
							{
								//logx("doMicro overlord ", (*unitit), " air enemy dichtbij\n");
								if(overlordSupplyProvidedSoon())
								{
									
									//logx("doMicro overlord ", (*unitit), " overlordSupplySoon");
									UnitGroup buildings = allEnemyUnits(isBuilding).inRadius(dist(8.00), t.position);
									if(buildings.size() == 0)
									{
										//logx("doMicro overlord ", (*unitit), " geen buildings");
										(*unitit)->rightClick(moveAway(*unitit));
									}
									else
									{
										
										//logx("doMicro overlord ", (*unitit), " wel buildings\n");
										if((*unitit)->getPosition().getDistance(t.position) < dist(2.00))
										{
											
											//logx("doMicro overlord ", (*unitit), " moveAway\n");
											(*unitit)->rightClick(moveAway(*unitit));
										}
										else
										{
											
											//logx("doMicro overlord ", (*unitit), " move naar task\n");
											(*unitit)->rightClick(t.position);
										}
									}
								}
								else
								{
									//logx("doMicro overlord ", (*unitit), " moveAway\n");
									(*unitit)->rightClick(moveAway(*unitit));
								}
							}
							else
							{
								UnitGroup stealths = allEnemyUnits(isCloaked);
								BWAPI::Unit* neareststealth = nearestUnit((*unitit)->getPosition(), stealths);
								if(neareststealth != NULL)
								{
										
									//logx("doMicro overlord ", (*unitit), " stealth gezien\n");
									(*unitit)->rightClick(neareststealth->getPosition());
								}
								else
								{
									UnitGroup dropships = allEnemyUnits(Dropship) + allEnemyUnits(Shuttle);
									dropships = dropships.inRadius(dist(10.00), (*unitit)->getPosition());
									if(dropships.size() > 0)
									{
											
										//logx("doMicro overlord ", (*unitit), " dropship gezien\n");
										(*unitit)->rightClick(nearestUnit((*unitit)->getPosition(), dropships)->getPosition());
									}
									else
									{
											
										//logx("doMicro overlord ", (*unitit), " geen dropship, move naar task\n");
										(*unitit)->rightClick(t.position);
									}
								}
							}
						}
						else
						{
							//logx("doMicro overlord ", (*unitit), " hydratask deel\n");
							std::set<Task> hydratasks = this->tm->findTasksWithUnitType(BWAPI::UnitTypes::Zerg_Hydralisk);
							Task* hydratask = NULL;
							for(std::set<Task>::iterator taskit=hydratasks.begin(); taskit!=hydratasks.end(); taskit++)
							{
								if((*taskit).type == 2 || (*taskit).type == 3 && allSelfUnits(Overlord).inRadius(dist(10.00), (*(*taskit).unitGroup->begin())->getPosition()).size() == 0)
								{
									Task loltask = *taskit;
									hydratask = &loltask;
									break;
								}
							}
							if(hydratask != NULL)
							{
								
								//logx("doMicro overlord ", (*unitit), " hydratask\n");
								BWAPI::Unit* volghydra = *(hydratask->unitGroup->begin());
								(*unitit)->rightClick(volghydra->getPosition());
							}
							else
							{
								UnitGroup overlordsnearby = allSelfUnits(Overlord).inRadius(dist(10.00), (*unitit)->getPosition());
								if(overlordsnearby.size() > 1)
								{
									
									//logx("doMicro overlord ", (*unitit), " andere overlord\n");
									if(canAttackAir(enemiesInRange((*unitit)->getPosition(), dist(8.00), 0)))
									{
										
										//logx("doMicro overlord ", (*unitit), " canAttackAir moveAway\n");
										(*unitit)->rightClick(moveAway(*unitit));
									}
									else
									{
										if (!(*unitit)->isMoving())
										{
											//logx("doMicro overlord ", (*unitit), " splitup\n");
											(*unitit)->rightClick(splitup(*unitit));
										}
									}
								}
								else
								{
									UnitGroup buildings = allSelfUnits(isBuilding).inRadius(dist(15.00), (*unitit)->getPosition());
									if(buildings.size() > 0)
									{
										if (!(*unitit)->isMoving())
										{
											//logx("doMicro overlord ", (*unitit), " building random\n");
											// als dit elk frame gebeurt, krijgt hij elk frame een nieuwe positie -> stuiterbal
											int x = (*unitit)->getPosition().x();
											int y = (*unitit)->getPosition().y();
											int factor = dist(10);
											int newx = x + (((rand() % 30)-15)*factor);
											int newy = y + (((rand() % 30)-15)*factor);
											(*unitit)->rightClick(BWAPI::Position(newx, newy));
										}
									}
									else
									{
										
										//logx("doMicro overlord ", (*unitit), " eigen building\n");
										BWAPI::Unit* nearestbuilding = nearestUnit((*unitit)->getPosition(), allSelfUnits(isBuilding));
										(*unitit)->rightClick(nearestbuilding->getPosition());
									}
								}
							}
						}
					}
				}
				/* EINDE OVERLORD */

				/* DRONE */
				else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Drone)
				{
					logx("\n\ndoMicro drone ", (*unitit), "\n");
					if(this->wbm->bouwdrones.count(*unitit) > 0)
					{
						logx("doMicro drone", (*unitit), " drone is aan het bouwen, skip\n");
						continue;
					}
					if((*unitit)->isUnderStorm())
					{
						logx("doMicro drone ", (*unitit), " under storm moveAway\n");
						moveToNearestBase(*unitit);
					}
					else
					{
						if(currentTask.type != 1)
						{
							logx("doMicro drone ", (*unitit), " task.type != 1\n");
							if(canAttackGround(enemiesInRange((*unitit)->getPosition(), dist(5.00), 0)) || this->eiudm->lostHealthThisFrame(*unitit))
							{
								logx("doMicro drone ", (*unitit), " ground enemies of geraakt\n");
								UnitGroup allyAirInRange = allSelfUnits(isFlyer).inRadius(dist(7.00), (*unitit)->getPosition());
								UnitGroup dronesInRange = allSelfUnits(Drone).inRadius(dist(7.00), (*unitit)->getPosition());
								UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(7.00), 0);
								if(!canAttackGround(allyAirInRange) && enemies.size()*4 <= dronesInRange.size())
								{
									logx("doMicro drone ", (*unitit), " drone rage\n");
									(*unitit)->attackUnit(*enemies.begin());
								}
								else
								{
									UnitGroup detectorsInRange = enemiesInRange((*unitit)->getPosition(), dist(10.00), 0)(isDetector);
									if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Burrowing) && detectorsInRange.size() == 0)
									{
										logx("doMicro drone ", (*unitit), " geen detectors, wel burrow\n");
										(*unitit)->burrow();
									}
									else
									{
										UnitGroup militaryInRange = allSelfUnits.inRadius(dist(14.00), (*unitit)->getPosition()).not(isWorker)(canAttack);
										if(militaryInRange.size() > 0)
										{
											logx("doMicro drone ", (*unitit), " military \n");
											(*unitit)->rightClick(nearestUnit((*unitit)->getPosition(), militaryInRange)->getPosition());
										}
										else
										{
											logx("doMicro drone ", (*unitit), " geen military moveAway\n");
											(*unitit)->rightClick(moveAway(*unitit));
										}
									}
								}
							}
							else
							{
								logx("doMicro drone ", (*unitit), " harvestcheck\n");
								UnitGroup mineralDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isGatheringMinerals);
								UnitGroup gasDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isGatheringGas); // has Order gather gas moet er nog bij of juist ipv
								UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted);
								if (gasDrones.size() < extractors.size()*3)
								{
									if (mineralDrones.size()<5)
									{
										if (mineralDrones.size()<3 || gasDrones.size() >= extractors.size()*2)
										{
											mineWhere(*unitit);
										}
										else
										{
											gasWhere(*unitit);
										}
									}
									else
									{
										gasWhere(*unitit);
									}
								}
								else
								{
									if (gasDrones.size() > extractors.size()*3) // teveel gas enzo
									{
										if ((*unitit)->isCarryingGas())
										{
											moveToNearestBase(*unitit);
										}
										else
										{
											mineWhere(*unitit);
										}
										// return de gas terug enzo, rechtermuisklik op nearest hatchery
										// mogelijke probleem hierbij is dat ze allemaal teruggaan, als carryinggas == returngas/gatheringGas
									}
								}
								logx("doMicro drone ", (*unitit), " harvestdone\n");							
							}
						}
						else
						{
							logx("doMicro drone ", (*unitit), " task.type = 1\n");
							if(canAttackGround(enemiesInRange((*unitit)->getPosition(), dist(7.00), 0)))
							{
								logx("doMicro drone ", (*unitit), " enemies moveAway\n");
								(*unitit)->rightClick(moveAway(*unitit));
							}
							else
							{
								if((*unitit)->getPosition().getDistance(currentTask.position) < dist(7))
								{
									logx("doMicro drone ", (*unitit), " moveToNearestBase\n");
									moveToNearestBase(*unitit);
								}
								else
								{
									logx("doMicro drone ", (*unitit), " naar task\n");
									(*unitit)->rightClick(currentTask.position);
								}
							}
						}
					}
					if((*unitit)->isIdle()==true || (*unitit)->getOrder() == BWAPI::Orders::None || (*unitit)->getOrder() == BWAPI::Orders::Nothing)
					{
						logx("doMicro drone ", (*unitit), " harvestcheckidle\n");
						UnitGroup mineralDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isGatheringMinerals);
						UnitGroup gasDrones = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(isGatheringGas); // has Order gather gas moet er nog bij of juist ipv
						UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted);
						if (gasDrones.size() < extractors.size()*3)
						{
							if (mineralDrones.size()<5)
							{
								if (mineralDrones.size()<3 || gasDrones.size() >= extractors.size()*2)
								{
									mineWhere(*unitit);
								}
								else
								{
									gasWhere(*unitit);
								}
							}
							else
							{
								gasWhere(*unitit);
							}
						}
						else
						{
							if (gasDrones.size() > extractors.size()*3) // teveel gas enzo
							{
								if ((*unitit)->isCarryingGas())
								{
									moveToNearestBase(*unitit);
								}
								else
								{
									mineWhere(*unitit);
								}
								// return de gas terug enzo, rechtermuisklik op nearest hatchery
								// mogelijke probleem hierbij is dat ze allemaal teruggaan, als carryinggas == returngas/gatheringGas
							}
							else
							{
								mineWhere(*unitit);
							}
						}
						logx("doMicro drone ", (*unitit), " harvestdonewasidle\n");	
					}
				}
				/* EINDE DRONE */

				/* HYDRALISK */
				else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Hydralisk)
				{
					if((*unitit)->isUnderStorm())
					{
						(*unitit)->rightClick(moveAway(*unitit, dist(10.00)));
					}
					else
					{
						UnitGroup allEnemies = allEnemyUnits;
						UnitGroup allMelee = allEnemies(Drone) + allEnemies(Zealot) + allEnemies(Zergling) + allEnemies(SCV) + allEnemies(Probe) + allEnemies(Ultralisk);
						allMelee = allMelee.inRadius(dist(6.00), (*unitit)->getPosition());
						if(nearestSwarm(*unitit)->getPosition().getDistance((*unitit)->getPosition()) < dist(10.00) && allMelee.size() == 0)
						{
							if(!isUnderDarkSwarm(*unitit))
							{
								(*unitit)->attackMove(nearestSwarm(*unitit)->getPosition());
							}
							else
							{
								UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(10.00), 0);
								BWAPI::Unit* target = NULL;
								for(std::set<BWAPI::Unit*>::iterator enit=enemies.begin(); enit!=enemies.end(); enit++)
								{
									if(!isUnderDarkSwarm(*enit))
									{
										target = *enit;
										break;
									}
								}
								if(target != NULL)
								{
									(*unitit)->attackUnit(target);
								}
							}
						}
						else
						{
							if(allSelfUnits(Hydralisk).inRadius(dist(8.00), (*unitit)->getPosition()).size() > 0)
							{
								int allies = allSelfUnits.inRadius(dist(15.00), (*unitit)->getPosition()).size();
								int enemies = allEnemyUnits.inRadius(dist(15.00), (*unitit)->getPosition()).size();
								if(enemies * 1.5 > allies)
								{
									if((*unitit)->getGroundWeaponCooldown() != 0)
									{
										(*unitit)->rightClick(moveAway(*unitit));
									}
									else
									{
										(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15.00), (*unitit)->getPosition())));
									}
								}
								else
								{
									if((*unitit)->getPosition().getDistance(currentTask.position) > dist(8.00))
									{
										if(enemiesInRange((*unitit)->getPosition(), dist(9.00), 0).size() > 0)
										{
											BWAPI::Unit* nearesttarget = nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15.00), (*unitit)->getPosition()));
											if(nearesttarget->getPosition().getDistance((*unitit)->getPosition()) < dist(9.00))
											{
												if(allSelfUnits(Hydralisk).inRadius(dist(9.00), nearesttarget->getPosition()).size() > 0)
												{
													(*unitit)->rightClick(moveAway(*unitit));
												}
												else
												{
													if((*unitit)->getGroundWeaponCooldown() != 0)
													{
														(*unitit)->rightClick(currentTask.position);
													}
													else
													{
														(*unitit)->attackUnit(nearesttarget);
													}
												}
											}
											else
											{
												if((*unitit)->getGroundWeaponCooldown() != 0)
												{
													(*unitit)->rightClick(currentTask.position);
												}
												else
												{
													(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), allEnemyUnits));
												}
											}
										}
										else
										{
											(*unitit)->attackMove(currentTask.position);
										}
									}
									else
									{
										if(enemiesInRange((*unitit)->getPosition(), dist(9.00), 0).size() > 0)
										{
											BWAPI::Unit* nearesttarget = nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15.00), (*unitit)->getPosition()));
											if(nearesttarget->getPosition().getDistance((*unitit)->getPosition()) < dist(9.00))
											{
												if(allSelfUnits(Hydralisk).inRadius(dist(9.00), nearesttarget->getPosition()).size() > 0)
												{
													(*unitit)->rightClick(moveAway(*unitit));
												}
												else
												{
													if((*unitit)->getGroundWeaponCooldown() != 0)
													{
														(*unitit)->rightClick(moveAway(*unitit));
													}
													else
													{
														(*unitit)->attackUnit(nearesttarget);
													}
												}
											}
											else
											{
												if((*unitit)->getGroundWeaponCooldown() != 0)
												{
													(*unitit)->rightClick(moveAway(*unitit));
												}
												else
												{
													(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), allEnemyUnits));
												}
											}
										}
										else
										{
											(*unitit)->rightClick(moveAway(*unitit));
										}
									}
								}
							}
							else
							{
								UnitGroup nearEnemies = enemiesInRange((*unitit)->getPosition(), dist(9.00), 0);
								if(nearEnemies.size() > 0 && canAttackGround(nearEnemies))
								{
									(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), nearEnemies));
								}
								else
								{
									moveToNearestBase(*unitit);
								}
							}
						}
					}
				}
				/* EINDE HYDRALISK */
			}
		}
	}
}

void MicroManager::moveToNearestBase(BWAPI::Unit* unit)
{
	unit->rightClick(hc->getNearestHatchery(unit->getPosition())->getPosition());
}

void MicroManager::moveToNearestBase(std::set<BWAPI::Unit*> units)
{
	UnitGroup::getUnitGroup(units).rightClick(hc->getNearestHatchery((*units.begin())->getPosition())->getPosition());
}

bool MicroManager::isUnderDarkSwarm(BWAPI::Unit* unit)
{
	UnitGroup darkSwarms = UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits())(Dark_Swarm);
	return darkSwarms.inRadius(dist(5), unit->getPosition()).size() > 0;
}

bool MicroManager::canAttackAir(BWAPI::Unit* unit)
{
	return unit->getType().airWeapon().targetsAir();
}

bool MicroManager::canAttackGround(BWAPI::Unit* unit)
{
	return unit->getType().groundWeapon().targetsGround();
}

bool MicroManager::canAttackAir(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackAir(*it))
		{
			return true;
		}
	}
	return false;
}

bool MicroManager::canAttackGround(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackGround(*it))
		{
			return true;
		}
	}
	return false;
}

int MicroManager::amountCanAttackAir(std::set<BWAPI::Unit*> units)
{
	int amount = 0;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackAir(*it))
		{
			amount++;
		}
	}
	return amount;
}

int MicroManager::amountCanAttackGround(std::set<BWAPI::Unit*> units)
{
	int amount = 0;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackGround(*it))
		{
			amount++;
		}
	}
	return amount;
}

double MicroManager::minimalDistanceToGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units)
{
	double minimalDistance = -1.00;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(minimalDistance == -1.00)
		{
			minimalDistance = currentDistance;
		}
		else if(currentDistance < minimalDistance)
		{
			minimalDistance = currentDistance;
		}
	}
	return minimalDistance;
}

BWAPI::Unit* MicroManager::nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units)
{
	double minimalDistance = -1.00;
	BWAPI::Unit* nearest = NULL;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(minimalDistance == -1.00)
		{
			minimalDistance = currentDistance;
			nearest = *it;
		}
		else if(currentDistance < minimalDistance)
		{
			minimalDistance = currentDistance;
			nearest = *it;
		}
	}
	return nearest;
}

BWAPI::Unit* MicroManager::nearestEnemyThatCanAttackAir(BWAPI::Unit* unit)
{
	BWAPI::Unit* enemy = NULL;
	double distance = -1.00;
	
	UnitGroup enemies = this->eudm->getUG();
	for(std::set<BWAPI::Unit*>::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if((*it)->getType().airWeapon().targetsAir())
		{
			if(distance == -1.00)
			{
				enemy = (*it);
				distance = currentDistance;
			}
			else if(currentDistance < distance)
			{
				enemy = (*it);
				distance = currentDistance;
			}
		}
	}

	return enemy;
}

BWAPI::Unit* MicroManager::nearestNonBuildingEnemy(BWAPI::Unit* unit)
{
	BWAPI::Unit* enemy = NULL;
	double distance = -1.00;
	
	UnitGroup enemies = this->eudm->getUG();
	for(std::set<BWAPI::Unit*>::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(!(*it)->getType().isBuilding())
		{
			if(distance == -1.00)
			{
				enemy = (*it);
				distance = currentDistance;
			}
			else if(currentDistance < distance)
			{
				enemy = (*it);
				distance = currentDistance;
			}
		}
	}

	return enemy;
}

BWAPI::Unit* MicroManager::nearestSwarm(BWAPI::Unit* unit)
{
	UnitGroup darkSwarms = UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits())(Dark_Swarm);
	BWAPI::Unit* nearest = NULL;
	double distance = -1.00;

	for(std::set<BWAPI::Unit*>::iterator it=darkSwarms.begin(); it!=darkSwarms.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(distance == -1.00)
		{
			nearest = *it;
			distance = currentDistance;
		}
		else if(currentDistance < distance)
		{
			nearest = *it;
			distance = currentDistance;
		}
	}
	return nearest;
}

BWAPI::Unit* MicroManager::nearestEnemyNotUnderDarkSwarm(BWAPI::Unit* unit)
{
	UnitGroup enemies = this->eudm->getUG();
	std::set<BWAPI::Unit*> underswarm;
	for(std::set<BWAPI::Unit*>::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		if(isUnderDarkSwarm(*it))
		{
			underswarm.insert(*it);
		}
	}
	UnitGroup notunderswarm = enemies - UnitGroup::getUnitGroup(underswarm);
	return nearestUnit(unit->getPosition(), notunderswarm);
}

// stuitert
BWAPI::Position MicroManager::splitup(BWAPI::Unit* unit)
{
	BWAPI::Position current = unit->getPosition();
	std::set<BWAPI::Position> mogelijkePosities = sanitizePositions(getAdjacentPositions(current));
	if(mogelijkePosities.empty()) { return unit->getPosition(); }
	std::map<BWAPI::Position, int> telling;
	UnitGroup enemies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(GetType, unit->getType()).inRadius(dist(13.00), unit->getPosition());
	if(enemies.size() == 0)
	{
		int x = current.x();
		int y = current.y();
		int factor = dist(20);
		int newx = x + (((rand() % 30)-15)*factor);
		int newy = y + (((rand() % 30)-15)*factor);
		return BWAPI::Position(newx, newy);
	}
	// van set naar map
	for(std::set<BWAPI::Position>::iterator it = mogelijkePosities.begin(); it != mogelijkePosities.end(); it++)
	{
		// aantal enemies waarvoor iets telt
		int aantal = 0;

		for(std::set<BWAPI::Unit*>::iterator iten = enemies.begin(); iten != enemies.end(); iten++)
		{
			if((*iten)->getDistance(*it) > (*iten)->getDistance(unit->getPosition()))
			{
				aantal++;
			}
		}

		telling.insert(std::pair<BWAPI::Position, int>(BWAPI::Position(it->x(), it->y()), aantal));
	}
	// positie met hoogste aantal eruit vissen

	int besteAantal = -1;
	BWAPI::Position bestePositie = unit->getPosition();

	for(std::map<BWAPI::Position, int>::iterator itlol = telling.begin(); itlol != telling.end(); itlol++)
	{
		if(itlol->second >= besteAantal)
		{
			besteAantal = itlol->second;
			bestePositie = itlol->first;
		}
	}
	return bestePositie;
}

std::string MicroManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

void MicroManager::logx(std::string func, BWAPI::Unit* unit, std::string msg)
{
	log(std::string(func).append(intToString(unit->getID())).append(std::string(msg)).c_str());

	// geeft spam aan berichten
	// werkt wel als logx alleen in het laagste if/else niveau van doMicro staat
	//BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), msg.c_str());
}

double MicroManager::dist(int d)
{
	return d*32;
}
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

MicroManager::MicroManager(EnemyUnitDataManager* e, TaskManager* t, HighCommand* h, EigenUnitDataManager* ei, WantBuildManager* w)
{
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
	
	// alle enemies in de gekozen radius
	BWAPI::Position enemiescenter = this->eudm->getUG().inRadius(radius, unit->getPosition()).getCenter();
	
	int newx = current.x() - (enemiescenter.x() - current.x());
	int newy = current.y() - (enemiescenter.y() - current.y());
	// mogelijk een verdere afstand als de onderlinge afstand klein is. Maar wellicht vuurt dit vaak genoeg.
	return BWAPI::Position(newx, newy).makeValid();
}

BWAPI::Position MicroManager::moveAway(BWAPI::Unit* unit)
{
	return moveAway(unit, dist(13));
}

void MicroManager::moveAway(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it=units.begin(); it!=units.end(); it++)
	{
		(*it)->move(moveAway((*it)));
	}
}

BWAPI::Position MicroManager::splitUp(BWAPI::Unit* unit)
{
	// huidige positie van de unit die gaat moven
	BWAPI::Position current = unit->getPosition();
	
	// alle enemies in de gekozen radius
	UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(3), current);
	allies.erase(unit);
	BWAPI::Position alliescenter = allies.getCenter();
	
	int newx = current.x() - (alliescenter.x() - current.x());
	int newy = current.y() - (alliescenter.y() - current.y());
	// mogelijk een verdere afstand als de onderlinge afstand klein is. Maar wellicht vuurt dit vaak genoeg.
	return BWAPI::Position(newx, newy).makeValid();
}

void MicroManager::splitUp(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it=units.begin(); it!=units.end(); it++)
	{
		(*it)->move(splitUp(*it));
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
		(this->wbm->countEggsMorphingInto(BWAPI::UnitTypes::Zerg_Overlord)+this->wbm->buildList.count(BWAPI::UnitTypes::Zerg_Overlord)) > 0;
}

bool MicroManager::enemyInRange(BWAPI::Position p, double radius, int type)
{
	return enemiesInRange(p, radius, type).size() > 0;
}

bool MicroManager::enemyInRange(BWAPI::Position p)
{
	return enemyInRange(p, dist(13), 0);
}

UnitGroup MicroManager::enemiesInRange(BWAPI::Position p, double radius, int type) // 0 beide, 1 ground, 2 flyer
{
	std::map<BWAPI::Unit*, EnemyUnitData> enemies = this->eudm->getEnemyUnitsInRadius(radius, p);
	UnitGroup result;
	for each(std::pair<BWAPI::Unit*, EnemyUnitData> enemy in enemies)
	{
		if(type == 0)
		{
			result.insert(enemy.first);
		}
		if(type == 1)
		{
			if(!enemy.second.unitType.isFlyer())
			{
				result.insert(enemy.first);
			}
		}
		if(type == 2)
		{
			if(enemy.second.unitType.isFlyer())
			{
				result.insert(enemy.first);
			}
		}
	}
	return result;
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
	double besteAfstand = -1;
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
	UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(9), p);
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
			unit->move(this->hc->getNearestHatchery(unit->getPosition())->getPosition());
		}
		unit->gather(nearestUnitInGroup(unit, minerals));
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
			unit->move(this->hc->getNearestHatchery(unit->getPosition())->getPosition());
		}
		unit->gather(nearestUnitInGroup(unit, result));
	}
}

void MicroManager::gatherWhere(BWAPI::Unit* unit)
{
	if ((unit->isGatheringGas()) || (unit->isConstructing()) || ((unit->isMoving()) && (!unit->isCarryingMinerals())) )
	{
		if (unit->isGatheringGas())
		{
			if (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).inRadius(dist(5), unit->getPosition()).size()>3)
			{
				unit->stop();
			}
		}
	}
	else
	{
		UnitGroup extractors = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Extractor)(isCompleted);
		UnitGroup result = extractors;
		logc("(result = extractors) aantal extractors: ");
		logc(this->hc->wantBuildManager->intToString(result.size()).c_str());
		logc("\n");
		if (UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone).size()>5)
		{
			for(std::set<BWAPI::Unit*>::iterator it=extractors.begin(); it!=extractors.end(); it++)
			{
				if(((UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(isGatheringGas).inRadius(dist(10), (*it)->getPosition()).size()+UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(GetTarget, *it).size()) > 2))
				{
					logc("erase extractor\n");
					result.erase(*it); // ug met een unit ehh
				}
			}

			if (!result.empty())
			{
				logc("!result.empty() gaat gas\n");
				logc("!result.empty() aantal extractors: ");
				logc(this->hc->wantBuildManager->intToString(result.size()).c_str());
				logc("\n");
				unit->gather(nearestUnitInGroup(unit, result)); // ga eerst naar extractor
				return;
			}
		}
		if (!unit->isGatheringMinerals())
		{
			result = getUnusedMineralsNearHatcheries();
			if (result.empty())
			{
				/*if (unit->isCarryingMinerals() || unit->isCarryingGas())
				{
					//unit->returnCargo();
				}
				else
				{*/
				if(unit->getDistance(this->hc->getNearestHatchery(unit->getPosition())->getPosition()) < dist(4))
				{
				}
				else
				{
					unit->move(this->hc->getNearestHatchery(unit->getPosition())->getPosition());
					return;
				}
			}
			else
			{
				unit->gather(nearestUnitInGroup(unit, result));
				return;
			}
		}
	}
	logc("einde gatherwhere\n");
}

UnitGroup MicroManager::getHatcheriesWithMinerals()
{
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
	{
		for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)  // stond eerst it!=minerals.end(), lijkt me beetje raar als de rest mit staat? ***
		{
			if((*it)->getDistance(*mit) <= dist(8))
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
	UnitGroup hatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery, Lair, Hive)(isCompleted);
	UnitGroup result = UnitGroup();
	UnitGroup minerals = UnitGroup::getUnitGroup(BWAPI::Broodwar->getMinerals());
	for(std::set<BWAPI::Unit*>::iterator mit=minerals.begin(); mit!=minerals.end(); mit++)
	{
		for(std::set<BWAPI::Unit*>::iterator it=hatcheries.begin(); it!=hatcheries.end(); it++)
		{
			UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(15), (*it)->getPosition()).not(Drone).not(Overlord).not(isBuilding);
			UnitGroup effectivemining = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(10), (*it)->getPosition())(Drone)(isGatheringMinerals);
			UnitGroup mineralsnearby = minerals.inRadius(dist(10), (*it)->getPosition());
			// idee is: per mineral check of er een hatchery in de buurt is, zo ja, check voor die hatchery of het wel een geldige basis is (i.e. geen enemies in de buurt of allies nearby) EN of er niet al genoeg drones in de buurt aant minen zijn.
			if (((amountCanAttackGround(enemiesInRange((*it)->getPosition(), dist(10), 0)) < 5) || (allies.size()>4)) && effectivemining.size()<mineralsnearby.size())
			{
				// als er niet al genoeg drones zijn voor de minerals && de basis is veilig && slechts per 1 hatchery checken of de mineral in de buurt van een hatchery bevindt.
				// het wordt erg dubbelop.. maar op deze manier weet ik zeker dat een mineral niet 10x erin komt te staan, dat niet alle drones gewoon naar de dichtsbijzijnde gaat, etc.
				//if((*it)->getDistance(*mit) <= dist(13) && !(*mit)->isBeingGathered())
				if((*it)->getDistance(*mit) <= dist(13) && UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Drone)(GetTarget, *mit).size() < 2)
				{
					result.insert(*mit);
					break;
				}
			}
			else
			{
				break;
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
	// return a unitgroup of the specified unittype that is in radius of a unit of a unit group;
	UnitGroup* result= new UnitGroup();
	for(std::set<BWAPI::Unit*>::iterator it=ug->begin(); it!=ug->end(); it++)
	{
		UnitGroup newunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(radius, (*it)->getPosition())(GetType, ut);
		result->insert(newunits.begin(), newunits.end()); // kan meerdere keren tot dezelfde UG worden toegevoegd?
	}
	return result;
}

bool MicroManager::tooSplitUp(double radius, UnitGroup* ug)
{
	BWAPI::Position ugcenter = ug->getCenter();
	int amount = 0;
	for(std::set<BWAPI::Unit*>::iterator it=ug->begin(); it!=ug->end(); it++)
	{
		if ((*it)->getDistance(ugcenter) > radius)
		{
			amount++;
		}
	}
	if (ug->size()/2 < amount)
	{
		return true;
	}
	return false;
}

void MicroManager::doMicro(std::set<UnitGroup*> listUG)
{
	logc("\n\ndoMicro\n\n");

	UnitGroup allSelfUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	UnitGroup allEnemyUnits = this->eudm->getUG();

	for(std::set<UnitGroup*>::iterator it=listUG.begin(); it!=listUG.end(); it++)
	{
		/* SCOURGE */
		if((**it)(Scourge).size() > 0)
		{
			BWAPI::Unit* eerste = *((**it)(Scourge).begin());
			UnitGroup airenemies = enemiesInRange(eerste->getPosition(), dist(7), 2);
			UnitGroup allenemies = enemiesInRange(eerste->getPosition(), dist(7), 0);
			if(eerste->isUnderStorm() || canAttackAir(allenemies) && airenemies.size() == 0)
			{
				(*it)->move(this->hc->getNearestHatchery(eerste->getPosition())->getPosition()); // move
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
					if(eerste->getDistance(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position) < dist(9))
					{
						for(std::set<BWAPI::Unit*>::iterator scourgeit=(*it)->begin(); scourgeit!=(*it)->end(); scourgeit++)
						{
							(*scourgeit)->move(splitUp(*scourgeit)); // kijk dit ff na aub
						}
					}
					else
					{
						(*it)->attackMove(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position);
					}
				}
			}
		}
		/* EINDE SCOURGE */

		 /* MUTALISK */
		else if((**it)(Mutalisk).size() > 0)
		{
			BWAPI::Unit* eerste = *((**it)(Mutalisk).begin());
			logx(eerste, " begin micro\n");
			if(eerste->isUnderStorm()) 
			{
				logx(eerste, " is under storm\n");
				moveToNearestBase(**it);
			}
			else
			{
				if(eerste->getGroundWeaponCooldown() != 0 || eerste->getAirWeaponCooldown() != 0)
				{
					logx(eerste, " groundweap cd\n");
					if(canAttackAir(enemiesInRange(eerste->getPosition(), dist(8), 0)))
					{
						logx(eerste, " enemies, moveaway\n");
						moveAway(**it);
					}
					else
					{
						if(eerste->getDistance(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position) < dist(5))
						{
							logx(eerste, " vlakbij task, terug naar base\n");
							moveToNearestBase(**it);
						}
						else
						{
							if (tooSplitUp(dist(3), (*it)))
							{
								(*it)->attackMove(nearestUnit((*it)->getCenter(), (**it))->getPosition());
							}
							else
							{
								logx(eerste, " gogo naar task\n");
								(*it)->attackMove(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position);	 // move
							}
						}
					}
				}
				else
				{
					logx(eerste, " groundweap klaar\n");
					logx(eerste, " bij task\n");
					UnitGroup enemies = enemiesInRange(eerste->getPosition(), dist(7), 0);
					UnitGroup enemiesMilitary = enemiesInRange(eerste->getPosition(), dist(7), 0).not(isBuilding);
					if (enemiesMilitary.size()>0)
					{
						BWAPI::Unit* nearestt = nearestUnit(eerste->getPosition(), enemiesMilitary);
						if (nearestt->getDistance(eerste->getPosition()) < dist(4))
						{
							(*it)->attackUnit(nearestt);
						}
						else
						{
							(*it)->attackMove(nearestt->getPosition());
						}
					}
					else
					{
						if (enemies.size()>0)
						{
							BWAPI::Unit* nearestt = nearestUnit(eerste->getPosition(), enemies);
							if (nearestt->getDistance(eerste->getPosition()) < dist(4))
							{
								(*it)->attackUnit(nearestt);
							}
							else
							{
								(*it)->attackMove(nearestt->getPosition());
							}
						}
						else
						{
							if(eerste->getDistance(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position) < dist(5))
							{
								logx(eerste, " vlakbij task, terug naar base\n");
								moveToNearestBase(**it);
							}
							else
							{
								if (tooSplitUp(dist(3), (*it)))
								{
									(*it)->attackMove(nearestUnit((*it)->getCenter(), (**it))->getPosition());
								}
								else
								{
									logx(eerste, " move naar task\n");
									(*it)->attackMove(this->hc->planAssigner->vindTask(this->hc->hcplan, (*it)).position);
								}
							}
						}
					}
				}
			}
		}
		/* EINDE MUTALISK */

		else
		{
			Task currentTask = this->hc->planAssigner->vindTask(this->hc->hcplan, *it);

			for(std::set<BWAPI::Unit*>::iterator unitit=(*it)->begin(); unitit!=(*it)->end(); unitit++)
			{
				logc("doMicro for unit iterator ");
				logc((*unitit)->getType().getName().c_str());
				logc("\n");
				//Task currentTask = this->hc->planAssigner->vindTask(this->hc->hcplan, this->hc->eigenUnitGroupManager->findUnitGroupWithUnit(*unitit));
				logc("doMicro selecteer unittype\n");

				/* NEW ZERGLING */
				if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Zergling)
				{
					UnitGroup allies = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).inRadius(dist(20), (*unitit)->getPosition());
					UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(10), 1);
					UnitGroup enemiesair = enemiesInRange((*unitit)->getPosition(), dist(7), 2);
					if (MicroManager::amountCanAttackGround(enemiesair)>0 && MicroManager::amountCanAttackAir(allies)==0)
					{
						logx((*unitit), "moveaway van air enemies, geen support in de buurt\n");
						(*unitit)->move(moveAway(*unitit));
					}
					else
					{
						if (currentTask.type == 1)
						{
							logx((*unitit), "scout task\n");
							if (enemiesInRange((*unitit)->getPosition(), dist(4), 0).not(isBuilding).size()>0)
							{
								logx((*unitit), "moveaway van enemies\n");
								(*unitit)->move(moveAway(*unitit));
							}
							else
							{
								logx((*unitit), "geen enemies\n");
								if ((*unitit)->getDistance(currentTask.position) < dist(4) && BWAPI::Broodwar->isVisible(currentTask.position))
								//if(BWTA::getGroundDistance((*unitit)->getTilePosition(), BWAPI::TilePosition(currentTask.position)) < dist(4))
								{
									logx((*unitit), "dichtbij task\n");
									if (!(*unitit)->isMoving())
									{
										logx((*unitit), "staat stil, random lopen\n");
										int x = (*unitit)->getPosition().x();
										int y = (*unitit)->getPosition().y();
										int factor = dist(10);
										int newx = x + (((rand() % 30)-15)*factor);
										int newy = y + (((rand() % 30)-15)*factor);
										(*unitit)->move(BWAPI::Position(newx, newy).makeValid());
									}
								}							
								else
								{
									logx((*unitit), "move naar task\n");
									(*unitit)->move(currentTask.position);
								}
							}
						}
						else
						{
							logx((*unitit), "geen scout task\n");
							BWAPI::Unit* swarm = nearestSwarm((*unitit));
							if(swarm != NULL && swarm->getPosition().getDistance((*unitit)->getPosition()) < dist(9))
							{
								logx((*unitit), " swarm in de buurt\n");
								if(!isUnderDarkSwarm((*unitit)) && !(*unitit)->isAttacking())
								{
									logx((*unitit), " naar swarm\n");
									(*unitit)->attackMove(swarm->getPosition()); // move
								}
								else
								{
									logx((*unitit), " onder swarm, attack enemy\n"); // wat als geen enemy? nullpointer!
									UnitGroup enemiesonderswarm = enemiesInRange((*unitit)->getPosition(), dist(6), 1);
									if (!(*unitit)->isAttacking() && enemiesonderswarm.size()>0)
									{
										(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), enemiesonderswarm));
									}
								}
							}
							else
							{
								logx((*unitit), "outnumbered!\n");
								if ((allies.size())<amountCanAttackGround(enemies.not(isWorker)))
								{
									logx((*unitit), "moveaway!\n");
									(*unitit)->move(moveAway(*unitit));
								}
								else
								{
									logx((*unitit), "niet outnumbered\n");
									if (enemiesInRange((*unitit)->getPosition(), dist(3), 1).size()>0)
									{
										logx((*unitit), "ground enemy in range 3, nothing AI enzo\n");
										// nothing AI enzo
									}
									else
									{
										logx((*unitit), "geen enemy in range 3\n");
										if (enemies.size()>0)
										{
											logx((*unitit), "wel in range 10, attackmove\n");
											BWAPI::Unit* nearest = nearestUnit((*unitit)->getPosition(), enemies);
											(*unitit)->attackMove(nearest->getPosition());
										}
										else
										{
											logx((*unitit), "geen enemies\n");
											if (tooSplitUp(dist(7), *it))
											{
												logx((*unitit), "groep is tooSplitUp, move naar center unit\n");
												(*unitit)->attackMove(nearestUnit((*it)->getCenter(), (**it))->getPosition());
											}
											else
											{
												logx((*unitit), "groep is bij elkaar, move naar task\n");
												(*unitit)->attackMove(currentTask.position);
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					/* ULTRALISK */
					if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Ultralisk)
					{
						if((*unitit)->isIrradiated())
						{
							UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(30), 1);
							if(enemies.size()>0)
							{
								(*unitit)->attackMove(nearestUnit((*unitit)->getPosition(), enemies)->getPosition());
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
								UnitGroup swarms = UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits())(Dark_Swarm).inRadius(dist(9), (*unitit)->getPosition());
								if(swarms.size() > 0)
								{
									if(isUnderDarkSwarm(*unitit))
									{
										// doe niks Game AI lost zelf op
									}
									else
									{
										(*unitit)->move(nearestSwarm(*unitit)->getPosition());
									}
								}
								else
								{
									if(currentTask.type != -1 && (*unitit)->getDistance(currentTask.position) < dist(6) && BWAPI::Broodwar->isVisible(currentTask.position) )
									{
										(*unitit)->attackUnit(*enemiesInRange((*unitit)->getPosition(), dist(6), 0).begin());
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
								if((*unitit)->getPosition().getDistance(currentTask.position) > dist(6) && enemiesInRange((*unitit)->getPosition(), dist(13), 0).size() == 0)
								{
									(*unitit)->move(currentTask.position);
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
								UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(6), 0);
								if(enemiesInRange((*unitit)->getPosition(), dist(13), 0).size() > 0)
								{
									if(!this->eiudm->unitIsSeen(*unitit))
									{
										if(enemies.size() > 3)
										{
											if(enemies(Marine).size() > 0 || enemies(isWorker).size() > 0 || enemies(Zealot).size() > 0 || enemies(Medic).size() > 0 || enemies(Zergling).size() > 0)
											{
												if(nearestUnit((*unitit)->getPosition(), enemies)->getPosition().getDistance((*unitit)->getPosition()) < dist(3))
												{
													(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), enemies)); // mogelijke changen
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
											if((*it)->getCenter().getDistance(neareststructure->getPosition()) < dist(10))
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
									if((*unitit)->getPosition().getDistance(currentTask.position) > dist(6))
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
							if(allSelfUnits.inRadius(dist(10), (*unitit)->getPosition()).not(Defiler).size() > 3)
							{
								UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(9), 0);
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
											(*unitit)->move(nearestSwarm(*unitit)->getPosition());
										}
									}
									else
									{
										BWAPI::Unit* nenuds = nearestEnemyNotUnderDarkSwarm(*unitit);
										BWAPI::Unit* swarm = nearestSwarm(nenuds);
										if(nenuds != NULL && swarm != NULL)
										{
											if(nenuds->getPosition().getDistance(swarm->getPosition()) > dist(5))
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
								UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(9), 0);
								if(enemies.size() > 0)
								{
									UnitGroup buildings = allSelfUnits(isBuilding).inRadius(dist(5), (*unitit)->getPosition());
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
									(*unitit)->move(currentTask.position);
								}
							}
						}
					}
					/* EINDE DEFILER */

					/* OVERLORD */
					else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Overlord)
					{
						logx((*unitit), "\n"); 
						if((*unitit)->isUnderStorm())
						{
							logx((*unitit), " under storm moveAway\n");
							(*unitit)->move(moveAway(*unitit));
						}
						else
						{						
							logx((*unitit), std::string(" task.type=").append(intToString(currentTask.type)).append("\n").c_str());
							if(currentTask.type == 1 || currentTask.type == 4 || currentTask.type == 2)
							{
								logx((*unitit), " type=1||4\n");
								BWAPI::Unit* nearAir = nearestEnemyThatCanAttackAir(*unitit);
								logx((*unitit), "nearAir ok\n");
								// de volgende if heeft geen else, hij gaat er niet in, maar is dan klaar met de micro
								if(nearAir != NULL && canAttackAir(enemiesInRange((*unitit)->getPosition(), dist(9), 0)))
								{
									logx((*unitit), " air enemy dichtbij\n");
									if(overlordSupplyProvidedSoon() && currentTask.type == 1)
									{
									
										logx((*unitit), " overlordSupplySoon\n");
										UnitGroup buildings = allEnemyUnits(isBuilding).inRadius(dist(8), currentTask.position);
										if(buildings.size() == 0)
										{
											logx((*unitit), " geen buildings\n");
											(*unitit)->move(moveAway(*unitit));
										}
										else
										{
										
											logx((*unitit), " wel buildings\n");
											if((*unitit)->getPosition().getDistance(currentTask.position) < dist(2))
											{
											
												logx((*unitit), " moveAway\n");
												(*unitit)->move(moveAway(*unitit));
											}
											else
											{
											
												logx((*unitit), " move naar task\n");
												(*unitit)->move(currentTask.position);
											}
										}
									}
									else
									{
										logx((*unitit), " moveAway\n");
										(*unitit)->move(moveAway(*unitit));
									}
								}
								else
								{
									logx((*unitit), "check stealths\n");
									UnitGroup stealths = allEnemyUnits(isCloaked);
									BWAPI::Unit* neareststealth = nearestUnit((*unitit)->getPosition(), stealths);
									logx((*unitit), "check stealths ok\n");
									if(neareststealth != NULL)
									{
										logx((*unitit), " stealth gezien\n");
										(*unitit)->move(neareststealth->getPosition());
									}
									else
									{
										logx((*unitit), "check dropships\n");
										UnitGroup dropships = allEnemyUnits(Dropship) + allEnemyUnits(Shuttle);
										dropships = dropships.inRadius(dist(10), (*unitit)->getPosition());
										if(dropships.size() > 0)
										{
											logx((*unitit), " dropship gezien\n");
											(*unitit)->move(nearestUnit((*unitit)->getPosition(), dropships)->getPosition());
										}
										else
										{
											if ((*unitit)->getPosition().getDistance(currentTask.position) < dist(6))
											{
												if (!(*unitit)->isMoving())
												{
													int x = (*unitit)->getPosition().x();
													int y = (*unitit)->getPosition().y();
													int factor = dist(10);
													int newx = x + (((rand() % 30)-15)*factor);
													int newy = y + (((rand() % 30)-15)*factor);
													(*unitit)->move(BWAPI::Position(newx, newy));
												}
											}
											else
											{
												logx((*unitit), " geen dropship, move naar task\n");
												(*unitit)->move(currentTask.position);
											}
										}
									}
								}
							}
							else
							{
								/*logx((*unitit), " hydratask deel\n");
								std::list<Task> hydratasks = this->tm->findTasksWithUnitType(BWAPI::UnitTypes::Zerg_Hydralisk);
								Task* hydratask = NULL;
								for(std::list<Task>::iterator taskit=hydratasks.begin(); taskit!=hydratasks.end(); taskit++)
								{
									if((*taskit).type == 2 || (*taskit).type == 3 && allSelfUnits(Overlord).inRadius(dist(10), (*(*taskit).unitGroup->begin())->getPosition()).size() == 0)
									{
										Task loltask = *taskit;
										hydratask = &loltask;
										break;
									}
								}
								if(hydratask != NULL)
								{
								
									logx((*unitit), " hydratask\n");
									BWAPI::Unit* volghydra = *(hydratask->unitGroup->begin());
									(*unitit)->rightClick(volghydra->getPosition());
								}
								else*/
								{
									UnitGroup overlordsnearby = allSelfUnits(Overlord).inRadius(dist(10), (*unitit)->getPosition());
									if(overlordsnearby.size() > 1)
									{
									
										logx((*unitit), " andere overlord\n");
										if(canAttackAir(enemiesInRange((*unitit)->getPosition(), dist(8), 0)))
										{
										
											logx((*unitit), " canAttackAir moveAway\n");
											(*unitit)->move(moveAway(*unitit));
										}
										else
										{
											if (!(*unitit)->isMoving())
											{
												logx((*unitit), " splitUp\n");
												(*unitit)->move(splitUp(*unitit));
											}
										}
									}
									else
									{
										UnitGroup buildings = allSelfUnits(isBuilding).inRadius(dist(15), (*unitit)->getPosition());
										if(buildings.size() > 0)
										{
											if (!(*unitit)->isMoving())
											{
												logx((*unitit), " building random\n");
												// als dit elk frame gebeurt, krijgt hij elk frame een nieuwe positie -> stuiterbal
												int x = (*unitit)->getPosition().x();
												int y = (*unitit)->getPosition().y();
												int factor = dist(10);
												int newx = x + (((rand() % 30)-15)*factor);
												int newy = y + (((rand() % 30)-15)*factor);
												(*unitit)->move(BWAPI::Position(newx, newy));
											}
										}
										else
										{
										
											logx((*unitit), " eigen building\n");
											BWAPI::Unit* nearestbuilding = nearestUnit((*unitit)->getPosition(), allSelfUnits(isBuilding));
											(*unitit)->move(nearestbuilding->getPosition());
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
						/*logx("\n\ndoMicro drone ", (*unitit), "\n");
						if(this->wbm->bouwdrones.count(*unitit) > 0)
						{
							logx((*unitit), " drone is aan het bouwen, skip\n");
							continue;
						}*/
						if((*unitit)->isUnderStorm())
						{
							logx((*unitit), " under storm moveAway\n");
							moveToNearestBase(*unitit);
						}
						else
						{
							if(currentTask.type != 1)
							{
								logx((*unitit), " task.type != 1\n");
								if(canAttackGround(enemiesInRange((*unitit)->getPosition(), dist(5), 0)) || this->eiudm->lostHealthThisFrame(*unitit))
								{
									logx((*unitit), " ground enemies of geraakt\n");
									UnitGroup allyAirInRange = allSelfUnits(isFlyer).inRadius(dist(7), (*unitit)->getPosition());
									UnitGroup dronesInRange = allSelfUnits(Drone).inRadius(dist(7), (*unitit)->getPosition());
									UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(7), 0);
									if(!canAttackGround(allyAirInRange) && enemies.size()*4 <= dronesInRange.size())
									{
										logx((*unitit), " drone rage\n");
										BWAPI::Unit* target = getVisibleUnit(enemies);
										if(target != NULL)
										{
											(*unitit)->attackUnit(target);
										}
										else
										{
											(*unitit)->move(moveAway(*unitit));
										}
									}
									else
									{
										UnitGroup detectorsInRange = enemiesInRange((*unitit)->getPosition(), dist(10), 0)(isDetector);
										if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Burrowing) && detectorsInRange.size() == 0)
										{
											logx((*unitit), " geen detectors, wel burrow\n");
											(*unitit)->burrow();
										}
										else
										{
											UnitGroup militaryInRange = allSelfUnits.inRadius(dist(8), (*unitit)->getPosition()).not(isWorker)(canAttack);
											if(militaryInRange.size() > 0)
											{
												logx((*unitit), " military \n");
												(*unitit)->rightClick(nearestUnit((*unitit)->getPosition(), militaryInRange)->getPosition());
											}
											else
											{
												logx((*unitit), " geen military moveAway\n");
												(*unitit)->move(moveAway(*unitit));
											}
										}
									}
								}
								else
								{
									logx((*unitit), " harvestcheck\n");
									gatherWhere(*unitit);
									logx((*unitit), " harvestdone\n");							
								}
							}
							else
							{
								logx((*unitit), " task.type = 1\n");
								if(canAttackGround(enemiesInRange((*unitit)->getPosition(), dist(7), 0)))
								{
									logx((*unitit), " enemies moveAway\n");
									(*unitit)->move(moveAway(*unitit));
								}
								else
								{
									if((*unitit)->getPosition().getDistance(currentTask.position) < dist(7))
									{
										logx((*unitit), " moveToNearestBase\n");
										moveToNearestBase(*unitit);
									}
									else
									{
										logx((*unitit), " naar task\n");
										(*unitit)->move(currentTask.position);
									}
								}
							}
						}
						if((*unitit)->isIdle()==true || (*unitit)->getOrder() == BWAPI::Orders::None || (*unitit)->getOrder() == BWAPI::Orders::Nothing)
						{
							logx((*unitit), " harvestcheckidle\n");
							gatherWhere(*unitit);
							logx((*unitit), " harvestdonewasidle\n");	
						}
					}
					/* EINDE DRONE */

					/* HYDRALISK */
					else if((*unitit)->getType() == BWAPI::UnitTypes::Zerg_Hydralisk)
					{
						if((*unitit)->isUnderStorm())
						{
							(*unitit)->move(moveAway(*unitit, dist(10)));
						}
						else
						{
							UnitGroup allEnemies = allEnemyUnits;
							UnitGroup allMelee = allEnemies(Drone) + allEnemies(Zealot) + allEnemies(Zergling) + allEnemies(SCV) + allEnemies(Probe) + allEnemies(Ultralisk);
							allMelee = allMelee.inRadius(dist(6), (*unitit)->getPosition());
							BWAPI::Unit* swarm = nearestSwarm(*unitit);
							if(swarm != NULL && swarm->getPosition().getDistance((*unitit)->getPosition()) < dist(10) && allMelee.size() == 0)
							{
								BWAPI::Unit* swarm = nearestSwarm(*unitit);
								if(!isUnderDarkSwarm(*unitit) && swarm != NULL)
								{
									(*unitit)->attackMove(swarm->getPosition());
								}
								else
								{
									UnitGroup enemies = enemiesInRange((*unitit)->getPosition(), dist(10), 0);
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
								if(allSelfUnits(Hydralisk).inRadius(dist(8), (*unitit)->getPosition()).size() > 1)
								{
									int allies = allSelfUnits.inRadius(dist(15), (*unitit)->getPosition()).size();
									int enemies = allEnemyUnits.inRadius(dist(15), (*unitit)->getPosition()).size();
									if(enemies * 1.5 > allies)
									{
										if((*unitit)->getGroundWeaponCooldown() != 0)
										{
											(*unitit)->move(moveAway(*unitit));
										}
										else
										{
											(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15), (*unitit)->getPosition())));
										}
									}
									else
									{
										if((*unitit)->getPosition().getDistance(currentTask.position) > dist(8))
										{
											if(enemiesInRange((*unitit)->getPosition(), dist(9), 0).size() > 0)
											{
												BWAPI::Unit* nearesttarget = nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15), (*unitit)->getPosition()));
												if(nearesttarget->getPosition().getDistance((*unitit)->getPosition()) < dist(9))
												{
													if(allSelfUnits(Hydralisk).inRadius(dist(9), nearesttarget->getPosition()).size() > 0)
													{
														(*unitit)->move(moveAway(*unitit));
													}
													else
													{
														if((*unitit)->getGroundWeaponCooldown() != 0)
														{
															(*unitit)->move(currentTask.position);
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
														(*unitit)->move(currentTask.position);
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
											if(enemiesInRange((*unitit)->getPosition(), dist(9), 0).size() > 0)
											{
												BWAPI::Unit* nearesttarget = nearestUnit((*unitit)->getPosition(), allEnemyUnits.inRadius(dist(15), (*unitit)->getPosition()));
												if(nearesttarget->getPosition().getDistance((*unitit)->getPosition()) < dist(9))
												{
													if(allSelfUnits(Hydralisk).inRadius(dist(9), nearesttarget->getPosition()).size() > 0)
													{
														(*unitit)->move(moveAway(*unitit));
													}
													else
													{
														if((*unitit)->getGroundWeaponCooldown() != 0)
														{
															(*unitit)->move(moveAway(*unitit));
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
														(*unitit)->move(moveAway(*unitit));
													}
													else
													{
														(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), allEnemyUnits));
													}
												}
											}
											else
											{
												(*unitit)->move(moveAway(*unitit));
											}
										}
									}
								}
								else
								{
									UnitGroup nearEnemies = enemiesInRange((*unitit)->getPosition(), dist(9), 0);
									if(nearEnemies.size() > 0 && canAttackGround(nearEnemies))
									{
										(*unitit)->attackUnit(nearestUnit((*unitit)->getPosition(), nearEnemies));
									}
									else
									{
										if (tooSplitUp(dist(8), (*it)))
										{
											(*unitit)->attackMove(nearestUnit((*it)->getCenter(), (**it))->getPosition());
										}
										else
										{
											moveToNearestBase(*unitit);
										}
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
}

void MicroManager::moveToNearestBase(BWAPI::Unit* unit)
{
	BWAPI::Unit* nearest = hc->getNearestHatchery(unit->getPosition());
	if (unit->getPosition().getDistance(nearest->getPosition()) > dist(5)) // voorkomt dat ze buggen
	{
		unit->move(nearest->getPosition());
	}
}

void MicroManager::moveToNearestBase(std::set<BWAPI::Unit*> units)
{
	BWAPI::Unit* nearest = hc->getNearestHatchery(UnitGroup::getUnitGroup(units).getCenter());
	if (UnitGroup::getUnitGroup(units).getCenter().getDistance(nearest->getPosition()) > dist(5))
	{
		UnitGroup::getUnitGroup(units).move(nearest->getPosition()); // move
	}
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
	double minimalDistance = -1;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(minimalDistance == -1)
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
	double minimalDistance = -1;
	BWAPI::Unit* nearest = NULL;
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(minimalDistance == -1)
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
	double distance = -1;
	
	std::map<BWAPI::Unit*, EnemyUnitData> enemies = this->eudm->getData();
	for(std::map<BWAPI::Unit*, EnemyUnitData>::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance(it->second.lastKnownPosition);
		if(it->second.unitType.airWeapon().targetsAir() || it->second.unitType.groundWeapon().targetsAir())
		{
			if(distance == -1)
			{
				enemy = it->first;
				distance = currentDistance;
			}
			else if(currentDistance < distance)
			{
				enemy = it->first;
				distance = currentDistance;
			}
		}
	}

	return enemy;
}

BWAPI::Unit* MicroManager::nearestNonBuildingEnemy(BWAPI::Unit* unit)
{
	BWAPI::Unit* enemy = NULL;
	double distance = -1;
	
	std::map<BWAPI::Unit*, EnemyUnitData> enemies = this->eudm->getData();
	for(std::map<BWAPI::Unit*, EnemyUnitData>::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance(it->second.lastKnownPosition);
		if(!it->second.unitType.isBuilding())
		{
			if(distance == -1)
			{
				enemy = it->first;
				distance = currentDistance;
			}
			else if(currentDistance < distance)
			{
				enemy = it->first;
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
	double distance = -1;

	for(std::set<BWAPI::Unit*>::iterator it=darkSwarms.begin(); it!=darkSwarms.end(); it++)
	{
		double currentDistance = unit->getPosition().getDistance((*it)->getPosition());
		if(distance == -1)
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

std::string MicroManager::intToString(int i) {
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

void MicroManager::logx(BWAPI::Unit* unit, std::string msg)
{
	logc(std::string("MM ").append(unit->getType().getName()).append(" ").append(intToString(unit->getID())).append(std::string(msg)).c_str());

	// geeft spam aan berichten
	// werkt wel als logx alleen in het laagste if/else niveau van doMicro staat
	//BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), msg.c_str());
}

void MicroManager::logc(const char* msg)
{
	if(true)
	{
		log(msg);
	}
}

double MicroManager::dist(int d)
{
	return d*32;
}

BWAPI::Position MicroManager::getCenterPositionFromEnemyMap(std::map<BWAPI::Unit*, EnemyUnitData> data)
{
	int result_x;
	int result_y;
	int cur_x;
	int cur_y;
	double avg_x = 0.0;
	double avg_y = 0.0;
	int aantal = 0;

	for(std::map<BWAPI::Unit*, EnemyUnitData>::iterator i=data.begin();i!=data.end();i++)
	{
			cur_x = i->second.lastKnownPosition.x();
			cur_y = i->second.lastKnownPosition.y();

			avg_x = cur_x + aantal * avg_x / aantal + 1;
			aantal++;
	}

	result_x = int(avg_x);
	result_y = int(avg_y);

	return BWAPI::Position(result_x, result_y);
}

BWAPI::Unit* MicroManager::getVisibleUnit(std::set<BWAPI::Unit*> units)
{
	for each(BWAPI::Unit* unit in units)
	{
		if(unit->isVisible())
		{
			return unit;
		}
	}
	return NULL;
}
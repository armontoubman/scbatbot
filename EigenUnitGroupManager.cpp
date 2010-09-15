#include "EigenUnitGroupManager.h"
#include <BWAPI.h>
#include <UnitGroup.h>
#include "HighCommand.h"
#include "Util.h"
#include <sstream>
#include <algorithm>
#include <set>
#include "TaskManager.h"
#include "Util.h"

EigenUnitGroupManager::EigenUnitGroupManager(HighCommand* hc, EigenUnitDataManager* e, TaskManager* t, PlanAssigner* p)
{
	this->highCommand = hc;
	this->eiudm = e;
	this->tm = t;
	this->pa = p;

	//this->defendlingUG = new UnitGroup();
	//this->overlordUG = new UnitGroup();
	this->droneUG = new UnitGroup();
	//this->defendmutaUG = new UnitGroup();
	//this->defendgroepUG = new UnitGroup();
	//this->lurkergroepUG = new UnitGroup();

	//addUG(this->defendlingUG);
	//addUG(this->overlordUG);
	addUG(this->droneUG);
	/*addUG(this->defendmutaUG);
	addUG(this->defendgroepUG);
	addUG(this->lurkergroepUG);*/

	//std::set<BWAPI::Unit*> units = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	//assignUnits(units);
}

bool EigenUnitGroupManager::canAttackAir(UnitGroup unitGroup)
{
	bool result = true;
	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		if((*i)->getType().airWeapon() == BWAPI::WeaponTypes::None) {
			result = false;
		}
	}
	return result;
}

bool EigenUnitGroupManager::canAttackGround(UnitGroup unitGroup)
{
	bool result = true;
	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		if((*i)->getType().groundWeapon() == BWAPI::WeaponTypes::None) {
			result = false;
		}
	}
	return result;
}

bool EigenUnitGroupManager::onlyAirUnits(UnitGroup unitGroup)
{
	bool result = true;
	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		if((*i)->getType().isFlyer() == false) {
			result = false;
		}
	}
	return result;
}

bool EigenUnitGroupManager::onlyGroundUnits(UnitGroup unitGroup)
{
	bool result = true;
	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		if((*i)->getType().isFlyer() == true) {
			result = false;
		}
	}
	return result;
}

BWAPI::Position EigenUnitGroupManager::getCenterPosition(UnitGroup unitGroup)
{
	return unitGroup.getCenter();
}

double EigenUnitGroupManager::seenRatio(UnitGroup unitGroup)
{
	double ratio = 0.0;
	double add = 1 / unitGroup.size();

	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		if(this->eiudm->unitIsSeen(*i)) {
			ratio = ratio+add;
		}
	}

	return ratio;
}

void EigenUnitGroupManager::addUG(UnitGroup* unitGroup) {
	this->unitGroups.insert(unitGroup);
}

void EigenUnitGroupManager::removeUG(UnitGroup* unitGroup) {
	this->unitGroups.erase(this->unitGroups.find(unitGroup));
}

void EigenUnitGroupManager::moveAll(UnitGroup* ug1, UnitGroup* ug2) {
	if(ug1==ug2) return;
	for(std::set<BWAPI::Unit*>::iterator it=ug1->begin(); it!=ug1->end(); it++)
	{
		ug2->insert(*it);
	}
	ug1->clear();
}

void EigenUnitGroupManager::moveUnitBetweenGroups(UnitGroup* ug1, BWAPI::Unit* unit, UnitGroup* ug2)
{
	ug1->erase(unit);
	ug2->insert(unit);
}

void EigenUnitGroupManager::onRemoveUnit(BWAPI::Unit* unit)
{
	logc("EUGM onRemoveUnit: ");
	logc(unit->getType().getName().c_str());
	logc("\n");
	for(std::set<UnitGroup*>::iterator i=unitGroups.begin();i!=unitGroups.end();i++)
	{
		if((*i)->count(unit) > 0)
		{
			(*i)->erase((*i)->find(unit));
		}
	}
	logc("EUGM onRemoveUnit ok\n");
}

UnitGroup* EigenUnitGroupManager::findUnitGroupWithUnit(BWAPI::Unit* unit)
{
	UnitGroup* ug;
	std::set<BWAPI::Unit*>::iterator uit;
	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		ug = *it;
		uit = ug->find(unit);
		if(uit != ug->end()) {
			return ug;
		}
	}
	return NULL;
}

void EigenUnitGroupManager::dropInEenUG(BWAPI::Unit* unit)
{
	// drop een unit in een ug die er al is of juist in zijn eigen groep
	UnitGroup* andere = findOtherUG(unit);
	if (andere != NULL)
	{
		(*andere).insert(unit);
		// drop unit in de ug
	}
	else
	{
		UnitGroup* newgroep = new UnitGroup();
		(*newgroep).insert(unit);
		addUG(newgroep);
		// in eigen groep
	}
}

UnitGroup* EigenUnitGroupManager::findOtherUG(BWAPI::Unit* unit)
{
	UnitGroup* ug;
	UnitGroup* alin = findUnitGroupWithUnit(unit);

	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		if ((alin)!=(*it))
		{
			ug = *it;
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling || unit->getType() == BWAPI::UnitTypes::Zerg_Ultralisk || unit->getType() == BWAPI::UnitTypes::Zerg_Defiler)
			{
				if ((*ug)(GetType, BWAPI::UnitTypes::Zerg_Zergling).size()>0 && ug->size()<6)
				{
					logc("findotherUGgevonden1\n");
					return ug;
				}
				else
				{
					if ((*ug)(GetType, BWAPI::UnitTypes::Zerg_Ultralisk).size()>0 && ug->size()<6)
					{
						logc("findotherUGgevonden2\n");
						return ug;
					}
					else
					{
						if ((*ug)(GetType, BWAPI::UnitTypes::Zerg_Defiler).size()>0 && ug->size()<6)
						{
							logc("findotherUGgevonden3\n");
							return ug;
						}
					}
				}
			}
			else
			{
				if((*ug)(GetType, unit->getType()).size()>0 && ug->size()<6) {
					logc("findotherUGgevonden4\n");
					return ug;
				}
			}
		}
	}
	return NULL;
}

void EigenUnitGroupManager::update()
{
	if(BWAPI::Broodwar->getFrameCount() > 3000) logc("begin van EIUGM::update()\n");

	if(BWAPI::Broodwar->getFrameCount() > 3000) printGroepen();

	// Pak alle unassigned units en drop ze in een UG waar je over heen itereert:
	UnitGroup assigned;
	logc("preassign\n");
	for(std::set<UnitGroup*>::iterator kit=unitGroups.begin(); kit!=unitGroups.end(); kit++)
	{
		assigned = assigned + (**kit);
	}
	logc("assignklaar\n");
	UnitGroup allUnits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).not(Egg).not(isBuilding).not(Larva);
	logc("allunitsklaar\n");
	UnitGroup unassigned = allUnits - assigned;
	logc("unassigned\n");
	// Assign unassigned to ugs die niet size>=20
	/*	lurkers bij lurkers
	hydras bij hydras
	defiler bij defiler/zerglings/ultralisks -> gewoon samen in 1 dus met volgende 2:
	ultralisks bij zerglings/ultralisks/defiler
	zerglings bij zerglings/defiler/ultralisks
	overlord bij overlord
	drones bij drones
	mutalisk bij mutalisks*/
	for(std::set<BWAPI::Unit*>::iterator unas=unassigned.begin(); unas!=unassigned.end(); unas++)
	{
		BWAPI::UnitType type = (*unas)->getType();
		if(type != BWAPI::UnitTypes::Zerg_Larva || type != BWAPI::UnitTypes::Zerg_Egg || !type.isBuilding())
		{
			logc("magassigned\n");
			if ((*unas)->getType() == BWAPI::UnitTypes::Zerg_Drone)
			{
				logc("droneinUG\n");
				this->droneUG->insert((*unas));
			}
			else
			{
				if ((*unas)->getType() == BWAPI::UnitTypes::Zerg_Overlord)
				{
					logc("overlrodeigengroep\n");
					UnitGroup* newgroep = new UnitGroup();
					(*newgroep).insert((*unas));
					addUG(newgroep);
				}
				else
				{
					logc("begindrop1unit\n");
					dropInEenUG((*unas));
				}
			}
		}
	}


	/*for each unitgroup in listUG (aka = assigned) -> met uitzondering van drones
	if overlord && there is a mutagroep with no overlord
		drop 1 overlord in mutagroep
	else
		if teklein (<5 ofzo)
			disband (ze worde hopelijk vanzelf opnew in een groep gedumpt), simpelweg delete groep van listUG (hopelijk crasht het dan niet lol)
		else
			if te groot (>20 units)
				split up in 2 same size groups
				
				UnitGroup* newmuta = new UnitGroup();
		moveAll(defendmutaUG, newmuta);
		addUG(newmuta);
				
				*/
	logc("assignklaarnugroepzelf\n");
	for(std::set<UnitGroup*>::iterator lit=unitGroups.begin(); lit!=unitGroups.end(); lit++)
	{
		if ((**lit)(GetType, BWAPI::UnitTypes::Zerg_Drone).size()==0)
		{
			if ((**lit)(GetType, BWAPI::UnitTypes::Zerg_Overlord).size()>0)
			{
				for(std::set<UnitGroup*>::iterator kit=unitGroups.begin(); kit!=unitGroups.end(); kit++)
				{
					if((**kit)(Mutalisk).size() > 0 && (**kit)(Overlord).size() == 0)
					{
						logug(*kit, "\tmuta>0 overlord==0\n");
						BWAPI::Unit* eerste = *(**kit)(Mutalisk).begin();
						BWAPI::Unit* besteoverlord = nearestUnitInGroup(eerste, **lit);
						moveUnitBetweenGroups(*lit, besteoverlord, *kit);
						break;
					}
				}
			}
			else
			{
				logc("preteklein\n");
				if ((*lit)->size() < 5 && (**lit)(GetType, BWAPI::UnitTypes::Zerg_Overlord).size()==0 && (**lit)(GetType, BWAPI::UnitTypes::Zerg_Mutalisk).size()==0)
				{
					logc("teklein\n");
					if((*lit)->size() > 0)
					{
						logc("teklein size>0\n");
						if (findOtherUG((*(**lit).begin())) != NULL)
						{
							logc("findotherUGgevondenremove\n");
							removeUG(*lit); // if there exists a ug with similar units DAT NIET DEZE UG is, disband deze groep OF drop alle units hierin daarin -> new methode ofzo maken voor checke
						}
					}
				}
				else
				{
					if ((*lit)->size()>6) // te groot
					{
						logc("tegrootsplitup\n");
						splitGroup(*lit);// splitup groep
					}
				}
				logc("klaar 1 stap ug\n");
			}
		}
	}

	// cleanup lege groepen behalve de vaste 6
    std::set<UnitGroup*> teDeleten;
    for(std::set<UnitGroup*>::iterator pit=unitGroups.begin(); pit!=unitGroups.end(); pit++)
    {
            if((**pit).empty())
            {
                    if(*pit != droneUG)
                    {
                            teDeleten.insert(*pit);
                    }
            }
    }
    for(std::set<UnitGroup*>::iterator dit=teDeleten.begin(); dit!=teDeleten.end(); dit++)
    {
            removeUG(*dit);
    }

}

BWAPI::Unit* EigenUnitGroupManager::nearestUnitInGroup(BWAPI::Unit* unit, std::set<BWAPI::Unit*> units)
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

void EigenUnitGroupManager::splitGroup(UnitGroup* ug)
{
	UnitGroup* nieuwe1 = new UnitGroup();
	UnitGroup* nieuwe2 = new UnitGroup();
	bool zebra = true;
	for(std::set<BWAPI::Unit*>::iterator uit=ug->begin(); uit!=ug->end(); uit++)
	{
		if(zebra)
		{
			nieuwe1->insert(*uit);
		}
		else
		{
			nieuwe2->insert(*uit);
		}
		zebra = !zebra;
	}
	ug->clear();
	addUG(nieuwe1);
	addUG(nieuwe2);
}

bool EigenUnitGroupManager::groupContainsType(std::set<BWAPI::Unit*>* ug, BWAPI::UnitType unittype)
{
	bool result = false;
	for(std::set<BWAPI::Unit*>::iterator it = ug->begin(); it != ug->end(); it++)
	{
		if((*it)->getType() == unittype)
		{
			result = true;
		}
	}
	return result;
}

std::string EigenUnitGroupManager::getName(UnitGroup* group)
{
	std::string unitmsg;
	if(group == this->droneUG) { unitmsg = "droneUG"; }
	else { unitmsg = this->highCommand->wantBuildManager->intToString((int) group); }
	return unitmsg;
}

void EigenUnitGroupManager::logug(UnitGroup* group, char* msg)
{
	logc(getName(group).c_str());
	logc("(");
	logc(this->highCommand->wantBuildManager->intToString(group->size()).c_str());
	logc(")");
	logc(msg);
}

void EigenUnitGroupManager::logc(const char* msg)
{
	if(true)
	{
		log(msg);
	}
}

void EigenUnitGroupManager::printGroepen()
{
	logc("\n\nPRINTGROEPEN\n\n");
	for each(UnitGroup* ug in this->unitGroups)
	{
		logc(getName(ug).c_str());
		logc("\t");
		std::set<BWAPI::Unit*> uset = *ug;
		for each(BWAPI::Unit* unit in uset)
		{
			logc("[");
			logc(unit->getType().getName().c_str());
			logc("] ");
		}
		logc("\n");
	}
	logc("\n\nEIND PRINTGROEPEN\n\n");
}
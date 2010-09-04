#include "EigenUnitGroupManager.h"
#include <BWAPI.h>
#include <UnitGroup.h>
#include "HighCommand.h"
#include "Util.h"
#include <sstream>
#include <algorithm>
#include <set>
#include "TaskManager.h"

EigenUnitGroupManager::EigenUnitGroupManager(HighCommand* hc, EigenUnitDataManager* e, TaskManager* t)
{
	this->highCommand = hc;
	this->eiudm = e;
	this->tm = t;

	this->defendlingUG = new UnitGroup();
	this->overlordUG = new UnitGroup();
	this->droneUG = new UnitGroup();
	this->defendmutaUG = new UnitGroup();
	this->defendgroepUG = new UnitGroup();
	this->lurkergroepUG = new UnitGroup();

	addUG(this->defendlingUG);
	addUG(this->overlordUG);
	addUG(this->droneUG);
	addUG(this->defendmutaUG);
	addUG(this->defendgroepUG);
	addUG(this->lurkergroepUG);

	std::set<BWAPI::Unit*> units = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
	assignUnits(units);
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
	for(std::set<UnitGroup*>::iterator i=unitGroups.begin();i!=unitGroups.end();i++)
	{
		(*i)->erase((*i)->find(unit));
	}
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

void EigenUnitGroupManager::assignUnits(std::set<BWAPI::Unit*> units)
{
	for each(BWAPI::Unit* u in units)
	{
		assignUnit(u);
	}
}

void EigenUnitGroupManager::assignUnit(BWAPI::Unit* unit)
{
	BWAPI::UnitType type = unit->getType();
	if(type == BWAPI::UnitTypes::Zerg_Zergling)
	{
		this->defendlingUG->insert(unit);
	}
	else if(type == BWAPI::UnitTypes::Zerg_Overlord)
	{
		this->overlordUG->insert(unit);
	}
	else if(type == BWAPI::UnitTypes::Zerg_Drone)
	{
		this->droneUG->insert(unit);
	}
	else if(type == BWAPI::UnitTypes::Zerg_Mutalisk)
	{
		this->defendmutaUG->insert(unit);
	}
	else if(type == BWAPI::UnitTypes::Zerg_Lurker)
	{
		this->lurkergroepUG->insert(unit);
	}
	else
	{
		this->defendgroepUG->insert(unit);
	}

	BWAPI::Broodwar->printf("%s %s", "assignUnit", unit->getType().getName());
}

void EigenUnitGroupManager::update()
{
	//van defend naar actievolle UGs
	bool defendmutaconditie = false;
	UnitGroup* defendmutaconditiegroep;

	bool othermutaconditie = false;
	UnitGroup* othermutagroep;

	bool geenmutalingconditie = false;
	UnitGroup* geenmutalinggroep;

	bool anderelurkergroepconditie = false;
	UnitGroup* anderelurkergroep;

	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		if((**it)(Mutalisk).size() > 0 && (**it)(Overlord).size() > 0 && (*it)->size() < 12)
		{
			defendmutaconditie = true;
			defendmutaconditiegroep = *it;
		}

		if((**it)(Mutalisk).size() > 0 && (**it).size() == 12)
		{
			othermutaconditie = true;
			othermutagroep = *it;
		}

		if((**it)(Mutalisk).size() == 0 && (**it)(Zergling).size() == 0 && (**it).size() < 8)
		{
			geenmutalingconditie = true;
			geenmutalinggroep = *it;
		}

		if((**it)(Lurker).size() > 0 && *it != this->lurkergroepUG)
		{
			anderelurkergroepconditie = true;
			anderelurkergroep = *it;
		}
	}

	if(this->defendmutaUG->size() > 0 && defendmutaconditie)
	{
		BWAPI::Unit* firstmuta = *(*defendmutaUG)(Mutalisk).begin();
		moveUnitBetweenGroups(defendmutaUG, firstmuta, defendmutaconditiegroep);
	}

	if(this->defendmutaUG->size() > 2 && othermutaconditie)
	{
		UnitGroup* newmuta = new UnitGroup();
		moveAll(defendmutaUG, newmuta);
		addUG(newmuta);
	}

	int aantalhatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery).size();
	if((defendgroepUG->size()+defendlingUG->size()+defendmutaUG->size() > 2* aantalhatcheries) && defendgroepUG->size()>2)
	{
		if(defendgroepUG->size() < 10 && geenmutalingconditie)
		{
			if(geenmutalinggroep->size() < 8)
			{
				moveAll(defendgroepUG, geenmutalinggroep);
			}
		}
		else
		{
			UnitGroup* newdefendgroep = new UnitGroup();
			moveAll(defendgroepUG, newdefendgroep);
			addUG(newdefendgroep);
		}
	}

	if(lurkergroepUG->size() > 1)
	{
		if(anderelurkergroepconditie)
		{
			moveAll(lurkergroepUG, anderelurkergroep);
		}
		else
		{
			UnitGroup* newlurkergroep = new UnitGroup();
			moveAll(lurkergroepUG, newlurkergroep);
			addUG(newlurkergroep);
		}
	}

	int aantaloverlords = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Overlord).size();
	if(aantaloverlords/2 < overlordUG->size())
	{
		BWAPI::Unit* firstoverlord = *(*overlordUG)(Overlord).begin();
		UnitGroup* newoverlordgroep = new UnitGroup();
		moveUnitBetweenGroups(overlordUG, firstoverlord, newoverlordgroep);
		addUG(newoverlordgroep);
	}


	//Groepen samenvoegen/units toevoegen
	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		if((**it)(Mutalisk).size() > 0 && (**it)(Overlord).size() == 0)
		{
			BWAPI::Unit* eerste = *(**it)(Mutalisk).begin();
			UnitGroup overlords = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Overlord);
			BWAPI::Unit* besteoverlord = nearestUnitInGroup(eerste, overlords);
			moveUnitBetweenGroups(findUnitGroupWithUnit(besteoverlord), besteoverlord, *it);
		}

		bool zerglingmergeconditie = false;
		UnitGroup* zerglingmergegroep;
		bool hydramergeconditie = false;
		UnitGroup* hydramergegroep;
		bool nogeenlurkerconditie = false;
		UnitGroup* nogeenlurkergroep;
		for(std::set<UnitGroup*>::iterator zit=unitGroups.begin(); zit!=unitGroups.end(); zit++)
		{
			if((**zit)(Zergling).size() > 0 && (**zit).size() > 1)
			{
				zerglingmergeconditie = true;
				zerglingmergegroep = *zit;
			}
			if((**zit)(Hydralisk).size() > 0)
			{
				hydramergeconditie = true;
				hydramergegroep = *zit;
			}
			if((**zit)(Lurker).size() > 0 && (**zit).not(Lurker).size() > 0)
			{
				nogeenlurkerconditie = true;
				nogeenlurkergroep = *zit;
			}
		}

		if((**it)(Zergling).size() > 0 && (**it).size() < 5 && zerglingmergeconditie)
		{
			moveAll(*it, zerglingmergegroep);
		}

		if((**it)(Zergling).size() == 0 && (**it)(Mutalisk).size() == 0 && hydramergeconditie)
		{
			moveAll(*it, hydramergegroep);
		}

		if((**it).size() > 20 && (**it)(Lurker).size() == 0)
		{
			splitGroup(*it);
		}

		if((**it)(Lurker).size() > 0 && (**it).not(Lurker).size() > 0)
		{
			BWAPI::Unit* lollurker = *(**it)(Lurker).begin();
			if(nogeenlurkerconditie)
			{
				moveUnitBetweenGroups(*it, lollurker, nogeenlurkergroep);
			}
			else
			{
				UnitGroup* nieuwelurkergroep = new UnitGroup();
				moveUnitBetweenGroups(*it, lollurker, nieuwelurkergroep);
				addUG(nieuwelurkergroep);
			}
		}

		if((**it).size() > 5 && (**it)(Lurker).size() > 1)
		{
			splitGroup(*it);
		}

		int militaryunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).not(isBuilding).not(Overlord).not(Drone).not(SCV).not(Probe).not(Observer).size();
		int aantalzerglings = (**it)(Zergling).size();
		int aantalscouttasks = this->tm->findTasksWithType(1).size();
		int aantalscoutgroups = 0;
		for(std::set<UnitGroup*>::iterator zit=unitGroups.begin(); zit!=unitGroups.end(); zit++)
		{
			if(
				(	((**zit)(Zergling).size() == 1
					|| (**zit)(Overlord).size() == 1
					&& (**zit).size() == 1)
					||
					((**zit)(Zergling).size() == 2
					|| (**zit)(Overlord).size() == 2 )
					&& (**zit).size() == 2 )
			)
			{
				aantalscoutgroups++;
			}
		}

		if(aantalzerglings >9 && militaryunits > 25 && aantalscouttasks/2 > aantalscoutgroups)
		{
			BWAPI::Unit* zergie = *(**it)(Zergling).begin();
			UnitGroup* newzergiegroep = new UnitGroup();
			moveUnitBetweenGroups(*it, zergie, newzergiegroep);
			addUG(newzergiegroep);
		}

	}

	// cleanup lege groepen behalve de vaste 6
	std::set<UnitGroup*> teDeleten;
	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		if((**it).empty())
		{
			if(
				*it != defendlingUG
				&& *it != overlordUG
				&& *it != droneUG
				&& *it != defendmutaUG
				&& *it != defendgroepUG
				&& *it != lurkergroepUG
				)
			{
				teDeleten.insert(*it);
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
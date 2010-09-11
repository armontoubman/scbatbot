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
	//log("EUGM onRemoveUnit: ");
	//log(unit->getType().getName().c_str());
	//log("\n");
	for(std::set<UnitGroup*>::iterator i=unitGroups.begin();i!=unitGroups.end();i++)
	{
		if((*i)->count(unit) > 0)
		{
			(*i)->erase((*i)->find(unit));
		}
	}
	//log("EUGM onRemoveUnit ok\n");
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
	if(type == BWAPI::UnitTypes::Zerg_Larva || type == BWAPI::UnitTypes::Zerg_Egg || type.isBuilding())
	{
		return;
	}
	if(type == BWAPI::UnitTypes::Zerg_Zergling)
	{
		this->defendlingUG->insert(unit);
	}
	else if(type == BWAPI::UnitTypes::Zerg_Overlord)
	{
		this->overlordUG->insert(unit);
		//log("overlordUG size: ");
		//log(this->highCommand->wantBuildManager->intToString(overlordUG->size()).append("\n").c_str());
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

	//log(std::string("EUGM assignUnit ").append(unit->getType().getName()).append("\n").c_str());
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
		//log("for elke unitgroup\n");
		if((**it)(Mutalisk).size() > 0 && (**it)(Overlord).size() > 0 && (*it)->size() < 12)
		{
			//log("mutas en overlords en <12\n");
			defendmutaconditie = true;
			defendmutaconditiegroep = *it;
		}

		if((**it)(Mutalisk).size() > 0 && (**it).size() == 12)
		{
			//log("12 mutas\n");
			othermutaconditie = true;
			othermutagroep = *it;
		}

		if((**it)(Mutalisk).size() == 0 && (**it)(Zergling).size() == 0 && (**it).size() < 8)
		{
			//log("0 mutas en 0 zerglings en <8\n");
			geenmutalingconditie = true;
			geenmutalinggroep = *it;
		}

		if((**it)(Lurker).size() > 0 && *it != this->lurkergroepUG)
		{
			//log("lurkers\n");
			anderelurkergroepconditie = true;
			anderelurkergroep = *it;
		}
	}

	if(this->defendmutaUG->size() > 0 && defendmutaconditie)
	{
		//log("defendmutaconditie\n");
		BWAPI::Unit* firstmuta = *(*defendmutaUG)(Mutalisk).begin();
		moveUnitBetweenGroups(defendmutaUG, firstmuta, defendmutaconditiegroep);
	}

	if(this->defendmutaUG->size() > 2 && othermutaconditie)
	{
		//log("othermutaconfitie\n");
		UnitGroup* newmuta = new UnitGroup();
		moveAll(defendmutaUG, newmuta);
		addUG(newmuta);
	}

	int aantalhatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery,Lair,Hive).size();
	if((defendgroepUG->size()+defendlingUG->size()+defendmutaUG->size() > 2* aantalhatcheries) && defendgroepUG->size()>2)
	{
		//log("meer def dan hatcheries");
		if(defendgroepUG->size() < 10 && geenmutalingconditie)
		{
			//log("geen mutas en zerglings def\n");
			if(geenmutalinggroep->size() < 8)
			{
				//log("moveall def\n");
				moveAll(defendgroepUG, geenmutalinggroep);
			}
		}
		else
		{
			//log("newdefendgroep\n");
			UnitGroup* newdefendgroep = new UnitGroup();
			moveAll(defendgroepUG, newdefendgroep);
			addUG(newdefendgroep);
		}
	}

	if(lurkergroepUG->size() > 1)
	{
		//log(">1 lurker\n");
		if(anderelurkergroepconditie)
		{
			//log("moveall lurkers\n");
			moveAll(lurkergroepUG, anderelurkergroep);
		}
		else
		{
			//log("newlurkergroep\n");
			UnitGroup* newlurkergroep = new UnitGroup();
			moveAll(lurkergroepUG, newlurkergroep);
			addUG(newlurkergroep);
		}
	}

	int aantaloverlords = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Overlord).size();
	if(aantaloverlords/2 < overlordUG->size())
	{
		//log("new overlord groep\n");
		BWAPI::Unit* firstoverlord = *(*overlordUG)(Overlord).begin();
		UnitGroup* newoverlordgroep = new UnitGroup();
		moveUnitBetweenGroups(overlordUG, firstoverlord, newoverlordgroep);
		addUG(newoverlordgroep);
	}


	//Groepen samenvoegen/units toevoegen
	for(std::set<UnitGroup*>::iterator kit=unitGroups.begin(); kit!=unitGroups.end(); kit++)
	{
		//log("for groepen samenvoegen/units toevoegen\n");
		if((**kit)(Mutalisk).size() > 0 && (**kit)(Overlord).size() == 0)
		{
			//log("muta>0 overlord==0\n");
			BWAPI::Unit* eerste = *(**kit)(Mutalisk).begin();
			UnitGroup overlords = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Overlord);
			BWAPI::Unit* besteoverlord = nearestUnitInGroup(eerste, overlords);
			moveUnitBetweenGroups(findUnitGroupWithUnit(besteoverlord), besteoverlord, *kit);
		}

		bool zerglingmergeconditie = false;
		UnitGroup* zerglingmergegroep;
		bool hydramergeconditie = false;
		UnitGroup* hydramergegroep;
		bool nogeenlurkerconditie = false;
		UnitGroup* nogeenlurkergroep;
		//log("net voor de 2e for\n");
		for(std::set<UnitGroup*>::iterator zit=unitGroups.begin(); zit!=unitGroups.end(); zit++)
		{
			//log("for zit\n");
			if((**zit)(Zergling).size() > 0 && (**zit).size() > 1)
			{
				//log("lol1\n");
				zerglingmergeconditie = true;
				zerglingmergegroep = *zit;
			}
			if((**zit)(Hydralisk).size() > 0)
			{
				//log("lol2\n");
				hydramergeconditie = true;
				hydramergegroep = *zit;
			}
			if((**zit)(Lurker).size() > 0 && (**zit).not(Lurker).size() > 0)
			{
				//log("lol3\n");
				nogeenlurkerconditie = true;
				nogeenlurkergroep = *zit;
			}
		}

		if((**kit)(Zergling).size() > 0 && (**kit).size() < 5 && zerglingmergeconditie)
		{
			//log("zerglingmergeconditie\n");
			moveAll(*kit, zerglingmergegroep);
		}

		if((**kit)(Zergling).size() == 0 && (**kit)(Mutalisk).size() == 0 && hydramergeconditie)
		{
			//log("hydramergeconditie\n");
			moveAll(*kit, hydramergegroep);
		}

		if((**kit).size() > 20 && (**kit)(Lurker).size() == 0)
		{
			//log(">20 split\n");
			splitGroup(*kit);
		}

		if((**kit)(Lurker).size() > 0 && (**kit).not(Lurker).size() > 0)
		{
			//log("lurkers en not-lurkers\n");
			BWAPI::Unit* lollurker = *(**kit)(Lurker).begin();
			if(nogeenlurkerconditie)
			{
				//log("nogeenlurkerconditie\n");
				moveUnitBetweenGroups(*kit, lollurker, nogeenlurkergroep);
			}
			else
			{
				//log("nieuwelurkergroep\n");
				UnitGroup* nieuwelurkergroep = new UnitGroup();
				moveUnitBetweenGroups(*kit, lollurker, nieuwelurkergroep);
				addUG(nieuwelurkergroep);
			}
		}

		if((**kit).size() > 5 && (**kit)(Lurker).size() > 1)
		{
			//log(">5 lurker>1 split\n");
			splitGroup(*kit);
		}

		//log("tot hier ok\n");
		int militaryunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).not(isBuilding).not(Overlord).not(Drone).not(SCV).not(Probe).not(Observer).size();
		//log("1\n");
		int aantalzerglings = (**kit)(Zergling).size();
		//log("2\n");
		int aantalscouttasks = 0;
		for each(std::pair<UnitGroup*, Task> paar in this->highCommand->hcplan)
		{
			//log("for task\n");
			if(paar.second.type == 1)
			{
				//log("is een scouttask\n");
				aantalscouttasks++;
			}
		}
		//log("3\n");
		int aantalscoutgroups = 0;
		//log("tot hier nog steeds ok\n");

		for(std::set<UnitGroup*>::iterator rit=unitGroups.begin(); rit!=unitGroups.end(); rit++)
		{
			//log("for rit\n");
			if(
				(	((**rit)(Zergling).size() == 1
					|| (**rit)(Overlord).size() == 1
					&& (**rit).size() == 1)
					||
					((**rit)(Zergling).size() == 2
					|| (**rit)(Overlord).size() == 2 )
					&& (**rit).size() == 2 )
			)
			{
				//log("rare scoutgroepconditie\n");
				aantalscoutgroups++;
			}
			else
			{
				//log("niet dus\n");
			}
		}

		if(aantalzerglings >9 && militaryunits > 25 && aantalscouttasks/2 > aantalscoutgroups)
		{
			//log("newzergiegroep\n");
			BWAPI::Unit* zergie = *(**kit)(Zergling).begin();
			UnitGroup* newzergiegroep = new UnitGroup();
			moveUnitBetweenGroups(*kit, zergie, newzergiegroep);
			addUG(newzergiegroep);
		}
		//log("einde 2e grote for\n");
	}

	// cleanup lege groepen behalve de vaste 6
	std::set<UnitGroup*> teDeleten;
	for(std::set<UnitGroup*>::iterator pit=unitGroups.begin(); pit!=unitGroups.end(); pit++)
	{
		if((**pit).empty())
		{
			//log("lege groep\n");
			if(
				*pit != defendlingUG
				&& *pit != overlordUG
				&& *pit != droneUG
				&& *pit != defendmutaUG
				&& *pit != defendgroepUG
				&& *pit != lurkergroepUG
				)
			{
				//log("te verwijderen\n");
				teDeleten.insert(*pit);
			}
			else
			{
				//log("vaste groep\n");
			}
		}
		//log("einde van de for loop zomg\n");
	}

	for(std::set<UnitGroup*>::iterator dit=teDeleten.begin(); dit!=teDeleten.end(); dit++)
	{
		//log("verwijder\n");
		removeUG(*dit);
	}
	//log("einde van EIUGM::update()\n");
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
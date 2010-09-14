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
		if(this->defendlingUG->count(unit) == 0)
		{
			this->defendlingUG->insert(unit);
			logc("ASSIGN ZERGLING -> DEFENDLINGUG\n");
		}
	}
	else if(type == BWAPI::UnitTypes::Zerg_Overlord)
	{
		this->overlordUG->insert(unit);
		logc("overlordUG size: ");
		logc(this->highCommand->wantBuildManager->intToString(overlordUG->size()).append("\n").c_str());
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

	logc(std::string("EUGM assignUnit ").append(unit->getType().getName()).append("\n").c_str());
}

void EigenUnitGroupManager::update()
{
	if(BWAPI::Broodwar->getFrameCount() > 3000) logc("begin van EIUGM::update()\n");

	if(BWAPI::Broodwar->getFrameCount() > 3000) printGroepen();

	//van defend naar actievolle UGs
	bool defendmutaconditie = false;
	UnitGroup* defendmutaconditiegroep;

	bool othermutaconditie = false;
	UnitGroup* othermutagroep;

	bool geenmutalingconditie = false;
	UnitGroup* geenmutalinggroep;

	bool anderelurkergroepconditie = false;
	UnitGroup* anderelurkergroep;

	UnitGroup* currentGroup;

	for(std::set<UnitGroup*>::iterator it=unitGroups.begin(); it!=unitGroups.end(); it++)
	{
		currentGroup = *it;
		logug(currentGroup, "for elke unitgroup\n");
		if((**it)(Mutalisk).size() > 0 && (**it)(Overlord).size() > 0 && (*it)->size() < 12)
		{
			logug(currentGroup, "\tmutas en overlords en <12\n");
			defendmutaconditie = true;
			defendmutaconditiegroep = *it;
		}

		if((**it)(Mutalisk).size() > 0 && (**it).size() == 12)
		{
			logug(currentGroup, "\t12 mutas\n");
			othermutaconditie = true;
			othermutagroep = *it;
		}

		if((**it)(Mutalisk).size() == 0 && (**it)(Zergling).size() == 0 && (**it).size() < 8)
		{
			logug(currentGroup, "\t0 mutas en 0 zerglings en <8\n");
			geenmutalingconditie = true;
			geenmutalinggroep = *it;
		}

		if((**it)(Lurker).size() > 0 && *it != this->lurkergroepUG)
		{
			logug(currentGroup, "\tlurkers\n");
			anderelurkergroepconditie = true;
			anderelurkergroep = *it;
		}

		(**it) = (**it) - (**it)(isBuilding);
		logug(currentGroup, "eind for elke unitgroup\n");
	}

	if(this->defendmutaUG->size() > 0 && defendmutaconditie)
	{
		logc("defendmutaconditie, zet eerste muta over\n");
		BWAPI::Unit* firstmuta = *(*defendmutaUG)(Mutalisk).begin();
		moveUnitBetweenGroups(defendmutaUG, firstmuta, defendmutaconditiegroep);
	}
	
	if(this->defendmutaUG->size() > 2 && othermutaconditie)
	{
		logc("othermutaconfitie\n");
		UnitGroup* newmuta = new UnitGroup();
		moveAll(defendmutaUG, newmuta);
		addUG(newmuta);
	}
	
	int aantalhatcheries = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Hatchery,Lair,Hive).size();
	if((defendgroepUG->size()+defendlingUG->size()+defendmutaUG->size() > 2* aantalhatcheries) && defendgroepUG->size()>2)
	{
		logc("meer def dan hatcheries");
		if(defendgroepUG->size() < 10 && geenmutalingconditie)
		{
			logc("\tgeen mutas en zerglings def\n");
			if(geenmutalinggroep->size() < 8)
			{
				logc("\t\tmoveall def naar groep zonder zerglings en mutas\n");
				moveAll(defendgroepUG, geenmutalinggroep);
			}
		}
		else
		{
			logc("\tnewdefendgroep\n");
			UnitGroup* newdefendgroep = new UnitGroup();
			moveAll(defendgroepUG, newdefendgroep);
			addUG(newdefendgroep);
		}
	}

	if(lurkergroepUG->size() > 1)
	{
		logc(">1 lurker\n");
		if(anderelurkergroepconditie)
		{
			logc("\tmoveall lurkers\n");
			moveAll(lurkergroepUG, anderelurkergroep);
		}
		else
		{
			logc("\tnewlurkergroep\n");
			UnitGroup* newlurkergroep = new UnitGroup();
			moveAll(lurkergroepUG, newlurkergroep);
			addUG(newlurkergroep);
		}
	}

	int aantaloverlords = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits())(Overlord).size();
	if(aantaloverlords/2 < overlordUG->size())
	{
		logc("new overlord groep\n");
		BWAPI::Unit* firstoverlord = *(*overlordUG)(Overlord).begin();
		UnitGroup* newoverlordgroep = new UnitGroup();
		moveUnitBetweenGroups(overlordUG, firstoverlord, newoverlordgroep);
		addUG(newoverlordgroep);
	}


	//Groepen samenvoegen/units toevoegen
	for(std::set<UnitGroup*>::iterator kit=unitGroups.begin(); kit!=unitGroups.end(); kit++)
	{
		currentGroup = *kit;
		logug(currentGroup, "for groepen samenvoegen/units toevoegen\n");
		if((**kit)(Mutalisk).size() > 0 && (**kit)(Overlord).size() == 0)
		{
			logug(currentGroup, "\tmuta>0 overlord==0\n");
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
		logug(currentGroup, "\t(zit) for binnen for:\n");
		for(std::set<UnitGroup*>::iterator zit=unitGroups.begin(); zit!=unitGroups.end(); zit++)
		{
			currentGroup = *zit;
			logug(currentGroup, "\t\tfor zit\n");
			if((**zit)(Zergling).size() > 0 && (**zit).size() > 1)
			{
				logug(currentGroup, "\t\t\tzerglings>0 en size >1\n");
				zerglingmergeconditie = true;
				zerglingmergegroep = *zit;
			}
			if((**zit)(Hydralisk).size() > 0)
			{
				logug(currentGroup, "\t\t\thydralisks >0\n");
				hydramergeconditie = true;
				hydramergegroep = *zit;
			}
			if((**zit)(Lurker).size() > 0 && (**zit).not(Lurker).size() > 0)
			{
				logug(currentGroup, "\t\t\tlurker>0 en !lurker>0\n");
				nogeenlurkerconditie = true;
				nogeenlurkergroep = *zit;
			}
			logug(currentGroup, "\t\teinde for zit\n");
		}
		currentGroup = *kit;
		logug(currentGroup, "\t(zit) einde for binnen for:\n");
		
		if((**kit)(Zergling).size() > 0 && (**kit).size() < 5 && zerglingmergeconditie)
		{
			logug(currentGroup, "\tzerglingmergeconditie\n");
			int size = this->defendlingUG->size();
			moveAll(*kit, zerglingmergegroep);
			if(size == 2 && this->defendlingUG->size() == 0) {
				logug(*kit, " moveall van deze\n");
				logug(zerglingmergegroep, " naar deze\n");
				//BWAPI::Broodwar->leaveGame();
			}
		}

		if((**kit)(Zergling).size() == 0 && (**kit)(Mutalisk).size() == 0 && hydramergeconditie)
		{
			logug(currentGroup, "\thydramergeconditie\n");
			moveAll(*kit, hydramergegroep);
		}

		if((**kit).size() > 20 && (**kit)(Lurker).size() == 0)
		{
			logug(currentGroup, "\t>20 split\n");
			splitGroup(*kit);
		}

		if((**kit)(Lurker).size() > 0 && (**kit).not(Lurker).size() > 0)
		{
			logug(currentGroup, "\tlurkers en not-lurkers\n");
			BWAPI::Unit* lollurker = *(**kit)(Lurker).begin();
			if(nogeenlurkerconditie)
			{
				logug(currentGroup, "\t\tnogeenlurkerconditie\n");
				moveUnitBetweenGroups(*kit, lollurker, nogeenlurkergroep);
			}
			else
			{
				logug(currentGroup, "\t\tnieuwelurkergroep\n");
				UnitGroup* nieuwelurkergroep = new UnitGroup();
				moveUnitBetweenGroups(*kit, lollurker, nieuwelurkergroep);
				addUG(nieuwelurkergroep);
			}
		}

		if((**kit).size() > 5 && (**kit)(Lurker).size() > 1)
		{
			logug(currentGroup, "\t>5 lurker>1 split\n");
			splitGroup(*kit);
		}

		int militaryunits = UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits()).not(isBuilding).not(Overlord).not(Drone).not(SCV).not(Probe).not(Observer).size();
		int aantalzerglings = (**kit)(Zergling).size();
		int aantalscouttasks = 0;
		logug(currentGroup, "\tcheck tasks\n");
		for each(std::pair<UnitGroup*, Task> paar in this->highCommand->hcplan)
		{
			logug(currentGroup, "\t\tfor task\n");
			if(paar.second.type == 1)
			{
				logug(currentGroup, "\t\t\tis een scouttask\n");
				aantalscouttasks++;
			}
		}
		logug(currentGroup, "\teinde check tasks\n");
		int aantalscoutgroups = 0;

		logug(currentGroup, "\t(rit) for binnen for:\n");
		for(std::set<UnitGroup*>::iterator rit=unitGroups.begin(); rit!=unitGroups.end(); rit++)
		{
			logug(currentGroup, "\t\tfor rit\n");
			if(
				(	(((**rit)(Zergling).size() == 1
					|| (**rit)(Overlord).size() == 1)
					&& (**rit).size() == 1)
					||
					(((**rit)(Zergling).size() == 2
					|| (**rit)(Overlord).size() == 2 )
					&& (**rit).size() == 2 ))
			)
			{
				logug(currentGroup, "\t\t\t <-- deze groep 1ling 1lord 1size of 2ling 2lord 2size\n");
				aantalscoutgroups++;
			}
			else
			{
				logug(currentGroup, "\t\t\tdeze groep is geen scoutgroep\n");
			}
			logug(currentGroup, "\t\teinde for rit\n");
		}
		currentGroup = *kit;
		logug(currentGroup, "\t(rit) einde for binnen for:\n");

		if(aantalzerglings >9 && militaryunits > 25 && aantalscouttasks/2 > aantalscoutgroups)
		{
			logug(currentGroup, "\t\tnewzergiegroep\n");
			BWAPI::Unit* zergie = *(**kit)(Zergling).begin();
			UnitGroup* newzergiegroep = new UnitGroup();
			moveUnitBetweenGroups(*kit, zergie, newzergiegroep);
			addUG(newzergiegroep);
		}
		logug(currentGroup, "\teinde 2e grote for\n");
	}

	// cleanup lege groepen behalve de vaste 6
	std::set<UnitGroup*> teDeleten;
	logug(currentGroup, "\t(pit) for binnen for:\n");
	for(std::set<UnitGroup*>::iterator pit=unitGroups.begin(); pit!=unitGroups.end(); pit++)
	{
		currentGroup = *pit;
		if((**pit).empty())
		{
			logug(currentGroup, "\t\t\tlege groep\n");
			if(
				*pit != defendlingUG
				&& *pit != overlordUG
				&& *pit != droneUG
				&& *pit != defendmutaUG
				&& *pit != defendgroepUG
				&& *pit != lurkergroepUG
				)
			{
				logug(currentGroup, "\t\t\t\tte verwijderen\n");
				teDeleten.insert(*pit);
			}
			else
			{
				logug(currentGroup, "\t\t\t\tvaste groep\n");
			}
		}
	}
	logug(currentGroup, "\t(pit) for binnen for:\n");

	for(std::set<UnitGroup*>::iterator dit=teDeleten.begin(); dit!=teDeleten.end(); dit++)
	{
		logug(currentGroup, "\t\tverwijder\n");
		removeUG(*dit);
	}
	logug(currentGroup, "EIUGM aantal UGs: ");
	logc(this->highCommand->wantBuildManager->intToString(this->unitGroups.size()).c_str());
	logug(currentGroup, "\n");
	if(BWAPI::Broodwar->getFrameCount() > 3000) logug(currentGroup, "einde van EIUGM::update()\n");

	if(BWAPI::Broodwar->getFrameCount() > 3000) printGroepen();
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
	if(group == this->defendlingUG) { unitmsg = "defendlingUG"; }
	else if(group == this->overlordUG) { unitmsg = "overlordUG"; }
	else if(group == this->droneUG) { unitmsg = "droneUG"; }
	else if(group == this->defendmutaUG) { unitmsg = "defendmutaUG"; }
	else if(group == this->defendgroepUG) { unitmsg = "defendgroepUG"; }
	else if(group == this->lurkergroepUG) { unitmsg = "lurkergroepUG"; }
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
	if(BWAPI::Broodwar->getFrameCount() > 3000 && false)
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
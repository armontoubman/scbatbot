#include "EigenUnitGroupManager.h"
#include <BWAPI.h>
#include <UnitGroup.h>
#include "HighCommand.h"
#include "Util.h"
#include <sstream>
#include <algorithm>
#include <set>

EigenUnitGroupManager::EigenUnitGroupManager(HighCommand* hc)
{
	this->highCommand = hc;
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
	/*int result_x;
	int result_y;
	int cur_x;
	int cur_y;
	double avg_x = 0.0;
	double avg_y = 0.0;
	int aantal = 0;

	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		cur_x = (*i)->getPosition().x();
		cur_y = (*i)->getPosition().y();

		avg_x = cur_x + aantal * avg_x / aantal + 1;
		aantal++;
	}

	result_x = int(avg_x);
	result_y = int(avg_y);

	BWAPI::Position* pos;
	pos = new BWAPI::Position(result_x, result_y);
	return pos;*/
	return unitGroup.getCenter();
}

double EigenUnitGroupManager::seenRatio(UnitGroup unitGroup)
{
	double ratio = 0.0;
	double add = 1 / unitGroup.size();

	for(std::set<BWAPI::Unit*>::iterator i=unitGroup.begin();i!=unitGroup.end();i++)
	{
		//if(this->eigenUnitDataManager->unitIsSeen(*i)) { need fix in constructor
		//	ratio = ratio+add;
		//}
	}

	return ratio;
}

void EigenUnitGroupManager::addUG(UnitGroup* unitGroup) {
	this->unitGroups.insert(unitGroup);
}

void EigenUnitGroupManager::removeUG(UnitGroup* unitGroup) {
	this->unitGroups.erase(this->unitGroups.find(unitGroup));
}

void EigenUnitGroupManager::mergeUGs(UnitGroup* ug1, UnitGroup* ug2) {
	*ug1 + *ug2;
	this->unitGroups.erase(this->unitGroups.find(ug2));
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
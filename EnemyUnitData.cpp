#include "EnemyUnitData.h"
#include <BWAPI.h>

EnemyUnitData::EnemyUnitData()
{
	this->hitPoints = 0;
	this->unitType = BWAPI::UnitTypes::None;
	this->position = BWAPI::Positions::None;
	this->lastKnownPosition = BWAPI::Positions::None;
	this->lastSeen = -1;
	this->hitPoints = 0;
}

void EnemyUnitData::update(BWAPI::Unit* u)
{
	this->unit = u;
	this->unitType = u->getType();
	this->position = u->getPosition();
	this->lastKnownPosition = u->getPosition();
	this->lastSeen = BWAPI::Broodwar->getFrameCount();
	this->hitPoints = u->getHitPoints();
}
#pragma once
#include "EnemyUnitData.h"
#include <BWAPI.h>
#include "HighCommand.h"

EnemyUnitData::EnemyUnitData()
{
	this->unitType = BWAPI::UnitTypes::Unknown;
	this->hitPoints = 0;
	this->position = Positions::Unknown;
	this->lastKnownPosition = Positions::None;
	this->lastSeen = -1;
}

void EnemyUnitData::update(BWAPI::Unit* u)
{
	this->unitType = u->getType();
	this->hitPoints = u->getHitPoints();
	this->position = u->getPosition();
	this->lastKnownPosition = u->getPosition();
	this->lastSeen = Broodwar->getFrameCount();
}
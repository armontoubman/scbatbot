#include "EnemyUnitData.h"
#include <BWAPI.h>

EnemyUnitData::EnemyUnitData()
{
	this->hitPoints = 0;
}

void EnemyUnitData::update(BWAPI::Unit* u)
{
	this->unit = u;
	this->unitType = u->getType();
	this->hitPoints = u->getHitPoints();
	this->position = u->getPosition();
}
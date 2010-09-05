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
	if(BWAPI::Broodwar->isVisible(u->getPosition()) && BWAPI::Broodwar->unitsOnTile(u->getTilePosition().x(), u->getTilePosition().y()).count(u) == 0)
	{
		this->position = BWAPI::Positions::Unknown;
	}
	if(u->isVisible())
	{
		this->position = u->getPosition();
		this->lastSeen = BWAPI::Broodwar->getFrameCount();
		this->hitPoints = u->getHitPoints();
	}
}
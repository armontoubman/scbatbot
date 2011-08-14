#pragma once

#include "DroneMicro.h"
#include <BWAPI.h>
#include "Task.h"
#include "GlobalMicro.h"
#include "Util.h"

#include "ContractManager.h"

DroneMicro::DroneMicro()
{
}

DroneMicro::DroneMicro(HighCommand* h)
{
	this->hc = h;
}

void DroneMicro::micro(BWAPI::Unit* unit)
{

	if(this->hc->ctm->isContracted(unit))
	{
		BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), std::string("\ncontracted").c_str());
		if(unit->getOrder() == BWAPI::Orders::PlaceBuilding)
		{
			// niks
		}
		else
		{
			Contract c = this->hc->ctm->getContract(unit);
			BWAPI::TilePosition pos = this->hc->ctm->findPositionFor(c.unittype);
			if(!BWAPI::Broodwar->isVisible(pos))
			{
				unit->move(BWAPI::Position(pos));
			}
			else
			{
				if(c.unittype == BWAPI::UnitTypes::None)
				{
					unit->build(pos, BWAPI::UnitTypes::Zerg_Hatchery);
				}
				else
				{
					unit->build(pos, c.unittype);
				}
			}
		}
		return;
	}

	Task task = this->hc->ta->getTaskOfUnit(unit);
	
	if(unit->isUnderStorm())
	{
		moveToNearestBase(unit);
	}
	else
	{
		if(task.getType() != ScoutTask)
		{
			if(canAttackGround(getEnemyUnitsInRadius(unit->getPosition(), dist(5))) || unit->isUnderAttack())
			{
				UnitGroup allyAirInRange = allEigenUnits()(isFlyer).inRadius(dist(7), unit->getPosition());
				UnitGroup dronesInRange = allEigenUnits()(Drone).inRadius(dist(7), unit->getPosition());
				UnitGroup enemies = allEnemyUnits().inRadius(dist(7), unit->getPosition());
				if(!canAttackGround(allyAirInRange) && enemies.size()*2 <= dronesInRange.size())
				{
					BWAPI::Unit* target = getNearestUnit(unit->getPosition(), enemies);
					if(target != NULL)
					{
						unit->attack(target);
					}
					else
					{
						unit->move(moveAway(unit));
					}
				}
				else
				{
					UnitGroup detectorsInRange = allEnemyUnits().inRadius(dist(10), unit->getPosition())(isDetector);
					if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Burrowing) && detectorsInRange.size() == 0)
					{
						unit->burrow();
					}
					else
					{
						UnitGroup militaryInRange = allEigenUnits().inRadius(dist(8), unit->getPosition()).not(isWorker)(canAttack);
						if(militaryInRange.size() > 0)
						{
							unit->attack(getNearestUnit(unit->getPosition(), militaryInRange));
						}
						else
						{
							unit->move(moveAway(unit));
						}
					}
				}
			}
			else
			{
				if((task.getType() == GatherMineralsTask || task.getType() == GatherGasTask) )
				{
					if(unit->isIdle())
					{
						unit->rightClick(task.getTarget());
					}
					else
					{
						// task has switched from gas->minerals or minerals->gas
						if(task.getType() == GatherMineralsTask && (unit->getOrder() == BWAPI::Orders::MoveToGas || unit->getOrder() == BWAPI::Orders::HarvestGas || unit->getOrder() == BWAPI::Orders::ReturnGas))
						{
							unit->rightClick(task.getTarget());
						}
						else if(task.getType() == GatherGasTask && (unit->getOrder() == BWAPI::Orders::MoveToMinerals || unit->getOrder() == BWAPI::Orders::MiningMinerals || unit->getOrder() == BWAPI::Orders::ReturnMinerals))
						{
							unit->rightClick(task.getTarget());
						}
					}
				}
			}
		}
		else
		{
			if(canAttackGround(allEnemyUnits().inRadius(dist(7), unit->getPosition())))
			{
				unit->move(moveAway(unit));
			}
			else
			{
				if(unit->getPosition().getDistance(task.getPosition()) < dist(7))
				{
					moveToNearestBase(unit);
				}
				else
				{
					unit->move(task.getPosition());
				}
			}
		}
	}
}
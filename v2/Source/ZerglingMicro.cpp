#pragma once

#include "ZerglingMicro.h"
#include <BWAPI.h>
#include "Task.h"
#include "GlobalMicro.h"
#include "Util.h"

ZerglingMicro::ZerglingMicro()
{
}

ZerglingMicro::ZerglingMicro(HighCommand* h)
{
	this->hc = h;
}

void ZerglingMicro::micro(BWAPI::Unit* unit)
{
	Task task = this->hc->ta->getTaskOfUnit(unit);
	
	UnitGroup allies = allEigenUnits().inRadius(dist(12), unit->getPosition());
	UnitGroup enemiesground = allEnemyUnits().inRadius(dist(10), unit->getPosition()).not(isFlyer);
	UnitGroup enemiesair = allEnemyUnits().inRadius(dist(7), unit->getPosition())(isFlyer);
	if(amountCanAttackGround(enemiesair) > 0 && amountCanAttackAir(allies) == 0)
	{
		unit->move(moveAway(unit));
	}
	else
	{
		if(task.getType() == ScoutTask)
		{
			if(allEnemyUnits().not(isBuilding).inRadius(dist(4), unit->getPosition()).size() > 0)
			{
				unit->move(moveAway(unit));
			}
			else
			{
				if(unit->getDistance(task.getPosition()) < dist(4) && BWAPI::Broodwar->isVisible(BWAPI::TilePosition(task.getPosition())))
				{
					if(!unit->isMoving())
					{
						int x = unit->getPosition().x();
						int y = unit->getPosition().y();
						int factor = dist(10);
						int newx = x + (((rand() % 30)-15)*factor);
						int newy = y + (((rand() % 30)-15)*factor);
						unit->move(BWAPI::Position(newx, newy).makeValid());
					}
				}
				else
				{
					unit->move(task.getPosition());
				}
			}
		}
		else
		{
			UnitGroup swarms = allUnits()(Dark_Swarm);
			BWAPI::Unit* swarm = getNearestUnit(unit->getPosition(), swarms);
			if(swarm != NULL && swarm->getPosition().getDistance(unit->getPosition()) < dist(9))
			{
				if(!isUnderDarkSwarm(unit) && !unit->isAttacking())
				{
					unit->attack(swarm->getPosition());
				}
				else
				{
					UnitGroup enemiesunderswarm = allEnemyUnits().inRadius(dist(6), unit->getPosition()).not(isFlyer);
					if(!unit->isAttacking() && enemiesunderswarm.size() > 0)
					{
						unit->attack(getNearestUnit(unit->getPosition(), enemiesunderswarm));
					}
				}
			}
			else
			{
				if(allies.size() < amountCanAttackGround(enemiesground))
				{
					unit->move(moveAway(unit));
				}
				else
				{
					if(enemiesground.inRadius(dist(3), unit->getPosition()).size() > 0)
					{
						// game AI
					}
					else
					{
						if(enemiesground.size() > 0)
						{
							BWAPI::Unit* nearest = getNearestUnit(unit->getPosition(), enemiesground);
							unit->attack(nearest);
						}
						else
						{
							UnitGroup* ug = this->hc->eiugm->getGroupOfUnit(unit);
							if(ug != NULL && tooSplitUp(dist(7), *ug))
							{
								BWAPI::Unit* nearest = getNearestUnit(ug->getCenter(), *ug);
								unit->attack(nearest);
							}
							else
							{
								unit->move(task.getPosition());
							}
						}
					}
				}
			}
		}
	}
}
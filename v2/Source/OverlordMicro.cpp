#pragma once

#include "OverlordMicro.h"
#include <BWAPI.h>
#include "Task.h"
#include "GlobalMicro.h"
#include "Util.h"

OverlordMicro::OverlordMicro()
{
}

OverlordMicro::OverlordMicro(HighCommand* h)
{
	this->hc = h;
}

void OverlordMicro::micro(BWAPI::Unit* unit)
{
	Task task = this->hc->ta->getTaskOfUnit(unit);
	
	if(unit->isUnderStorm())
	{
		moveToNearestBase(unit);
	}
	else
	{
		if(task.getType() == ScoutTask || task.getType() == DetectorTask || task.getType() == CombatTask)
		{
			BWAPI::Unit* nearAir = this->hc->eudm->nearestEnemyThatCanAttackAir(unit);
			if(nearAir != NULL && canAttackAir(allEnemyUnits().inRadius(dist(10), unit->getPosition())))
			{
				unit->move(moveAway(unit));
			}
			else
			{
				UnitGroup stealths = allEnemyUnits()(isCloaked);
				BWAPI::Unit* nearestStealth = getNearestUnit(unit->getPosition(), stealths);
				if(nearestStealth != NULL)
				{
					unit->move(nearestStealth->getPosition());
				}
				else
				{
					UnitGroup dropships = allEnemyUnits()(Dropship,Shuttle).inRadius(dist(10), unit->getPosition());
					if(dropships.size() > 0)
					{
						unit->move(getNearestUnit(unit->getPosition(), dropships)->getPosition());
					}
					else
					{
						if(unit->getPosition().getDistance(task.getPosition()) < dist(6))
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
			}
		}
		else
		{
			UnitGroup overlords = allEigenUnits()(Overlord).inRadius(dist(10), unit->getPosition());
			if(overlords.size() > 1)
			{
				if(canAttackAir(allEnemyUnits().inRadius(dist(8), unit->getPosition())))
				{
					unit->move(moveAway(unit));
				}
				else
				{
					if(!unit->isMoving())
					{
						unit->move(splitUp(unit));
					}
				}
			}
			else
			{
				UnitGroup buildings = allEigenUnits()(isBuilding).inRadius(dist(15), unit->getPosition());
				if(buildings.size() > 0)
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
					BWAPI::Unit* nearestbuilding = getNearestUnit(unit->getPosition(), allEigenUnits()(isBuilding));
					unit->move(nearestbuilding->getPosition());
				}
			}
		}
	}
}
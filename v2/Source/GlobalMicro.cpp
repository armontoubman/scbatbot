#pragma once
#include "GlobalMicro.h"
#include <BWAPI.h>
#include <BWTA.h>
#include "Util.h"
#include "UnitGroup.h"

void moveToNearestBase(BWAPI::Unit* unit)
{
	UnitGroup bases = allEigenUnits()(Hatchery,Lair,Hive);
	BWAPI::Unit* nearest = getNearestUnit(unit->getPosition(), bases);
	unit->move(nearest->getPosition());
}

bool canAttackAir(BWAPI::Unit* unit)
{
	return unit->getType().airWeapon().targetsAir();
}

bool canAttackAir(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackAir(*it))
		{
			return true;
		}
	}
	return false;
}

bool canAttackGround(BWAPI::Unit* unit)
{
	return unit->getType().groundWeapon().targetsGround();
}

bool canAttackGround(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator it = units.begin(); it != units.end(); it++)
	{
		if(canAttackGround(*it))
		{
			return true;
		}
	}
	return false;
}

BWAPI::Position moveAway(BWAPI::Unit* unit, double radius)
{
	// huidige positie van de unit die gaat moven
	BWAPI::Position current = unit->getPosition();
	
	// alle enemies in de gekozen radius
	BWAPI::Position enemiescenter = allEnemyUnits().inRadius(radius, unit->getPosition()).getCenter();
	
	int newx = current.x() - (enemiescenter.x() - current.x());
	int newy = current.y() - (enemiescenter.y() - current.y());
	// mogelijk een verdere afstand als de onderlinge afstand klein is. Maar wellicht vuurt dit vaak genoeg.
	return BWAPI::Position(newx, newy).makeValid();
}

BWAPI::Position moveAway(BWAPI::Unit* unit)
{
	return moveAway(unit, dist(13));
}

BWAPI::Position splitUp(BWAPI::Unit* unit)
{
	// huidige positie van de unit die gaat moven
	BWAPI::Position current = unit->getPosition();
	
	// alle enemies in de gekozen radius
	UnitGroup allies = allEigenUnits().inRadius(dist(3), current);
	BWAPI::Position alliescenter = allies.getCenter();
	
	int newx = current.x() - (alliescenter.x() - current.x());
	int newy = current.y() - (alliescenter.y() - current.y());
	// mogelijk een verdere afstand als de onderlinge afstand klein is. Maar wellicht vuurt dit vaak genoeg.
	return BWAPI::Position(newx, newy).makeValid();
}
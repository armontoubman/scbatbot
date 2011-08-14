#pragma once
#include "Util.h"
#include <fstream>
#include <stdarg.h>
#include <sys/stat.h>
#include <sstream>
#include <BWAPI.h>
#include <BWTA.h>
#include "UnitGroup.h"
char buffer[1024];
void log(const char* text, ...)
{
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  va_list ap;
  va_start(ap, text);
  vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE, text, ap);
  va_end(ap);

  FILE *outfile;
  if (fopen_s(&outfile, "bwapi-data\\logs\\BWSAL.log", "a+")==0)
  {
    fprintf_s(outfile, buffer);
    fclose(outfile);
  }
}

std::string intToString(int i)
{
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

std::string floatToString(float f)
{
	std::ostringstream buffer;
	buffer << f;
	return buffer.str();
}

int dist(int d)
{
	return d*32;
}

BWAPI::Unit* getNearestUnit(BWAPI::Position pos, UnitGroup ug)
{
	double besteAfstand = -1.00;
	BWAPI::Unit* besteUnit = NULL;
	double d = -1.00;
	for(std::set<BWAPI::Unit*>::iterator it = ug.begin(); it != ug.end(); it++)
	{
		d = (*it)->getDistance(pos);
		if(besteAfstand == -1)
		{
			besteAfstand = d;
			besteUnit = (*it);
		}
		else
		{
			if(d < besteAfstand)
			{
				besteAfstand = d;
				besteUnit = (*it);
			}
		}
	}

	return besteUnit;
}

BWAPI::Position getNearestPosition(BWAPI::Position pos, std::set<BWAPI::Position> posset)
	{
	double besteAfstand = -1.00;
	BWAPI::Position bestePos = BWAPI::Positions::None;
	double d = -1.00;
	for(std::set<BWAPI::Position>::iterator it = posset.begin(); it != posset.end(); it++)
	{
		d = it->getDistance(pos);
		if(besteAfstand == -1)
		{
			besteAfstand = d;
			bestePos = (*it);
		}
		else
		{
			if(d < besteAfstand)
			{
				besteAfstand = d;
				bestePos = (*it);
			}
		}
	}

	return bestePos;
}

BWAPI::TilePosition getNearestTilePosition(BWAPI::TilePosition pos, std::set<BWAPI::TilePosition> posset)
	{
	double besteAfstand = -1.00;
	BWAPI::TilePosition bestePos = BWAPI::TilePositions::None;
	double d = -1.00;
	for(std::set<BWAPI::TilePosition>::iterator it = posset.begin(); it != posset.end(); it++)
	{
		d = it->getDistance(pos);
		if(besteAfstand == -1)
		{
			besteAfstand = d;
			bestePos = (*it);
		}
		else
		{
			if(d < besteAfstand)
			{
				besteAfstand = d;
				bestePos = (*it);
			}
		}
	}

	return bestePos;
}

BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset)
{
	int result_x;
	int result_y;
	int cur_x;
	int cur_y;
	double avg_x = 0.0;
	double avg_y = 0.0;
	int aantal = 0;

	for(std::set<BWAPI::Position>::iterator i=posset.begin();i!=posset.end();i++)
	{
			cur_x = (*i).x();
			cur_y = (*i).y();

			avg_x = cur_x + aantal * avg_x / aantal + 1;
			aantal++;
	}

	result_x = int(avg_x);
	result_y = int(avg_y);

	return BWAPI::Position(result_x, result_y);
}

bool isMilitary(BWAPI::UnitType unittype)
{
	return !unittype.isBuilding() && unittype != BWAPI::UnitTypes::Zerg_Overlord
		&& unittype != BWAPI::UnitTypes::Zerg_Drone && unittype != BWAPI::UnitTypes::Terran_SCV
		&& unittype != BWAPI::UnitTypes::Protoss_Probe && unittype != BWAPI::UnitTypes::Protoss_Observer;
}

std::map<BWAPI::UnitType, int> countUnitTypesInUnitGroup(std::set<BWAPI::Unit*> myUnits)
{
	std::map<BWAPI::UnitType, int> unitTypeCounts;
	for(std::set<BWAPI::Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	return unitTypeCounts;
}

UnitGroup getEnemyUnitsInRadius(BWAPI::Position pos, int radius)
{
	std::set<BWAPI::Unit*> units = BWAPI::Broodwar->getUnitsInRadius(pos, radius);
	std::set<BWAPI::Unit*> result;
	for each(BWAPI::Unit* u in units)
	{
		if(u->getPlayer() == BWAPI::Broodwar->enemy())
		{
			result.insert(u);
		}
	}
	return UnitGroup::getUnitGroup(result);
}

UnitGroup getEigenUnitsInRadius(BWAPI::Position pos, int radius)
{
	std::set<BWAPI::Unit*> units = BWAPI::Broodwar->getUnitsInRadius(pos, radius);
	std::set<BWAPI::Unit*> result;
	for each(BWAPI::Unit* u in units)
	{
		if(u->getPlayer() == BWAPI::Broodwar->self())
		{
			result.insert(u);
		}
	}
	return UnitGroup::getUnitGroup(result);
}

UnitGroup allEigenUnits()
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->self()->getUnits());
}

UnitGroup allEnemyUnits()
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->enemy()->getUnits());
}

UnitGroup allUnits()
{
	return UnitGroup::getUnitGroup(BWAPI::Broodwar->getAllUnits());
}

UnitGroup filterMilitary(UnitGroup ug)
{
	UnitGroup result;

	for each(BWAPI::Unit* u in ug)
	{
		if(isMilitary(u->getType()))
		{
			result.insert(u);
		}
	}

	return result;
}
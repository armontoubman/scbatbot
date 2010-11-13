#pragma once
#include <BWAPI.h>
#include "EigenUnitData.h"
#include "HighCommand.h"
class EigenUnitDataManager
{
	friend class HighCommand;
public:
	EigenUnitDataManager(HighCommand* h);
	void update(std::set<BWAPI::Unit*> units, std::set<BWAPI::Unit*> enemyUnits);
	void onRemoveUnit(BWAPI::Unit* unit);
	void eventHitPointsChanged(BWAPI::Unit* unit, int difference);
	void eventUnitSeen(BWAPI::Unit* unit);
	bool unitIsSeen(BWAPI::Unit* unit);
	bool lostHealthThisFrame(BWAPI::Unit* unit);

	std::string intToString(int i);
	std::map<BWAPI::Unit*,EigenUnitData> myUnitsMap;
	bool haveUnitOfType(BWAPI::UnitType type);

	std::set<BWAPI::Unit*> lostHealthSet;
	void addToHealthSet(BWAPI::Unit* unit);
	void clearHealthSet();

	HighCommand* hc;
};
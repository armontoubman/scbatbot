#pragma once
#include <BWAPI.h>
class HighCommand
{
public:
	class UnitData
	{
	public:
		int hitPoints;
		bool seen;
		int seenTime;
		BWAPI::Position* seenPosition;
		UnitData(){
			hitPoints = 0;
			seen = false;
			seenTime = 0;
			seenPosition = new BWAPI::Position();
		}
	};
	HighCommand();
	void update(std::set<BWAPI::Unit*> units, std::set<BWAPI::Unit*> enemyUnits);
	void onRemoveUnit(BWAPI::Unit* unit);
	void eventHitPointsChanged(BWAPI::Unit* unit, int difference);
	void eventUnitSeen(BWAPI::Unit* unit);
private:
	std::map<BWAPI::Unit*,UnitData> myUnits;
	std::string intToString(int i);
};
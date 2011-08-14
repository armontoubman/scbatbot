#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

using namespace BWAPI;

struct Contract
{
	BWAPI::Unit* drone;
	BWAPI::UnitType unittype;

	Contract()
	{
		drone = NULL;
		unittype = BWAPI::UnitTypes::Unknown; // None = expand
	}
	
	bool operator==(const Contract& l)
	{
		return ((l.drone == drone) && (l.unittype == unittype));
	}
};

class ContractManager
{
public:
	ContractManager(HighCommand* h);
	void update();

	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitMorph(BWAPI::Unit* u);

	bool isContracted(BWAPI::Unit* drone);
	int count(BWAPI::UnitType unittype);
	int countExpand();
	void newContract(BWAPI::UnitType unittype);
	void newExpandContract();
	Contract getContract(BWAPI::Unit* drone);
	BWAPI::TilePosition findPositionFor(BWAPI::UnitType unittype);
	BWAPI::TilePosition findPositionForExpand();
	std::list<Contract> getContractList();
	int getTotalGas();
	int getTotalMinerals();
private:
	HighCommand* hc;
	std::list<Contract> contractList;
	BWAPI::Unit* findAvailableDrone();
	void removeContract(BWAPI::UnitType unittype);
};
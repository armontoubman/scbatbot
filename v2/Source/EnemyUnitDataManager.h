#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

#include "EnemyUnitData.h"
#include <boost/unordered_map.hpp>
#include "Task.h"

using namespace BWAPI;

typedef boost::unordered_map<Unit*, EnemyUnitData> EnemyUnitMap ;

class EnemyUnitDataManager
{
public:
	EnemyUnitDataManager(HighCommand* h);
	void update();
	void onStart();
	void onUnitDiscover(Unit* u);
	void onUnitEvade(Unit* u);
	void onUnitShow(Unit* u);
	void onUnitHide(Unit* u);
	void onUnitCreate(Unit* u);
	void onUnitDestroy(Unit* u);
	void onUnitMorph(Unit* u);
	void onUnitRenegade(Unit* u);

	void createTask(TaskType tasktype, Position position, Unit* u);
	std::list<Task> getTasklist(TaskType tasktype);

	BWAPI::Unit* nearestEnemyThatCanAttackAir(BWAPI::Unit* unit);

	int count(BWAPI::UnitType unittype);

	std::string chat();
private:
	HighCommand* hc;
	void updateUnit(Unit* u);
	void cleanup();
	void clearTasklists();

	EnemyUnitMap unitmap;
	std::list<Task> scouttasklist;
	std::list<Task> combattasklist;
};
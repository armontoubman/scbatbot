#pragma once
#include <BWAPI.h>
#include "EnemyUnitData.h"
#include <InformationManager.h>
#include <UnitGroup.h>
class EnemyUnitDataManager
{
	friend class HighCommand;
public:
	EnemyUnitDataManager();
	void update(std::set<BWAPI::Unit*> units);
	void onRemoveUnit(BWAPI::Unit* unit);
	std::map<BWAPI::Unit*,EnemyUnitData> getData();
	int count(BWAPI::UnitType unittype);
	UnitGroup getUG();
	EnemyUnitData getEnemyUnitData(BWAPI::Unit* unit);
	std::map<BWAPI::Unit*, EnemyUnitData> getEnemyUnitsInRadius(double radius, BWAPI::Position pos);
	int nrMilitaryUnits(std::set<BWAPI::Unit*> ug);
	bool isMilitary(BWAPI::UnitType unittype);
	std::map<BWAPI::Unit*, EnemyUnitData> getMapFromUG(UnitGroup* ug);
	bool onlyAirUnits(std::map<BWAPI::Unit*, EnemyUnitData> data);
	bool onlyGroundUnits(std::map<BWAPI::Unit*, EnemyUnitData> data);
	bool canAttackAir(BWAPI::UnitType unittype);
	bool canAttackGround(BWAPI::UnitType unittype);
	bool canAttackAir(std::map<BWAPI::Unit*, EnemyUnitData> data);
	bool canAttackGround(std::map<BWAPI::Unit*, EnemyUnitData> data);

	bool mapContainsAir(std::map<BWAPI::Unit*, EnemyUnitData> data);
	bool mapContainsGround(std::map<BWAPI::Unit*, EnemyUnitData> data);
	int mapSize(std::map<BWAPI::Unit*, EnemyUnitData> data);
	int mapMilitarySize(std::map<BWAPI::Unit*, EnemyUnitData> data);
private:
	std::string intToString(int i);
	std::map<BWAPI::Unit*,EnemyUnitData> enemyUnitsMap;
	InformationManager* informationManager;
};
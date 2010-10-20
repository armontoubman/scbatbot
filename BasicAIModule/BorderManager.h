#pragma once
#include <BWAPI.h>
class BorderManager
{
public:
	void setInformationManager(InformationManager* informationManager);
	void addMyBase(BWTA::BaseLocation* location);
	void BorderManager::removeMyBase(BWTA::BaseLocation* location);
	void BorderManager::update();
	void BorderManager::recalculateBorders();
private:
	const std::set<BWTA::Chokepoint*>& BorderManager::getMyBorder();
	const std::set<BWTA::Chokepoint*>& BorderManager::getEnemyBorder();
};
#include "EigenUnitDataManager.h"

#include <BWAPI.h>
#include "HighCommand.h"

#include "UnitGroup.h"
#include <boost/format.hpp>
#include "ContractManager.h"

EigenUnitDataManager::EigenUnitDataManager(HighCommand* h)
{
	this->hc = h;
}

void EigenUnitDataManager::update()
{
}

void EigenUnitDataManager::onStart()
{
}

void EigenUnitDataManager::updateUnit(Unit* u)
{
}

std::string EigenUnitDataManager::chat()
{
	std::string s;
	s = boost::str( boost::format( "EIUDM: %d units" ) % this->unitmap.size() );
	return s;
}

void EigenUnitDataManager::onUnitDiscover(Unit* u)
{
}

void EigenUnitDataManager::onUnitEvade(Unit* u)
{
}

void EigenUnitDataManager::onUnitShow(Unit* u)
{
}

void EigenUnitDataManager::onUnitHide(Unit* u)
{
}

void EigenUnitDataManager::onUnitCreate(Unit* u)
{
}

void EigenUnitDataManager::onUnitDestroy(Unit* u)
{
}

void EigenUnitDataManager::onUnitMorph(Unit* u)
{
}

void EigenUnitDataManager::onUnitRenegade(Unit* u)
{
}
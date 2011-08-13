#include "EigenUnitGroupManager.h"

#include <BWAPI.h>
#include "HighCommand.h"

#include "UnitGroup.h"
#include "Util.h"
#include <boost/format.hpp>

EigenUnitGroupManager::EigenUnitGroupManager(HighCommand* h)
{
	this->hc = hc;
	this->droneUG = new UnitGroup();
	this->unitgroupset.insert(this->droneUG);
	this->initiated = false;
}

void EigenUnitGroupManager::update()
{
	if(!this->initiated)
	{
		this->init();
	}
	this->balanceGroups();
	this->cleanup();
}

void EigenUnitGroupManager::init()
{
	if(Broodwar->getFrameCount() > 1)
	{
		for each(Unit* u in Broodwar->self()->getUnits())
		{
			if(this->isGroupable(u))
			{
				this->assignUnit(u);
			}
		}
		this->initiated = true;
	}
}

void EigenUnitGroupManager::onUnitCreate(Unit* u)
{
	if(this->isGroupable(u))
	{
		this->assignUnit(u);
	}
}

void EigenUnitGroupManager::onUnitDestroy(Unit* u)
{
	if(this->isGroupable(u))
	{
		this->removeUnit(u);
	}
}

void EigenUnitGroupManager::onUnitMorph(Unit* u)
{
	if(this->isGroupable(u))
	{
		if(this->unitIsInAGroup(u))
		{
			this->removeUnit(u);
		}
		this->assignUnit(u);
	}
	else
	{
		this->removeUnit(u);
	}
}

void EigenUnitGroupManager::onUnitRenegade(Unit* u)
{
	if(this->isGroupable(u))
	{
		this->assignUnit(u);
	}
	else
	{
		this->removeUnit(u);
	}
}

bool EigenUnitGroupManager::isGroupable(Unit* u)
{
	return u->getPlayer() == Broodwar->self() && 
		!u->getType().isBuilding() && 
		u->getType() != UnitTypes::Zerg_Egg && 
		u->getType() != UnitTypes::Zerg_Larva;
}

void EigenUnitGroupManager::removeUnit(Unit* u)
{
	UnitGroup* currentugptr = NULL;
	for each(UnitGroup* ugptr in this->unitgroupset)
	{
		if(ugptr->contains(u))
		{
			currentugptr = ugptr;
			break;
		}
	}
	if(currentugptr != NULL)
	{
		currentugptr->erase(currentugptr->find(u));
	}
}

void EigenUnitGroupManager::cleanup()
{
	std::set<UnitGroup*>::iterator it = this->unitgroupset.begin();
	while ( it != unitgroupset.end() ) {
		if ( (*it)->empty() && (*it) != this->droneUG ) {
			it = unitgroupset.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void EigenUnitGroupManager::balanceGroups()
{
	for each(UnitGroup* ugptr in this->unitgroupset)
	{
		// if the current group has no drones
		if((*ugptr)(Drone).size() == 0)
		{
			// if it has an overlord
			if((*ugptr)(Overlord).size() > 0)
			{
				for each(UnitGroup* mutaugptr in this->unitgroupset)
				{
					// if there is a group with mutalisks but no overlord
					if((*mutaugptr)(Mutalisk).size() > 0 && (*mutaugptr)(Overlord).size() == 0)
					{
						// give it the overlord
						Position mutapos = (*mutaugptr).getCenter();
						Unit* nearest = getNearestUnit(mutapos, (*ugptr));
						moveUnitBetweenGroups(nearest, ugptr, mutaugptr);
						break;
					}
				}
			}
			// if it has no overlord
			else
			{
				// if it is too small and not an overlord- or mutaliskgroup
				if(ugptr->size() < 5 && (*ugptr)(Overlord, Mutalisk).size() == 0)
				{
					UnitGroup tempug = (*ugptr);
					ugptr->clear();
					for each(Unit* u in tempug)
					{
						this->assignUnit(u);
					}
				}
				if(ugptr->size() > 12)
				{
					this->splitGroup(ugptr);
				}
			}
		}
	}
}

void EigenUnitGroupManager::assignUnit(Unit* u)
{
	//Broodwar->printf("EIUGM: assigning %s %d", u->getType().getName().c_str(), u->getID());
	if(u->getType() == UnitTypes::Zerg_Drone)
	{
		this->droneUG->insert(u);
	}
	else if(u->getType() == UnitTypes::Zerg_Overlord)
	{
		UnitGroup* newOverlordGroup = new UnitGroup();
		newOverlordGroup->insert(u);
		this->unitgroupset.insert(newOverlordGroup);
	}
	else
	{
		UnitGroup* fit = findFittingGroupForUnit(u);
		if(fit != NULL)
		{
			fit->insert(u);
		}
		else
		{
			UnitGroup* newfitgroup = new UnitGroup();
			newfitgroup->insert(u);
			this->unitgroupset.insert(newfitgroup);
		}
	}
	//Broodwar->printf(this->chat().c_str());
}

std::set<UnitGroup*> EigenUnitGroupManager::getUnitGroupSet()
{
	return this->unitgroupset;
}

UnitGroup* EigenUnitGroupManager::findFittingGroupForUnit(Unit* u)
{
	for each(UnitGroup* ugptr in this->unitgroupset)
	{
		if(u->getType() == UnitTypes::Zerg_Zergling || u->getType() == UnitTypes::Zerg_Ultralisk || u->getType() == UnitTypes::Zerg_Defiler)
		{
			if((*ugptr)(Zergling).size() > 0 && ugptr->size() < 12)
			{
				return ugptr;
			}
			else
			{
				if((*ugptr)(Ultralisk).size() > 0 && ugptr->size() < 12)
				{
					return ugptr;
				}
				else
				{
					if((*ugptr)(Defiler).size() > 0 && ugptr->size() < 12)
					{
						return ugptr;
					}
				}
			}
		}
		else
		{
			if((*ugptr)(GetType, u->getType()).size() > 0 && ugptr->size() < 12)
			{
				return ugptr;
			}
		}
	}
	return NULL;
}

void EigenUnitGroupManager::moveUnitBetweenGroups(Unit* u, UnitGroup* ug1, UnitGroup* ug2)
{
	ug1->erase(ug1->find(u));
	ug2->insert(u);
}

void EigenUnitGroupManager::splitGroup(UnitGroup* ugptr)
{
	UnitGroup* nieuwe1 = new UnitGroup();
	UnitGroup* nieuwe2 = new UnitGroup();
	bool zebra = true;
	for each(Unit* u in (*ugptr))
	{
		if(zebra)
		{
			nieuwe1->insert(u);
		}
		else
		{
			nieuwe2->insert(u);
		}
		zebra = !zebra;
	}
	ugptr->clear();
	this->unitgroupset.insert(nieuwe1);
	this->unitgroupset.insert(nieuwe2);
}

bool EigenUnitGroupManager::unitIsInAGroup(Unit* u)
{
	for each(UnitGroup* ug in this->unitgroupset)
	{
		if(ug->contains(u))
		{
			return true;
		}
	}
	return false;
}

std::string EigenUnitGroupManager::chat()
{
	for each(UnitGroup* ugptr in this->unitgroupset)
	{
		std::string text = "UG: ";
		std::map<UnitType, int> counts = countUnitTypesInUnitGroup(*ugptr);
		for each(std::pair<UnitType, int> typepair in counts)
		{
			text.append(intToString(typepair.second)).append(" ").append(typepair.first.getName()).append(",");
		}
		Broodwar->printf(text.c_str());
	}

	std::string s;
	s = boost::str( boost::format( "EIUGM: %d groups") %this->unitgroupset.size() );
	return s;
}

bool EigenUnitGroupManager::groupIsDroneUG(UnitGroup* ug)
{
	if(ug == this->droneUG)
	{
		return true;
	}
	return false;
}

UnitGroup* EigenUnitGroupManager::getGroupOfUnit(BWAPI::Unit* u)
{
	for each(UnitGroup* ug in this->unitgroupset)
	{
		if(ug->contains(u))
		{
			return ug;
		}
	}
	return NULL;
}

UnitGroup* EigenUnitGroupManager::getDroneUG()
{
	return this->droneUG;
}
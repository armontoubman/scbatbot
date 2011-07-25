#pragma once
#include "BuildItem.h"
#include <BWAPI.h>

BuildItem::BuildItem()
{
	this->priority = 1;
	this->base = 0;
	this->type = NoType;
	this->buildtype = BWAPI::UnitTypes::None;
	this->researchtype = BWAPI::TechTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

BuildItem::BuildItem(BWAPI::UnitType t, int p)
{
	this->buildtype = t;
	this->type = BuildType;
	this->priority = p;
	this->base = 0;
	this->researchtype = BWAPI::TechTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

BuildItem::BuildItem(BWAPI::TechType t, int p)
{
	this->researchtype = t;
	this->type = ResearchType;
	this->priority = p;
	this->base = 0;
	this->buildtype = BWAPI::UnitTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

BuildItem::BuildItem(BWAPI::UpgradeType t, int p)
{
	this->upgradetype = t;
	this->type = UpgradeType;
	this->priority = p;
	this->base = 0;
	this->buildtype = BWAPI::UnitTypes::None;
	this->researchtype = BWAPI::TechTypes::None;
}

int BuildItem::mineralPrice()
{
	int price;
	switch(this->type) {
	case BuildType:
		{
			price = this->buildtype.mineralPrice();
			break;
		}
	case ResearchType:
		{
			price = this->researchtype.mineralPrice();
			break;
		}
	case UpgradeType:
		{
			price = this->upgradetype.mineralPrice(1); // factor?
			break;
		}
	default:
		{
			price = -1;
		}
	}
	return price;
}

int BuildItem::gasPrice()
{
	int price;
	switch(this->type) {
	case BuildType:
		{
			price = this->buildtype.gasPrice();
			break;
		}
	case ResearchType:
		{
			price = this->researchtype.gasPrice();
			break;
		}
	case UpgradeType:
		{
			price = this->upgradetype.gasPrice(1); // factor?
			break;
		}
	default:
		{
			price = -1;
		}
	}
	return price;
}

bool BuildItem::operator==(const BuildItem& other) const
{
	if(type != other.type) return false;
	if(type == BuildType)
	{
		if(buildtype == other.buildtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == ResearchType)
	{
		if(researchtype == other.researchtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == UpgradeType)
	{
		if(upgradetype == other.upgradetype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == ExpandType)
	{
		if(other.type == ExpandType)
		{
			return true;
		} else {
			return false;
		}
	}
	return false;
}

BuildItem BuildItem::expand()
{
	this->type = ExpandType;
	return *this;
}
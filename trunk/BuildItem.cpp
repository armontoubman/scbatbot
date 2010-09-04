#pragma once
#include "BuildItem.h"
#include <BWAPI.h>

BuildItem::BuildItem()
{
	this->priority = 1;
	this->base = 0;
}

BuildItem::BuildItem(BWAPI::UnitType t, int p)
{
	this->buildtype = t;
	this->typenr = 1;
	this->priority = p;
	this->base = 0;
}

BuildItem::BuildItem(BWAPI::TechType t, int p)
{
	this->researchtype = t;
	this->typenr = 2;
	this->priority = p;
	this->base = 0;
}

BuildItem::BuildItem(BWAPI::UpgradeType t, int p)
{
	this->upgradetype = t;
	this->typenr = 3;
	this->priority = p;
	this->base = 0;
}

int BuildItem::mineralPrice() {
	int price;
	switch(this->typenr) {
	case 1:
		{
			price = this->buildtype.mineralPrice();
			break;
		}
	case 2:
		{
			price = this->researchtype.mineralPrice();
			break;
		}
	case 3:
		{
			price = this->upgradetype.mineralPriceBase(); // factor?
			break;
		}
	default:
		{
			price = -1;
		}
	}
	return price;
}

int BuildItem::gasPrice() {
	int price;
	switch(this->typenr) {
	case 1:
		{
			price = this->buildtype.gasPrice();
			break;
		}
	case 2:
		{
			price = this->researchtype.gasPrice();
			break;
		}
	case 3:
		{
			price = this->upgradetype.gasPriceBase(); // factor?
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
	if(typenr != other.typenr) return false;
	if(typenr == 1)
	{
		if(buildtype == other.buildtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(typenr == 2)
	{
		if(researchtype == other.researchtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(typenr == 3)
	{
		if(upgradetype == other.upgradetype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(typenr == 4)
	{
		if(other.typenr == 4)
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
	this->typenr = 4;
	return *this;
}
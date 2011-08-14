#pragma once
#include "Product.h"
#include <BWAPI.h>

Product::Product()
{
	this->priority = 1;
	this->base = 0;
	this->type = NoProduct;
	this->buildtype = BWAPI::UnitTypes::None;
	this->techtype = BWAPI::TechTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

Product::Product(BWAPI::UnitType t, int p)
{
	this->buildtype = t;
	this->type = BuildProduct;
	this->priority = p;
	this->base = 0;
	this->techtype = BWAPI::TechTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

Product::Product(BWAPI::TechType t, int p)
{
	this->techtype = t;
	this->type = TechProduct;
	this->priority = p;
	this->base = 0;
	this->buildtype = BWAPI::UnitTypes::None;
	this->upgradetype = BWAPI::UpgradeTypes::None;
}

Product::Product(BWAPI::UpgradeType t, int p)
{
	this->upgradetype = t;
	this->type = UpgradeProduct;
	this->priority = p;
	this->base = 0;
	this->buildtype = BWAPI::UnitTypes::None;
	this->techtype = BWAPI::TechTypes::None;
}

int Product::mineralPrice()
{
	int price;
	switch(this->type) {
	case BuildProduct:
		{
			price = this->buildtype.mineralPrice();
			break;
		}
	case TechProduct:
		{
			price = this->techtype.mineralPrice();
			break;
		}
	case UpgradeProduct:
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

int Product::gasPrice()
{
	int price;
	switch(this->type) {
	case BuildProduct:
		{
			price = this->buildtype.gasPrice();
			break;
		}
	case TechProduct:
		{
			price = this->techtype.gasPrice();
			break;
		}
	case UpgradeProduct:
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

bool Product::operator==(const Product& other) const
{
	if(type != other.type) return false;
	if(type == BuildProduct)
	{
		if(buildtype == other.buildtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == TechProduct)
	{
		if(techtype == other.techtype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == UpgradeProduct)
	{
		if(upgradetype == other.upgradetype)
		{
			return true;
		} else {
			return false;
		}
	}
	if(type == ExpandProduct)
	{
		if(other.type == ExpandProduct)
		{
			return true;
		} else {
			return false;
		}
	}
	return false;
}

Product Product::expand()
{
	this->type = ExpandProduct;
	return *this;
}
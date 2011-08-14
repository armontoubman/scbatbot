#pragma once
#include <BWAPI.h>

enum ProductType
{
	NoProduct,
	BuildProduct,
	TechProduct,
	UpgradeProduct,
	ExpandProduct
};

class Product
{
public:
	BWAPI::UnitType buildtype;
	BWAPI::TechType techtype;
	BWAPI::UpgradeType upgradetype;
	ProductType type;

	Product();
	Product(BWAPI::UnitType t, int p);
	Product(BWAPI::TechType t, int p);
	Product(BWAPI::UpgradeType t, int p);

	int priority;
	int mineralPrice();
	int gasPrice();

	Product expand();

	int base;
	bool operator==(const Product& other) const;
};
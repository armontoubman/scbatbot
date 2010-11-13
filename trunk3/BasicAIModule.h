/****************************************************************************************************************//**
* Class acting as an interface between Starcraft and this AI modification.											*							
* Program:      Basic AI Module																						*
*																													*
* File:         BasicAIModule.h																						*
*																													*
* Function:     Connect the AI to Starcraft																			*
*																													*
* Description:  Program is called by Starcraft whenever something happens to which the AI can respond.				*
*				Usually it just forwards this information to our AI by calling some part of highCommand.			*
*																													*
* Author:       people from BWAPI, Armon Toubman, Ben Haanstra?, Torec Luik now as well haha (I wrote comments!)!	*
*																													*
* Environment:  ...																									*
*																													*
* Notes:        Program skeleton is taken from BWAPI http://code.google.com/p/bwapi/								*
*				See BasicAIModule.cpp.																				*
*																													*
* Revisions:    ...																									*
********************************************************************************************************************/


#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <Arbitrator.h>
#include <WorkerManager.h>
#include <SupplyManager.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
#include <TechManager.h>
#include <UpgradeManager.h>
#include <BaseManager.h>
#include <ScoutManager.h>
#include <DefenseManager.h>
#include <InformationManager.h>
#include <BorderManager.h>
#include <UnitGroupManager.h>
#include <EnhancedUI.h>

#include "HighCommand.h"

class BasicAIModule : public BWAPI::AIModule
{
public:
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onUnitDiscover(BWAPI::Unit* unit);
  virtual void onUnitEvade(BWAPI::Unit* unit);
  virtual void onUnitMorph(BWAPI::Unit* unit);
  virtual void onUnitRenegade(BWAPI::Unit* unit);
  virtual void onUnitDestroy(BWAPI::Unit* unit);
  virtual void onSendText(std::string text);
  ~BasicAIModule(); //not part of BWAPI::AIModule
  void showStats(); //not part of BWAPI::AIModule
  void showPlayers();
  void showForces();
  bool analyzed;
  std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
  Arbitrator::Arbitrator<BWAPI::Unit*,double> arbitrator;
  WorkerManager* workerManager;
  SupplyManager* supplyManager;
  BuildManager* buildManager;
  TechManager* techManager;
  UpgradeManager* upgradeManager;
  BaseManager* baseManager;
  ScoutManager* scoutManager;
  BuildOrderManager* buildOrderManager;
  DefenseManager* defenseManager;
  InformationManager* informationManager;
  BorderManager* borderManager;
  UnitGroupManager* unitGroupManager;
  EnhancedUI* enhancedUI;
  bool showManagerAssignments;

  HighCommand* highCommand;
};
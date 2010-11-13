/****************************************************************************************************************//**
* Class acting as an interface between Starcraft and this AI modification.											*							
* Program:      Basic AI Module																						*
*																													*
* File:         BasicAIModule.cpp																					*
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
*				See BasicAIModule.h.																				*
*																													*
* Revisions:    ...																									*
********************************************************************************************************************/
#include "BasicAIModule.h"
#include "Util.h"
using namespace BWAPI;
/**
* A virtual void.
* This function is called by Starcraft when a new game has started.
* It sets the initial settings for the AI to use:
* - cheat flags are enabled or disabled, 
* - the map is analysed by BWTA, 
* - highCommand is instantiated
* - variables concerning the races and workers of the AIs are recorded
*
* @see highCommand.cpp
* @see Addons/BWTA
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onStart()
{
  this->showManagerAssignments=false;
  if (Broodwar->isReplay()) return;
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);
  //Broodwar->enableFlag(Flag::CompleteMapInformation);
  BWTA::readMap();
  BWTA::analyze();
  this->analyzed=true;
  /*
  this->buildManager       = new BuildManager(&this->arbitrator);
  this->techManager        = new TechManager(&this->arbitrator);
  this->upgradeManager     = new UpgradeManager(&this->arbitrator);
  this->scoutManager       = new ScoutManager(&this->arbitrator);
  this->workerManager      = new WorkerManager(&this->arbitrator);
  this->supplyManager      = new SupplyManager();
  this->buildOrderManager  = new BuildOrderManager(this->buildManager,this->techManager,this->upgradeManager,this->workerManager,this->supplyManager);
  this->baseManager        = new BaseManager();
  this->defenseManager     = new DefenseManager(&this->arbitrator);
  this->informationManager = new InformationManager();
  this->borderManager      = new BorderManager();
  this->unitGroupManager   = new UnitGroupManager();
  this->enhancedUI         = new EnhancedUI();
  */

  this->highCommand = new HighCommand();
  //this->buildOrderManager->setDebugMode(true);

  /*
  this->supplyManager->setBuildManager(this->buildManager);
  this->supplyManager->setBuildOrderManager(this->buildOrderManager);
  this->techManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
  this->upgradeManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
  this->workerManager->setBaseManager(this->baseManager);
  this->workerManager->setBuildOrderManager(this->buildOrderManager);
  this->baseManager->setBuildOrderManager(this->buildOrderManager);
  this->borderManager->setInformationManager(this->informationManager);
  this->baseManager->setBorderManager(this->borderManager);
  this->defenseManager->setBorderManager(this->borderManager);
  */
  
  BWAPI::Race race = Broodwar->self()->getRace();
  BWAPI::Race enemyRace = Broodwar->enemy()->getRace();
  BWAPI::UnitType workerType=race.getWorker();
  
  //this->buildOrderManager->enableDependencyResolver();
  //this->workerManager->disableAutoBuild();
  
  if (race == Races::Zerg)
  {
    
  }
  else if (race == Races::Terran)
  {
    
  }
  else if (race == Races::Protoss)
  {
    
  }

}

/**
* A destructor.
* Frees up the memory taken by the created variables (highCommand here).
*/
BasicAIModule::~BasicAIModule()
{
  /*delete this->buildManager;
  delete this->techManager;
  delete this->upgradeManager;
  delete this->scoutManager;
  delete this->workerManager;
  delete this->supplyManager;
  delete this->buildOrderManager;
  delete this->baseManager;
  delete this->defenseManager;
  delete this->informationManager;
  delete this->borderManager;
  delete this->unitGroupManager;
  delete this->enhancedUI;*/

  delete this->highCommand;
}
/**
* A virtual void.
* Logs whether the AI has won the game or not.
* @param isWinner Whether the AI has won or not 
*/
void BasicAIModule::onEnd(bool isWinner)
{
  log("onEnd(%d)\n",isWinner);
}
/**
* A virtual void.
* This function is called by Starcraft on every new frame.
* It updates highCommand with the current units of our AI and the enemy's.
*
* @see highCommand.cpp
*
* Returns prematurely if the game a replay or if the map is not analyzed.
*/
void BasicAIModule::onFrame()
{
  if (Broodwar->isReplay()) return;
  if (!this->analyzed) return;
  /*//this->buildManager->update();
  this->buildOrderManager->update();
  this->baseManager->update();
  //this->workerManager->update();
  this->techManager->update();
  this->upgradeManager->update();
  //this->supplyManager->update();
  //this->scoutManager->update();
  this->enhancedUI->update();
  this->borderManager->update();
  //this->defenseManager->update();
  this->arbitrator.update();*/

  this->highCommand->update(Broodwar->self()->getUnits(), Broodwar->enemy()->getUnits());

  ///////////////////
  //std::set<Unit*> units=Broodwar->self()->getUnits();
  /*if (this->showManagerAssignments)
  {
    for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
      if (this->arbitrator.hasBid(*i))
      {
        int x=(*i)->getPosition().x();
        int y=(*i)->getPosition().y();
        std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > bids=this->arbitrator.getAllBidders(*i);
        int y_off=0;
        bool first = false;
        const char activeColor = '\x07', inactiveColor = '\x16';
        char color = activeColor;
        for(std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> >::iterator j=bids.begin();j!=bids.end();j++)
        {
          Broodwar->drawTextMap(x,y+y_off,"%c%s: %d",color,j->first->getShortName().c_str(),(int)j->second);
          y_off+=15;
          color = inactiveColor;
        }
      }
    }
  }*/
	///////////////////

  /*UnitGroup myPylonsAndGateways = SelectAll()(Pylon,Gateway)(HitPoints,"<=",200);
  for each(Unit* u in myPylonsAndGateways)
  {
    Broodwar->drawCircleMap(u->getPosition().x(),u->getPosition().y(),20,Colors::Red);
  }*/
}
/**
* A virtual void.
* This function is called by Starcraft whenever a unit is destroyed.
* It informs highCommand to remove the unit from the data.
*
* @param unit The unit that is destroyed in the game
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  /*this->arbitrator.onRemoveObject(unit);
  this->buildManager->onRemoveUnit(unit);
  this->techManager->onRemoveUnit(unit);
  this->upgradeManager->onRemoveUnit(unit);
  this->workerManager->onRemoveUnit(unit);
  this->scoutManager->onRemoveUnit(unit);
  this->defenseManager->onRemoveUnit(unit);
  this->informationManager->onUnitDestroy(unit);
  this->baseManager->onRemoveUnit(unit);*/

  this->highCommand->onRemoveUnit(unit);
}
/**
* A virtual void.
* This function is called by Starcraft whenever a unit is seen for the first time by our AI.
* It informs highCommand that the new unit has appeared.
*
* @param unit	The unit that has been discovered
* @see highCommand.cpp
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  /*this->informationManager->onUnitDiscover(unit);
  this->unitGroupManager->onUnitDiscover(unit);*/

  this->highCommand->onUnitShow(unit);
}
/**
* A virtual void.
* This does absolutely nothing at this moment.
*
* @param unit The unit that has evaded?
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onUnitEvade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  /*this->informationManager->onUnitEvade(unit);
  this->unitGroupManager->onUnitEvade(unit);*/
}
/**
* A virtual void.
* This function is called by Starcraft whenever a unit in the game has morphed.
* It informs highCommand that the new unit has appeared if it is ours.
*
* @param unit	The unit that has been morphed into
* @see highCommand.cpp
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onUnitMorph(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  //this->unitGroupManager->onUnitMorph(unit);

  if(unit->getPlayer() == BWAPI::Broodwar->self())
  {
	  this->highCommand->onUnitShow(unit);
  }
}
/**
* A virtual void.
* This does absolutely nothing at this moment.
*
* @param unit The unit that has renegaded?
*
* Returns prematurely if the game is a replay
*/
void BasicAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  //this->unitGroupManager->onUnitRenegade(unit);
}

/**
* A virtual void.
* This function is called upon when a text is inputted to Starcraft.
* It prints out the text sent and if it was "b" or "w" it will also make highCommand print out buildList or wantList.
*
* @param text	The text that has been inputted
* @see highCommand.cpp
*
* Returns prematurely if the game is a replay, still prints out the text though.
*/
void BasicAIModule::onSendText(std::string text)
{
  if (Broodwar->isReplay())
  {
    Broodwar->sendText("%s",text.c_str());
    return;
  }
  if(text=="b")
  {
	  this->highCommand->chatBuildList();
  }
  if(text=="w")
  {
	  this->highCommand->chatWantList();
  }
  /*UnitType type=UnitTypes::getUnitType(text);
  if (text=="debug")
  {
    if (this->showManagerAssignments==false)
    {
      this->showManagerAssignments=true;
      this->buildOrderManager->setDebugMode(true);
      this->scoutManager->setDebugMode(true);
    }
    else
    {
      this->showManagerAssignments=false;
      this->buildOrderManager->setDebugMode(false);
      this->scoutManager->setDebugMode(false);
    }
    Broodwar->printf("%s",text.c_str());
    return;
  }
  if (text=="expand")
  {
    this->baseManager->expand();
    Broodwar->printf("%s",text.c_str());
    return;
  }
  if (type!=UnitTypes::Unknown)
  {
    this->buildOrderManager->buildAdditional(1,type,300);
  }
  else
  {
    TechType type=TechTypes::getTechType(text);
    if (type!=TechTypes::Unknown)
    {
      this->techManager->research(type);
    }
    else
    {
      UpgradeType type=UpgradeTypes::getUpgradeType(text);
      if (type!=UpgradeTypes::Unknown)
      {
        this->upgradeManager->upgrade(type);
      }
      else
        Broodwar->printf("You typed '%s'!",text.c_str());
    }
  }*/
  Broodwar->sendText("%s",text.c_str());
}
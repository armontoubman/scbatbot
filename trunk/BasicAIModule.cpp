#include "BasicAIModule.h"
#include "Util.h"
using namespace BWAPI;

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
void BasicAIModule::onEnd(bool isWinner)
{
  log("onEnd(%d)\n",isWinner);
}
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

void BasicAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  /*this->informationManager->onUnitDiscover(unit);
  this->unitGroupManager->onUnitDiscover(unit);*/

  this->highCommand->onUnitShow(unit);
}
void BasicAIModule::onUnitEvade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  /*this->informationManager->onUnitEvade(unit);
  this->unitGroupManager->onUnitEvade(unit);*/
}

void BasicAIModule::onUnitMorph(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  //this->unitGroupManager->onUnitMorph(unit);

  if(unit->getPlayer() == BWAPI::Broodwar->self())
  {
	  this->highCommand->onUnitShow(unit);
  }
}
void BasicAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  //this->unitGroupManager->onUnitRenegade(unit);
}

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
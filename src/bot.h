#ifndef __BOT_H
#define __BOT_H

#include <iostream>
#include <sc2api/sc2_api.h>
#include "ManagerBase.h"
#include "ModuleManager.h"
#include "StrategyManager.h"
#include "SupplyManager.h"
#include "EconManager.h"
#include "ArmyManager.h"
#include "ConstructionManager.h"
#include "StructuresManager.h"
#include "UpgradesManager.h"
#include "DrawingManager.h"
#include "BaseLocationManager.h"
#include "BuildQueueManager.h"
#include "DataManager.h"
#include "MorphManager.h"
#include "MapManager.h"
#include "Utils.h"


class Bot : public sc2::Agent {
private:
	ModuleManager moduleManager;
	StrategyManager strategyManager;
	SupplyManager supplyManager;
	EconManager econManager;
	ArmyManager armyManager;
	ConstructionManager constructionManager;
	StructuresManager structuresManager;
	UpgradesManager upgradesManager;
	DrawingManager drawingManager;
	BaseLocationManager baseLocationManager;
	BuildQueueManager buildQueueManager;
	DataManager dataManager;
	MorphManager morphManager;
	MapManager mapManager;

	std::vector<ManagerBase*> managers;

public:
	Bot();
	~Bot();

	//////////////////////////////////////////////
	//  Access to Manager Classes				//
	//////////////////////////////////////////////
	ModuleManager & Modules();
	ConstructionManager & Construction();
	StructuresManager & Structures();
	SupplyManager & Supply();
	ArmyManager & Army();
	EconManager & Econ();
	DrawingManager & Draw();
	BaseLocationManager & BaseLocations();
	UpgradesManager & Upgrades();
	BuildQueueManager & BuildQueue();
	DataManager & Data();
	MorphManager & Morph();
	MapManager & Map();


	std::string GetVersion();


	//Module System



	//////////////////////////////////////////////
	//Everything in ClientEvents, with comments //
	//////////////////////////////////////////////

	/////////////////////////////////////////////
	// Full game level, not passed to managers //
	/////////////////////////////////////////////
	//! Called when a game is started after a load. Fast restarting will not call this.
	virtual void OnGameFullStart();
	//! Called when a game is started or restarted.
	virtual void OnGameStart();
	//! Called when a game has ended.
	virtual void OnGameEnd();
	//! Called for various errors the library can encounter. See ClientError enum for possible errors.
	virtual void OnError(const std::vector<sc2::ClientError>& client_errors, const std::vector<std::string>& protocol_errors = {});

	////////////////////////////////////////
	// In game events, passed to managers //
	////////////////////////////////////////

	//! In non realtime games this function gets called after each step as indicated by step size.
	//! In realtime this function gets called as often as possible after request/responses are received from the game gathering observation state.
	virtual void OnStep();
	//! Called whenever one of the player's units has been destroyed.
	//!< \param unit The destroyed unit.
	virtual void OnUnitDestroyed(const sc2::Unit* unit);
	//! Called when a Unit has been created by the player.
	//!< \param unit The created unit.
	virtual void OnUnitCreated(const sc2::Unit* unit);
	//! Called when a unit becomes idle, this will only occur as an event so will only be called when the unit becomes
	//! idle and not a second time. Being idle is defined by having orders in the previous step and not currently having
	//! orders or if it did not exist in the previous step and now does, a unit being created, for instance, will call both
	//! OnUnitCreated and OnUnitIdle if it does not have a rally set.
	//!< \param unit The idle unit.
	virtual void OnUnitIdle(const sc2::Unit* unit);
	//! Called when an upgrade is finished, warp gate, ground weapons, baneling speed, etc.
	//!< \param upgrade The completed upgrade.
	virtual void OnUpgradeCompleted(sc2::UpgradeID upgradeID);
	//! Called when the unit in the previous step had a build progress less than 1.0 but is greater than or equal to 1.0 in the current step.
	//!< \param unit The constructed unit.
	virtual void OnBuildingConstructionComplete(const sc2::Unit* unit);
	//! Called when a nydus is placed.
	virtual void OnNydusDetected();
	//! Called when a nuclear launch is detected.
	virtual void OnNuclearLaunchDetected();
	//! Called when an enemy unit enters vision from out of fog of war.
	//!< \param unit The unit entering vision.
	virtual void OnUnitEnterVision(const sc2::Unit* unit);


private:
	//If a single step takes over this threshold, print out a warning.
	//	85ms comes from the student AI competition rules settings.
	const int64_t stepWarningThresholdMs = 85;

	//Track the last 20 loops then show the avg
	int64_t last20GameLoopsTotalTimeMs;
	int64_t last20GameLoopsAvgTimeMs;
};

#define DllExport   __declspec( dllexport )

// Returns a pointer to a class deriving from sc2::Agent
void DllExport *CreateNewAgent(void);

// Returns a string identifier for the agent name
const char  DllExport *GetAgentName(void);

// Returns the agents prefered race.  should be sc2::Race cast to int.
int DllExport GetAgentRace(void);

#endif //__BOT_H

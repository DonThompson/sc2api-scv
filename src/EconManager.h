#ifndef __ECON_MANAGER_H
#define __ECON_MANAGER_H

#include <iostream>
#include <sc2api/sc2_api.h>
#include "ManagerBase.h"
#include "Utils.h"
#include "Structure.h"

class Bot;
class ModuleBase;
class VespeneWorkerBalanceModule;
class IdleWorkerModule;

//Behaviors
//	Always
//		* VespeneWorkerBalanceModule
//		* Idle workers moved to nearest minerals 

//	Autonomy Enabled



//	Enabled:  Builds workers if we have not saturated the minerals & gas fully.  Builds refineries if missing.  Shifts workers between the two.  And everything in disabled.
//	Disabled:  Handles idle workers and puts them back to mining.
class EconManager : public ManagerBase {
private:
	clock_t lastBalanceClock;

public:
	EconManager(Bot & b);
	virtual void OnStep();
	virtual void OnGameStart();
	virtual void OnUnitIdle(const sc2::Unit* unit);
	//TODO:  Get modules in base and don't mess with this stuff
	//virtual void OnUnitDestroyed(const sc2::Unit* unit);
	//virtual void OnUnitCreated(const sc2::Unit* unit);
	//virtual void OnUpgradeCompleted(sc2::UpgradeID upgradeID);
	//virtual void OnBuildingConstructionComplete(const sc2::Unit* unit);
	//virtual void OnUnitEnterVision(const sc2::Unit* unit);
	//virtual void OnNydusDetected();
	//virtual void OnNuclearLaunchDetected();

	void OnRefinerySuccess(int64_t taskId);
	void OnRefineryFailed(int64_t taskId);

	static const sc2::Unit* FindNearestVespeneGeyser(const sc2::Point2D& start, const sc2::ObservationInterface* obs);

	//Train a worker.  Optionally provide a source to build from.  If not provided, one will be found from the most suitable base.
	bool TrainWorker(Structure* buildFrom = nullptr);

private:
	void BalanceBuilders();
	void BalanceGasWorkers();
	bool NeedRefinery();
	int32_t GetRefineryCount();
	void BuildRefinery();
	
	const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
	void HandleCommandCenterIdle(Structure cc);

	uint32_t refineriesInProgress;
	uint32_t refineriesCompleted;


	//Included modules
public:
	void InitializeModules();	//TODO:  Base class?
private:
	std::vector<std::shared_ptr<ModuleBase>> gameStartNotifications;		//TODO:  Base class
	std::vector<std::shared_ptr<ModuleBase>> unitIdleNotifications;		//TODO:  Base class
	std::vector<std::shared_ptr<ModuleBase>> unitCreateNotifications;		//TODO:  Base class
	std::vector<std::shared_ptr<ModuleBase>> unitDestroyNotifications;		//TODO:  Base class
	std::map<std::shared_ptr<ModuleBase>, uint32_t> stepLoopNotificationMap;	//TODO:  Base class

	std::shared_ptr<VespeneWorkerBalanceModule> vespeneWorkerBalanceModule;
	std::shared_ptr<IdleWorkerModule> idleWorkerModule;
};

#endif //__ECON_MANAGER_H

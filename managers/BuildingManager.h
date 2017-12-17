#pragma once

#include <iostream>
#include <sc2api/sc2_api.h>
using namespace sc2;
#include "ManagerBase.h"
#include "../utils/Utils.h"
#include "../BuildQueueTask.h"

class Bot;

/* Usage
	BuildingManager currently will attempt to create your building, but will abort the task for any invalid build
	attempts (not enough resources, invalid position, etc.).  If this happens, it will never re-attempt.  caller must do so.

	TODO:  callback event on success
	callback on fail
	never lose a request
	Logging/status:  Output status of the queue every few seconds.

	Progress:
	[DONE] - callback functions (done.  tested as null)
	[DONE] - test callbacks non-null
	[DONE] - fix the step to go backwards instead of aborting.  remove this failure callback.  Note:  Current code will probably then be queued to build hundreds of depots.
	[DONE] - put in some kind of counter - can't bounce around forever, it needs to eventually time out
	[DONE]- callback on that cancel
	[DONE]- test that case somehow
	- go fix econ (refineries), supply (depots), and army (rax) managers to not spam so many commands and to implement callbacks
	- logging work
	- document, document, document.
	- refactor and cleanup
	- this should put buildmanager in a good place going forward for a while
	- rename to construction manager?  ongoing confusion between "constructing buildings" manager and manager of "existing buildings"  construction/structure instead?  bot.Construction() and bot.Structures().  I am liking these.
	- TODO:  maybe put a cap on how many items in the queue get processed?  the game really starts to slow down at least at 100, and at 300 it's barely moving.  safety check.
	- TODO:  HandleConfirmingOrders complexity is getting really high.  HandleWaitingOnBuildStart isn't far behind.
	- TODO:  HandleWaitingOnBuildStart -- In my test game, by loop ~4700 (first attack) I had 29 requests in the queue that were in this state, but none of the builders were actually trying to build any longer.  How'd we get here and how do we purge these reasonably?  Maybe the test for expiration should be outside of only HandleConfirmingOrders?
*/

class BuildingManager : public ManagerBase
{
public:
	BuildingManager(Bot & b);
	~BuildingManager();

	int64_t BuildStructure(ABILITY_ID ability_type_for_structure, BuildQueueTaskCallbackFunction callbackSuccess = nullptr, BuildQueueTaskCallbackFunction callbackFailure = nullptr);

	virtual void OnStep();

private:
	int64_t nextBuildingId;
	std::map<int64_t, BuildQueueTask> mapBuildingQueue;
	int64_t UseNextIdentifier();


	//Queue management functions
	void HandledQueuedBuilding(BuildQueueTask &task);
	void HandleFindingPosition(BuildQueueTask &task);
	void HandleIssuingBuild(BuildQueueTask &task);
	void HandleConfirmingOrders(BuildQueueTask &task, std::vector<int64_t> &tasksToRemove, const int64_t taskId);
	void HandleWaitingOnBuildStart(BuildQueueTask &task);
	void HandleConstructionInProgress(BuildQueueTask &task);
	void HandleCompleted(BuildQueueTask task, std::vector<int64_t> &tasksToRemove, const int64_t taskId);

	const Unit* HandleFindingRefineryTarget(Point2D builderPos);

};

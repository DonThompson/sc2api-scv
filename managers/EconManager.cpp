#include "EconManager.h"

EconManager::EconManager()
{
	lastBalanceClock = clock();
}

EconManager::~EconManager()
{
}

void EconManager::OnStep()
{
	if (NeedRefinery()) {
		TryBuildRefinery();
	}

	//Rebalance workers every few seconds.  Some odd timing issues can happen if we go every step
	const clock_t rebalanceTime = CLOCKS_PER_SEC * 2;   //2 seconds
	if (clock() - lastBalanceClock > rebalanceTime) {
		BalanceBuilders();
		lastBalanceClock = clock();
	}
}

void EconManager::OnUnitIdle(const Unit* unit)
{
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER:     OnCommandCenterIdle(unit);      break;
	case UNIT_TYPEID::TERRAN_SCV: {
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (mineral_target == nullptr) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}
	default: {
		break;
	}
	}
}

void EconManager::BalanceBuilders()
{
	//Version 1:  SIMPLE.  If we have a refinery < max, assign there.  Otherwise, assign to minerals.
	Units refineries = Utils::GetOwnUnits(Observation(), UNIT_TYPEID::TERRAN_REFINERY);
	for (const Unit* r : refineries) {
		if (r->build_progress >= 1.0f && r->assigned_harvesters < r->ideal_harvesters) {
			std::cout << "Moving harvester to gas refinery.  Assigned:  " << r->assigned_harvesters << ".  Ideal:  " << r->ideal_harvesters << std::endl;
			const Unit* unit = Utils::GetRandomHarvester(Observation());
			Actions()->UnitCommand(unit, ABILITY_ID::SMART, r);
		}
	}

	//Make sure command centers have enough units - we might have just stolen some to bring them below threshold
	Units cc = Utils::GetOwnUnits(Observation(), UNIT_TYPEID::TERRAN_COMMANDCENTER);
	for (const Unit* u : cc) {
		//Just call the idle function, it'll quit if not needed
		if (u->orders.size() == 0) {
			OnCommandCenterIdle(u);
		}
	}
}

void EconManager::OnCommandCenterIdle(const Unit* unit)
{
	//Only build if we're short harvesters
	bool buildSCV = false;

	if (unit->assigned_harvesters < unit->ideal_harvesters) {
		buildSCV = true;
	}

	//Or if we're short gas harvesters
	Units refineries = Utils::GetOwnUnits(Observation(), UNIT_TYPEID::TERRAN_REFINERY);
	for (const Unit* r : refineries) {
		if (r->assigned_harvesters < r->ideal_harvesters) {
			buildSCV = true;
		}
	}

	if (buildSCV) {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
	}
}

//TODO:  Hardcoded
bool EconManager::NeedRefinery()
{
	const ObservationInterface* observation = Observation();
	ActionInterface* actions = Actions();

	int32_t supplyUsed = observation->GetFoodUsed();

	//Don't build the first until 18 supply.
	if (supplyUsed >= 18 && GetRefineryCount() < 1) {
		return true;
	}

	//Build the second around 22
	if (supplyUsed >= 22 && GetRefineryCount() < 2) {
		return true;
	}

	return false;
}

//Counts building and built
int32_t EconManager::GetRefineryCount()
{
	return Utils::CountOwnUnits(Observation(), UNIT_TYPEID::TERRAN_REFINERY);
}

bool EconManager::TryBuildRefinery()
{
	const ObservationInterface* observation = Observation();
	ActionInterface* actions = Actions();

	//Get a builder to work with
	const Unit* builder = Utils::GetRandomHarvester(observation);
	if (builder == nullptr) {
		return false;
	}

	const Unit* vespeneGeyser = FindNearestVespeneGeyser(builder->pos);
	if (vespeneGeyser == nullptr) {
		return false;
	}

	//Geyers builds are actioned by tag, not by point
	actions->UnitCommand(builder, ABILITY_ID::BUILD_REFINERY, vespeneGeyser);

	return true;
}

const Unit* EconManager::FindNearestVespeneGeyser(const Point2D& start)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

const Unit* EconManager::FindNearestMineralPatch(const Point2D& start)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

#include "RLAIModule.h"
#include <iostream>
#include "rl/ControlAlgorithm.h"
#include "rl/RL.h"

using namespace RLLib;
using namespace BWAPI;
using namespace Filter;


RLProblem <double> * problem;


void RLAIModule::onStart()
{
	// Hello World!
	Broodwar->sendText("Hello world!");

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if (Broodwar->isReplay())
	{

		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;

		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
		{
			// Only print the player if they are not an observer
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}

	}
	else // if this is not a replay
	{
		if (!unit_problem){
			this->unit_problem = new UnitProblem<double>();
			this->reinforcement = new ReinforcementLearning(this->unit_problem);
		}
		else{
			
		}
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}



}

void RLAIModule::onEnd(bool isWinner)
{
	if (unit_problem){
		//delete unit_problem;
	}
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!	
	}else{
		
	}
}

void RLAIModule::onFrame()
{
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() == 0){
		// Controles
		// Arm controller
		// Iterate through all the units that we own
		unitControl();
	}
}

void RLAIModule::
unitControl(){
	for (auto &u : Broodwar->self()->getUnits()){
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;

		if (u->getType() == UnitTypes::Terran_Vulture){
			vulture(u);
		}

		// Finally make the unit do some stuff!
		if (u->getType() == UnitTypes::Protoss_Dragoon){
			dragoons(u);			
		}
		if (u->getType() == UnitTypes::Protoss_Zealot)
			zealots(u);
		if (u->getType() == UnitTypes::Zerg_Mutalisk)
			muta(u);

	}
}

void RLAIModule::
vulture(BWAPI::Unit unit){
	
	unit_experiment = unit;
	if (unit_problem){
		this->unit_problem->setUnit(unit_experiment);
		if (reinforcement) {
			reinforcement->run();
		}
	}
}

void RLAIModule::
onUnitDiscover(BWAPI::Unit unit)
{}

void RLAIModule::
onUnitEvade(BWAPI::Unit unit)
{
}


void RLAIModule::
dragoons(BWAPI::Unit unit)
{
	unit_experiment = unit;
	if (unit_problem){
		this->unit_problem->setUnit(unit_experiment);
		if (reinforcement) {
			reinforcement->run();
		}
	}
}

void RLAIModule::
muta(BWAPI::Unit unit){
	unit_experiment = unit;
	if (unit_problem){
		this->unit_problem->setUnit(unit_experiment);
		if (reinforcement) {
			reinforcement->run();
		}
	}
}

void RLAIModule::
zealots(BWAPI::Unit unit){
	unit_experiment = unit;
	if (unit_problem){
		this->unit_problem->setUnit(unit_experiment);
		if (reinforcement) {
			reinforcement->run();
		}
	}
}

void RLAIModule::
onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void RLAIModule::
onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void RLAIModule::
onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void RLAIModule::
onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void RLAIModule::
onUnitShow(BWAPI::Unit unit)
{
}

void RLAIModule::
onUnitHide(BWAPI::Unit unit)
{
}

void RLAIModule::
onUnitCreate(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void RLAIModule::
onUnitDestroy(BWAPI::Unit unit)
{

}

void RLAIModule::
onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void RLAIModule::
onUnitRenegade(BWAPI::Unit unit)
{
}

void RLAIModule::
onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void RLAIModule::
onUnitComplete(BWAPI::Unit unit)
{
}

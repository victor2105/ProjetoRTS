#pragma once

#include <BWAPI.h>
#include "rl/ControlAlgorithm.h"
#include "rl/RL.h"
#include "rl/Action.h"
#include <iostream>


/*
*		Actions: Discrete
*			0 - Fight
*			1 - Reatreat
*		State: Discrete
*			weapon:
*				0 - in cooldown
*				1 - active
*			distanceCE:
*				0 - x <= 25%
*				1 - 25 < x <= 75%
*				2 - 75 < x <= 120 
*				3 - > 120%
*			numberEUR: int
*			health: int
*
*/

/**/


using namespace RLLib;
using namespace BWAPI;

/*
#define FIGHT 0
#define REATREAT 1
*/

template < class T >
class UnitProblem : public RLProblem<T> // this class will observe a unit
{
	typedef RLProblem<T> Base;
protected:

	int weapon;	// Weapon Cooldown (is in cooldown or not/ 1 or 0)
	int distanceCE;	// Distance to Closest Enemy
	int numberEUR;	// Number of Enemy Units in Range
	int hp;			// Hit point - Health classified into one of {25,50,75,100}%
	int last_enemy_units_health;
	int last_agent_health;

	int n_enemy_health;
	int l_enemy_health;
	int n_agent_health;
	int l_agent_health;

	BWAPI::Unit unit_experiment;

public:
	UnitProblem(Random<T> * random = 0, BWAPI::Unit u = 0):
		RLProblem<T>(random, 4, 2, 1), weapon(0), distanceCE(0),
		numberEUR(0), hp(0), unit_experiment(u)
	{
		for (int i = 0; i < Base::discreteActions->dimension(); i++)
			Base::discreteActions->push_back(i, i);
		// Not used
		Base::discreteActions->push_back(0, 0.0);
		

	}
	
	~UnitProblem(){}
	
	// Broodwar
	void initialize(){
		if (unit_experiment){
			if (unit_experiment->exists()){
				Broodwar->sendText("Inicialize Unit Ex periment", this->weapon);
				// weapon
				this->hp = unit_experiment->getInitialHitPoints();
				Broodwar->sendText("hp %d", this->hp);
				this->numberEUR = 0;
				// take numberEUR
				Broodwar->sendText("numberEUR %d", this->numberEUR);
				//this->distanceCE = unit_experiment->getClosestUnit()->getDistance(unit_experiment->getPosition());
				Broodwar->sendText("distenceCE %d", this->distanceCE);
			}
		}
	}

	void step(const Action<T>* a){
		update_actions(a);
	}

	void update(){}

	bool endOfEpisode() const {
		if (unit_experiment){
			if (unit_experiment->getHitPoints() == 0) return true;
			if (unit_experiment->getKillCount() != 0) return true;
		}
		return false; // all killed or we die
	}

	T r() const
	{
		Broodwar->drawTextScreen(100, 0, "rewardR %d", 0);
		return rewardFunction();
	}

	T z() const
	{

		Broodwar->drawTextScreen(100, 20, "rewardZ %d", 0);
		return 0;
	}

	void setUnit(BWAPI::Unit unit){
		unit_experiment = unit;
		initialize();
	}

private:

	void update_actions(const Action<T>* act) {
		
		if (act->id() == 0){
			fight();
		}
		else {
			reatreat();
		}
	}

	void updateTRStep()
	{
		//bool w = unit_experiment->canAttack();
		//if (w) weapon = 1;
		//else weapon = 0;
		
		//Broodwar->drawTextScreen(100, 20, "%d", numberEUR);
		//Broodwar->drawTextScreen(100, 40, "%d", weapon);
		Base::output->o_tp1->setEntry(0, weapon);
		Base::output->o_tp1->setEntry(1, distanceCE);
		Base::output->o_tp1->setEntry(2, numberEUR);
		Base::output->o_tp1->setEntry(3, hp);

		Base::output->observation_tp1->setEntry(0, weapon);
		Base::output->observation_tp1->setEntry(1, distanceCE);
		Base::output->observation_tp1->setEntry(2, numberEUR);
		Base::output->observation_tp1->setEntry(3, hp);
	}

	int rewardFunction() const{
		return l_enemy_health - n_enemy_health - (l_agent_health - n_agent_health);
	}

	int enemy_health_function(){
		return 0;
	}
	
	void fight() {
		if (unit_experiment){
			Unit u = unit_experiment;
			Unitset enemies = unit_experiment->getUnitsInRadius(200, BWAPI::Filter::IsEnemy);
			Unit closestEnemy = nullptr;

			for (auto &m : enemies)
			{
				if (!closestEnemy || u->getDistance(m) < u->getDistance(closestEnemy))
					closestEnemy = m;
			}
			if (closestEnemy){
				Broodwar->drawCircleMap(closestEnemy->getPosition() , 15, BWAPI::Colors::Red, true);
				u->rightClick(closestEnemy);
			}
			else{
				Position p = unit_experiment->getPosition();
				p += Position(64, 0);
				Broodwar->drawCircleMap(p, 15, BWAPI::Colors::Red, true);
				unit_experiment->attack(p);
			}
		}
	}
	
	void reatreat() {
		if (unit_experiment){
			Unit u = unit_experiment;
			Unitset enemies = unit_experiment->getUnitsInRadius(150, BWAPI::Filter::IsEnemy);
			Unit closestEnemy = nullptr;

			for (auto &m : enemies)
			{
				if (!closestEnemy || u->getDistance(m) < u->getDistance(closestEnemy))
					closestEnemy = m;
			}
			if (closestEnemy){
				Position p = unit_experiment->getPosition();
				p -= closestEnemy->getPosition() - p;
				Broodwar->drawCircleMap(p, 15, BWAPI::Colors::Blue, true);
				unit_experiment->move(p);
			}else{
				Position p = unit_experiment->getPosition();
				p -= Position(64, 0);
				Broodwar->drawCircleMap(p, 15, BWAPI::Colors::Blue, true);
				unit_experiment->move(p);
			}			
		}
	}
};

/**/
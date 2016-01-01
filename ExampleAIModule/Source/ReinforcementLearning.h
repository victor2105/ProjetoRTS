#pragma once
/**/

#include "UnitProblem.h"
#include "rl/ControlAlgorithm.h"
#include "rl/RL.h"


using namespace RLLib;

class ReinforcementLearning
{
public:
	ReinforcementLearning(UnitProblem <double> * problem);
	~ReinforcementLearning();

	void run();
	void end();
	void evaluate();
private:

	Random <double> * random;
	//RP Problem
	RLProblem <double> * problem;
	// Projector
	Hashing <double> * hashing;
	Projector <double> * projector;
	StateToStateAction <double> * toStateAction;
	// Predictor
	Sarsa<double> * sarsaAdaptive;
	Trace<double> * e;
	double gamma;
	double lambda;
	double epsilon;
	Policy<double>* acting;

	// Controller
	OnPolicyControlLearner<double>* control;

	// Runner
	RLAgent<double> * agent;
	RLRunner <double> * sim;
};

/**/
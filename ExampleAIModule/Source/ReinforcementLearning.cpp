
#include "ReinforcementLearning.h"

/**/
ReinforcementLearning::ReinforcementLearning(UnitProblem <double> * nproblem)
{
	random = new Random < double > ;
	problem = nproblem;
	hashing = new UNH<double>(random, 10000);
	projector = new TileCoderHashing<double>(hashing, problem->dimension(), 9, 10,
		false); // 10, 10, true
	toStateAction = new StateActionTilings<double>(projector, problem->getDiscreteActions());
	e = new ATrace<double>(projector->dimension());
	gamma = 0.99;
	lambda = 0.3;
	sarsaAdaptive = new SarsaAlphaBound<double>(1.0f, gamma, lambda, e);//new SarsaAlphaBound<double>(1.0f, gamma, lambda, e);
	epsilon = 0.01;
	acting = new EpsilonGreedy<double>(random, problem->getDiscreteActions(), sarsaAdaptive, epsilon);
	control = new SarsaControl<double>(acting, toStateAction, sarsaAdaptive);

	agent = new LearnerAgent<double>(control);
	sim = new RLRunner<double>(agent, problem, 5000, 300, 1);

}


ReinforcementLearning::~ReinforcementLearning()
{

	delete random;
	delete problem;
	delete hashing;
	delete projector;
	delete toStateAction;
	delete e;
	delete sarsaAdaptive;
	delete acting;
	delete control;
	delete agent;
	delete sim;
}

void ReinforcementLearning::evaluate(){
	
}

void ReinforcementLearning::run(){
	sim->step();
}

void ReinforcementLearning::end(){
	
	control->persist("UnitProblem.txt");
	sim->computeValueFunction();
}
/**/
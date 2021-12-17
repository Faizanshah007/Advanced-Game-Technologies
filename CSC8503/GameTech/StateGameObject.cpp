#include "StateGameObject.h"
#include "..\CSC8503Common\StateTransition.h"
#include "..\CSC8503Common\StateMachine.h"
#include "..\CSC8503Common\State.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(const string& objName) {
	name = objName;
	counter = 0.0f;
	oscillatingFreq = 3.0f;

	force = Vector3(0, 0, 30);

	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->state = "Moving Front";
			this->MoveFront(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->state = "Moving Back";
			this->MoveBack(dt);
		}
	);

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{
			return this->counter > this->GetOscillatingFreq();
		}
	));

	stateMachine->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->counter < -(this->GetOscillatingFreq());
		}
	));
}

StateGameObject ::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveFront(float dt) {
	GetPhysicsObject()->AddForce(force * Vector3(0, 0, -1));
	counter += dt;
}

void StateGameObject::MoveBack(float dt) {
	GetPhysicsObject()->AddForce(force * Vector3(0, 0, 1));
	counter -= dt;
}
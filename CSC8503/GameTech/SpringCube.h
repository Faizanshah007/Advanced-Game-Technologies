#pragma once
#include "../GameTech/StateGameObject.h"
#include "..\CSC8503Common\StateTransition.h"
#include "..\CSC8503Common\StateMachine.h"
#include "..\CSC8503Common\State.h"

namespace NCL {
	namespace CSC8503 {
		class SpringCube : public GameObject {
		public:
			Vector3 origin1, origin2;
			SpringCube(Vector3 wantedOrigin1, Vector3 wantedOrigin2, float springConstant, const string& objName = "SrpingCube") {
				name = objName;
				k = springConstant;
				origin1 = wantedOrigin1;
				origin2 = wantedOrigin2;
				isOn = false;

				stateMachine = new StateMachine();

				State* onState = new State([&](float dt)-> void
					{
						this->state = "on";
						this->Oscillate(dt, this->origin1);
					}
				);
				State* offState = new State([&](float dt)-> void
					{
						this->state = "off";
						this->Oscillate(dt, this->origin2);
					}
				);

				stateMachine->AddState(offState);
				stateMachine->AddState(onState);

				stateMachine->AddTransition(new StateTransition(onState, offState,
					[&]()-> bool
					{
						//std::cout << "on to off" << std::endl;
						return !(this->GetState());
					}
				));

				stateMachine->AddTransition(new StateTransition(offState, onState,
					[&]()-> bool
					{
						//std::cout << "off to on" << std::endl;
						return this->GetState();
					}
				));
			}
			~SpringCube() {};

			virtual void Update(float dt) {
				stateMachine->Update(dt);
			}

			bool GetState() const { return isOn; }

			void TurnOn() { isOn = true; }
			void TurnOff() { isOn = false; }

		protected:
			void Oscillate(float dt, Vector3 origin) {
				Vector3 currentLoc = GetTransform().GetPosition();
				Vector3 force = (currentLoc - origin) * -k;
				GetPhysicsObject()->AddForce(force);
				/*if (force.Length() > 0)
				Debug::DrawLine(origin, force, Debug::RED, 1.0f);*/
			}

			StateMachine* stateMachine;

			float k;
			bool isOn;
		};
	}
}
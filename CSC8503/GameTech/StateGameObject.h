#pragma once
#include "../CSC8503Common/GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class StateGameObject : public GameObject {
		public:
			StateGameObject(const string& objName);
			~StateGameObject();

			virtual void Update(float dt);

			void SetOscillatingFreq(float freq) {
				oscillatingFreq = freq;
			}

			float GetOscillatingFreq() const {
				return oscillatingFreq;
			}

			void SetSystemForce(Vector3 f) {
				force = f;
			}

		protected:
			void MoveFront(float dt);
			void MoveBack(float dt);

			StateMachine* stateMachine;
			float counter;
			float oscillatingFreq;
			Vector3 force;
		};
	}
}
#pragma once
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

class Coin : public GameObject
{
public:
	Coin() : GameObject("Coin", false) {

	}
	~Coin() {}
protected:
	void OnCollisionBegin(GameObject* otherObject) override {
		if (otherObject->GetName() == "playerBall") {
			otherObject->specialValue += 10;
			this->Deactivate();
		}
	}


};


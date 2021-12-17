#pragma once
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

class WinningPad : public GameObject
{
public:
	WinningPad() : GameObject("WinningPad", false) {
		specialValue = 0;
	}
	~WinningPad() {}
protected:
	void OnCollisionBegin(GameObject* otherObject) override {
		specialValue = 1;
	}

};


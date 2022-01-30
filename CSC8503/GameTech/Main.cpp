#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

using namespace NCL;
using namespace CSC8503;

Window* w;
TutorialGame* g;

int reset;

enum MyMenuOptions
{
	lvl1,
	lvl2,
	quit,
};

int currentState = lvl1;
int menuState = lvl1;

class WinLoseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		Debug::Print(gameState, Vector2(50, 50), Debug::CYAN);
		Debug::Print("Score: " + std::to_string(score), Vector2(50, 60), Debug::CYAN);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		g->UpdateGame(dt, -1);
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		gameState = g->gameState;
		score = g->score;
		g = new TutorialGame(-1);
	}
protected:
	string gameState;
	float score;
};

class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {

		if (reset == 1) {
			*newState = new WinLoseScreen();
			reset = 0;
			return PushdownResult::Push;
		}
		
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			if (reset == -1)
				return PushdownResult::Pop;
			else
				return PushdownResult::NoChange;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
			currentState = menuState;
			switch (menuState)
			{
			case lvl1: g = new TutorialGame(); return PushdownResult::Pop; break;
			case lvl2: g = new TutorialGame(2); return PushdownResult::Pop; break;
			case quit:	return PushdownResult::Pop; break;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
			menuState = (menuState + 1) % 3;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) {
			menuState = (menuState - 1);
			if (menuState < 0) menuState += 3;
		}
		if(reset == -1)	Debug::Print("Press esc to continue...", Vector2(30, 30));
		switch (menuState) {
		case lvl1: Debug::Print("Start Level1  <-------------", Vector2(30, 50), Vector3(1.0f,0.5f,0.5f)); Debug::Print("Start Level2", Vector2(30, 70)); Debug::Print("Quit", Vector2(30, 90)); break;
		case lvl2: Debug::Print("Start Level1", Vector2(30, 50)); Debug::Print("Start Level2  <-------------", Vector2(30, 70), Vector3(1.0f, 0.5f, 0.5f)); Debug::Print("Quit", Vector2(30, 90)); break;
		case quit: Debug::Print("Start Level1", Vector2(30, 50)); Debug::Print("Start Level2", Vector2(30, 70)); Debug::Print("Quit    <-------------", Vector2(30, 90), Vector3(1.0f, 0.5f, 0.5f)); break;
		}
		g->UpdateGame(dt, -1);
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		menuState = lvl1;
		//std::cout << "Press U to unpause game!\n";
	}
};

class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		if (currentState == quit) {
			return PushdownResult::Pop;
		}

		if (g->gameState == "Won" || g->gameState == "Lost") {
			//*newState = new WinLoseScreen();
			*newState = new PauseScreen();
			reset = 1;
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			g->togglePlaneMagic();
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
		g->UpdateGame(dt, currentState);

		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		std::cout << "Back to game.!\n";
		reset = -1;
	}
};

//vector <Vector3 > testNodes;
//void TestPathfinding() {
//	NavigationGrid grid("TestGrid1.txt");
//
//	NavigationPath outPath;
//
//	Vector3 startPos(80, 0, 10);
//	Vector3 endPos(80, 0, 80);
//
//	bool found = grid.FindPath(startPos, endPos, outPath);
//
//	Vector3 pos;
//	while (outPath.PopWaypoint(pos)) {
//		testNodes.push_back(pos);
//	}
//}
//void DisplayPathfinding() {
//	for (int i = 1; i < testNodes.size(); ++i) {
//		Vector3 a = testNodes[i - 1];
//		Vector3 b = testNodes[i];
//
//		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
//	}
//}

//class IntroScreen : public PushdownState {
//	PushdownResult OnUpdate(float dt,
//		PushdownState** newState) override {
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
//			*newState = new GameScreen();
//			return PushdownResult::Push;
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
//			return PushdownResult::Pop;
//		}
//		return PushdownResult::NoChange;
//	};
//
//	void OnAwake() override {
//		std::cout << "Welcome to a really awesome game!\n";
//		std::cout << "Press Space To Begin or escape to quit!\n";
//	}
//};

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	g = new TutorialGame();
	PushdownMachine machine(new GameScreen());

	//TestPathfinding();

	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow()){// && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}

		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			g->togglePlaneMagic();
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));*/
		
		if (!machine.Update(dt)) {
			return 0;
		}
		//if (currentState == 1) DisplayPathfinding();
		//g->UpdateGame(dt);
		//TestStateMachine();
	}
	Window::DestroyGameWindow();
}
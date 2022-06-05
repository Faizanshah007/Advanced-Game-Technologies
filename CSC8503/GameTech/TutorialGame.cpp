#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"

#include "../CSC8503Common/NavigationGrid.h"

using namespace NCL;
using namespace CSC8503;

vector <Vector3 > testNodes;

void TutorialGame::TestPathfinding(Vector3 currentPos) {
	testNodes.clear();

	NavigationGrid grid("TestGrid1.txt");

	if (currentPos == Vector3( 80, 0, 10 )) {
		for (Vector3 wallPos : grid.wallNodeLocSet) {
			AddCubeToWorld(wallPos + Vector3(0,5,0), Vector3(5, 5, 5),0);
		}
	}

	NavigationPath outPath;

	//Vector3 startPos(80, 0, 10);
	Vector3 endPos = playerBall->GetTransform().GetPosition();//(80, 0, 80);

	bool found = grid.FindPath(currentPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void TutorialGame::DisplayPathfinding(float dt) {
	/*for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1] ;
		Vector3 b = testNodes[i] ;

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}*/

	if (testNodes.size() < 2) return;
	Vector3 a = testNodes[0];// [i - 1] ;
	Vector3 b = testNodes[1];// [i] ;

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
		/*if ((b - test->GetTransform().GetPosition()).Length() <= 2.0f) {
			test->GetPhysicsObject()->ClearForces();
			test->GetPhysicsObject()->SetLinearVelocity({});
		}*/

		Vector3 currentPos = test->GetTransform().GetPosition();
		Vector3 requiredPos = ProjectPointOntoVector(a, b, currentPos);
		Vector3 requiredDirection = (b - a).Normalised();
		//Vector3 currentDirection = (b - currentPos).Normalised();

		if ((currentPos- requiredPos).Length() > 0.1f) {
			test->GetPhysicsObject()->AddForce((requiredPos - currentPos).Normalised() * 20 * dt);
		}

		test->GetPhysicsObject()->AddForce(requiredDirection * 20 * dt);

		
	}
//}

TutorialGame::TutorialGame(int level)	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;
	showPlaneMagic	= false;

	gameLevel = 0;
	score = 0;

	gameState = "InGame";

	Debug::SetRenderer(renderer);

	InitialiseAssets(level);
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets(int level) {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	//InitWorldTest();
	if (level == 1) Lvl1();
	else if (level == 2) InitWorld();
	else if (level == -1) Blank();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::Lvl2Updates(float dt) {
	TestPathfinding(test->GetTransform().GetPosition());
	DisplayPathfinding(dt);
}

void TutorialGame::Lvl1Updates(float dt) {


	if (winPad->specialValue == 1) {
		gameState = "Won";
		return;
	}

	if ((playerBall->GetTransform().GetPosition() - baseFloor->GetTransform().GetPosition()).Length() > 500.0f) {
		gameState = "Lost";
		return;
	}

	playerBall->GetRenderObject()->SetColour(Vector3(0, 1, 0));
	spinningCapsule2->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
	swingCapsule->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
	springBlock->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));

	spinningCapsule->GetPhysicsObject()->SetAngularVelocity(Vector3(0, -3, 0));

	swingCapsule->GetPhysicsObject()->SetAngularVelocity(Vector3());
	swingCapsule->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))));

	blockingBlock1->Update(dt);
	blockingBlock2->Update(dt);

	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		if (selectionObject == springBlock) {

			springBlock->TurnOn();
		}
		else springBlock->TurnOff();

		if (selectionObject == spinningCapsule2) {
			spinningCapsule2->GetPhysicsObject()->SetAngularVelocity(Vector3(0, -3, 0));
		}
	}
	springBlock->Update(dt);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I)) {
		AddCubeToWorld(spinningCapsule2->GetTransform().GetPosition() + Vector3(-3, 20, 0), Vector3(2, 2, 2), 1000.0f);
	}

	score = playerBall->specialValue;
	Debug::Print("Score: " + std::to_string(score), Vector2(60, 25));

	
}

void TutorialGame::UpdateGame(float dt, int mode) {
	if (mode != -1) {

		gameLevel = mode + 1;

		if (!inSelectionMode) {
			world->GetMainCamera()->UpdateCamera(dt);
		}

		UpdateKeys();

		if (useGravity) {
			Debug::Print("(G)ravity on", Vector2(5, 95));
		}
		else {
			Debug::Print("(G)ravity off", Vector2(5, 95));
		}

		SelectObject();
		//RayCastFromObject();
		MoveSelectedObject();
		physics->Update(dt);

		if (lockedObject != nullptr) {
			Vector3 objPos = lockedObject->GetTransform().GetPosition();
			Vector3 camPos = objPos + lockedOffset;

			Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

			Matrix4 modelMat = temp.Inverse();

			Quaternion q(modelMat);
			Vector3 angles = q.ToEuler(); //nearly there now!

			world->GetMainCamera()->SetPosition(camPos);
			world->GetMainCamera()->SetPitch(angles.x);
			world->GetMainCamera()->SetYaw(angles.y);

			//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
		}

		if (gameLevel == 1) Lvl1Updates(dt);
		else if (gameLevel == 2) Lvl2Updates(dt);

		PlayWithPlane();
		world->UpdateWorld(dt);
		renderer->Update(dt);
	}

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::Blank() {
	world->ClearAndErase();
	physics->Clear();
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	GameObject* floor = AddFloorToWorld(Vector3(45,-2,45));
	Vector3 scale = floor->GetTransform().GetScale();
	floor->GetTransform().SetScale({ scale.x / 2,scale.y,scale.z / 2 });

	test = AddCubeToWorld({ 0,0,0 }, { 2.5,2.5,2.5 });
	test->GetTransform().SetPosition({ 80, 0, 10 });
	test->GetRenderObject()->SetColour(Debug::RED);

	playerBall = AddSphereToWorld({}, 3);
	playerBall->GetTransform().SetPosition({ 80, 1.5, 40 });
	playerBall->GetRenderObject()->SetColour(Debug::CYAN);

	TestPathfinding();
}

void TutorialGame::InitWorldTest() {
	world->ClearAndErase();
	physics->Clear();

	InitDefaultFloor();
	//AddSphereToWorld(Vector3(0, 20, -10), 5.0f, 10.0f);
	
	//AddCubeToWorld(Vector3(0, 20, -10), Vector3(5.0f, 5.0f, 5.0f));
	// 
	// 
	//GameObject* test = AddCapsuleToWorld(Vector3(5, 65, 0), 15.0f, 4.0f);
	// 
	// 
	//test->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(180, Vector3(0, 0, 1))));


	/*test->GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0,0,1), 180.0f));
	testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));*/

}

void TutorialGame::Lvl1() {
	world->ClearAndErase();
	physics->Clear();
	
	spinningCapsule = AddCapsuleToWorld(Vector3(50, 5, 0), 30.0f, 4.0f, 0.0f);
	spinningCapsule->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))));

	spinningCapsule2 = AddCapsuleToWorld(Vector3(0, 5, -50), 15.0f, 4.0f, 0.0f);
	spinningCapsule2->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))));
	spinningCapsule2->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));

	Vector3 baseFloorSize = Vector3(200, 4, 200);
	Vector3 tileSize = Vector3(200, 4, 200) / 4.0f;
	Vector3 slidingFloorSize = baseFloorSize / Vector3(1,1,4);

	AddFloorToWorld(Vector3(0, -2, 0) + Vector3(baseFloorSize.x - 22, 68, -75), slidingFloorSize, Quaternion(Matrix3::Rotation(40, Vector3(0, 0, 1))), true);
	//->GetPhysicsObject()->SetElasticity(0.0f);

	baseFloor = AddFloorToWorld(Vector3(0, -2, 0), baseFloorSize);
	baseFloor->GetPhysicsObject()->SetElasticity(0.8f);
	
	GameObject* bumpyTile = AddFloorToWorld(Vector3((baseFloorSize.x - tileSize.x)/2.0f, 2.0f, -(baseFloorSize.z - tileSize.z)/2.0f), tileSize);
	bumpyTile->GetPhysicsObject()->SetInverseMass(1.0f);
	bumpyTile->GetPhysicsObject()->SetElasticity(1.5f);

	winPad = AddWinningPadToWorld(Vector3(-(baseFloorSize.x - tileSize.x) / 2.0f - 50.0f, 47.0f, (baseFloorSize.z - tileSize.z) / 2.0f - 2.0f), tileSize + Vector3(0, 90, 0));
	AddFloorToWorld(Vector3(-(baseFloorSize.x - tileSize.x) / 2.0f - 50.0f, 1.0f, (baseFloorSize.z - tileSize.z) / 2.0f - 2.0f), tileSize) ->GetRenderObject()
		->SetColour(Debug::GREEN);
	
	AddBonusToWorld(bumpyTile->GetTransform().GetPosition() + Vector3(0, 10, 0));
	AddBonusToWorld(Vector3() + Vector3(0, 10, 0));

	Vector3 wallSize(4, 50, 100);
	Vector3 threeWallSize = wallSize / Vector3(1, 1, 2.0f / 3.0f);
	Vector3 fourWallSize = wallSize / Vector3(1, 1, 2.0f / 4.0f);
	
	//Opp slide
	//AddWallToWorld(Vector3(-baseFloorSize.x/2.0f, threeWallSize.y / 2.0f,-baseFloorSize.z/8.0f), threeWallSize);
	AddWallToWorld(Vector3(-baseFloorSize.x / 1.9f, wallSize.y / 2.0f, baseFloorSize.z / 100.0f - 5.0f), wallSize);
	//Near Slide
	AddWallToWorld(Vector3(baseFloorSize.x / 1.9f, wallSize.y / 2.0f, baseFloorSize.z / 100.0f), wallSize);

	AddWallToWorld(Vector3(-baseFloorSize.x / 8.0f, threeWallSize.y / 2.0f, -baseFloorSize.z / 2.0f), Vector3(threeWallSize.z, threeWallSize.y, threeWallSize.x));
	AddWallToWorld(Vector3(0.0f, fourWallSize.y / 2.0f, baseFloorSize.z / 2.0f), Vector3(fourWallSize.z, fourWallSize.y, fourWallSize.x));

	GameObject* swingHook1 = AddSphereToWorld(Vector3(baseFloorSize.x / 2.5f, 70, -baseFloorSize.z / 2.0f), 1.0f, 0.0f);
	GameObject* swingHook2 = AddSphereToWorld(Vector3(baseFloorSize.x / 2.7f, 70, -baseFloorSize.z / 2.0f), 1.0f, 0.0f);

	swingCapsule = AddCapsuleToWorld(Vector3(baseFloorSize.x / 2.5f, 10, -baseFloorSize.z / 2.1f), 20.0f, 4.0f,1.0f);
	swingCapsule->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))));
	swingCapsule->GetPhysicsObject()->SetElasticity(1.0f);
	swingCapsule->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
	
	PositionConstraint* constraint = new PositionConstraint(swingHook1,
		swingCapsule, 60.0f);
	world->AddConstraint(constraint);
	constraint = new PositionConstraint(swingHook2,
		swingCapsule, 60.0f);
	world->AddConstraint(constraint);

	playerBall = AddSphereToWorld(Vector3(baseFloorSize.x + 40, 150, -75), 5.0f,100.0f);
	playerBall->GetPhysicsObject()->SetElasticity(1.0f);
	playerBall->Rename("playerBall");

	blockingBlock1 = AddStateObjectToWorld(Vector3(-30.0f, 1.0f, 0.0f));
	blockingBlock2 = AddStateObjectToWorld(Vector3(-70.0f, 1.0f, -80.0f));
	blockingBlock2->SetOscillatingFreq(2.0f);
	blockingBlock2->SetSystemForce(Vector3(0, 0, 60));

	springBlock = AddSpringCubeToWorld(Vector3(125.0f, 23.0f, 75.0f), Vector3(22, 22, 22), Vector3(60.0f, 23.0f, 75.0f), 10.0f);
	springBlock->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));

	AddFloorToWorld(Vector3(125.0f, -2.0f, 75.0f), Vector3(25, 2, 25))->GetPhysicsObject()->SetInverseMass(0.0f);
}

void TutorialGame::BridgeConstraintTest(const Vector3& bridgePos) {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 1; //5//how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 20; // constraint distance
	float cubeDistance = 30; // distance between links

	Vector3 startPos = bridgePos;//Vector3(0, 0, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
		, cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2)
		* cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous,
			block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, const Vector3& floorSize, const Quaternion orientation, const bool& isOBB) {
	GameObject* floor = new GameObject("floor");

	//Vector3 floorSize	= Vector3(100, 2, 100) * 2;
	if (isOBB) {
		OBBVolume* volume = new OBBVolume(floorSize / 2.0f);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}
	else {
		AABBVolume* volume = new AABBVolume(floorSize / 2.0f);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}

	floor->GetTransform()
		.SetScale(floorSize)
		.SetPosition(position)
		.SetOrientation(orientation);
		//.SetOrientation(Quaternion(Matrix3::Rotation(20, Vector3(1,0,0))));

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

WinningPad* TutorialGame::AddWinningPadToWorld(const Vector3& position, const Vector3& wpSize) {
	WinningPad* wp = new WinningPad();


	AABBVolume* volume = new AABBVolume(wpSize / 2.0f);
	wp->SetBoundingVolume((CollisionVolume*)volume);

	wp->GetTransform()
		.SetScale(Vector3()) //Vector3()
		.SetPosition(position);

	wp->SetRenderObject(new RenderObject(&wp->GetTransform(), cubeMesh, basicTex, basicShader));
	wp->SetPhysicsObject(new PhysicsObject(&wp->GetTransform(), wp->GetBoundingVolume()));

	wp->GetPhysicsObject()->SetInverseMass(0);
	wp->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wp);

	return wp;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, const Vector3& wallSize) {
	GameObject* wall = new GameObject("wall");

	AABBVolume* volume = new AABBVolume(wallSize / 2.0f);
	
	wall->SetBoundingVolume((CollisionVolume*)volume);

	wall->GetTransform()
		.SetScale(wallSize)
		.SetPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wall);

	return wall;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, bool isHollow) {
	GameObject* sphere = new GameObject("sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia(isHollow);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject("capsule");

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject("cube");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject("player");

	//AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);
	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.85f, meshSize * 0.5f); //Right Dimensions
	//CapsuleVolume* volume = new CapsuleVolume(meshSize*0.55f, meshSize*0.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

//GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
//	GameObject* apple = new GameObject("apple");
//
//	SphereVolume* volume = new SphereVolume(0.25f);
//	apple->SetBoundingVolume((CollisionVolume*)volume);
//	apple->GetTransform()
//		.SetScale(Vector3(0.25, 0.25, 0.25))
//		.SetPosition(position);
//
//	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
//	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));
//
//	apple->GetPhysicsObject()->SetInverseMass(1.0f);
//	apple->GetPhysicsObject()->InitSphereInertia();
//
//	world->AddGameObject(apple);
//
//	return apple;
//}

Coin* TutorialGame::AddBonusToWorld(const Vector3& position) {
	Coin* coin = new Coin();

	SphereVolume* volume = new SphereVolume(1.0f);
	coin->SetBoundingVolume((CollisionVolume*)volume);
	coin->GetTransform()
		//.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	coin->SetRenderObject(new RenderObject(&coin->GetTransform(), bonusMesh, nullptr, basicShader));
	coin->GetRenderObject()->SetColour(Vector3(1.0f, 0.5f, 0.0f));
	coin->SetPhysicsObject(new PhysicsObject(&coin->GetTransform(), coin->GetBoundingVolume()));

	coin->GetPhysicsObject()->SetInverseMass(0.0f);
	coin->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(coin);

	return coin;
}

void TutorialGame::PlayWithPlane() {
	if (showPlaneMagic) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		Plane p(Vector3(0, 0, 1), -400.0f);
		CollisionDetection::RayPlaneIntersection(ray, p, RayCollision());
	}
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject	= nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if(selectionObject){
		//renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
		Vector3 position = selectionObject->GetTransform().GetPosition();
		Quaternion orientation = selectionObject->GetTransform().GetOrientation();

		string objName = selectionObject->GetName();
		
		renderer->DrawString("Position ( " + std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z) + " )", Vector2(5, 5), Vector4(0.5,0,0,1));
		renderer->DrawString("orientation ( " + std::to_string(orientation.x) + " " + std::to_string(orientation.y) + " " + std::to_string(orientation.z) + " " + std::to_string(orientation.w) + " )", Vector2(5, 10), Vector4(0.5, 0, 0, 1));
		renderer->DrawString("state (if any): " + selectionObject->state , Vector2(5, 15), Vector4(0.5, 0, 0, 1));
		
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

bool TutorialGame::RayCastFromObject() {
	if (!selectionObject) return false;
	Matrix4 local = selectionObject->GetTransform().GetMatrix();
	local.SetPositionVector({ 0, 0, 0 });

	Vector3 fwd = local * Vector4(0, 0, -1, 1.0f);
	Ray objRay(selectionObject->GetTransform().GetPosition(), fwd);

	Debug::DrawLine(objRay.GetPosition(), objRay.GetDirection() * 30.0f, Debug::GREEN, 10.0f);

	RayCollision collision;
	bool b = world->Raycast(objRay, collision);

	if (b && selectionObject != collision.node) {
		std::cout << ((GameObject*)(collision.node))->GetName() << std::endl;
		return true;
	}
	else {
		return false;
	}
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {
	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude),
		Vector2(10, 20)); //Draw debug text at 10,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven’t selected anything!
	}

	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(
			*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->
					AddForceAtPosition(ray.GetDirection() * forceMagnitude,
						closestCollision.collidedAt);
			}
		}
	}
	if (!lockedObject) return;
	//Move the locked objects
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(1, 0, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(-1, 0, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SHIFT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -1, 0) * forceMagnitude);
	}
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, const Vector3& dimensions) {
	StateGameObject* block = new StateGameObject("block");

	AABBVolume* volume = new AABBVolume(dimensions);
	block->SetBoundingVolume((CollisionVolume*)volume);
	block->GetTransform()
		.SetScale(dimensions * 2)
		.SetPosition(position);

	block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
	block->SetPhysicsObject(new PhysicsObject(&block->GetTransform(), block->GetBoundingVolume()));

	block->GetPhysicsObject()->SetInverseMass(1.0f);
	block->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(block);

	return block;
}

SpringCube* TutorialGame::AddSpringCubeToWorld(const Vector3& position, const Vector3& dimensions, const Vector3& hingePosition, const float& k) {
	SpringCube* block = new SpringCube(hingePosition, position, k);

	AABBVolume* volume = new AABBVolume(dimensions);
	block->SetBoundingVolume((CollisionVolume*)volume);
	block->GetTransform()
		.SetScale(dimensions * 2)
		.SetPosition(position);

	block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
	block->SetPhysicsObject(new PhysicsObject(&block->GetTransform(), block->GetBoundingVolume()));

	block->GetPhysicsObject()->SetInverseMass(1.0f);
	block->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(block);

	return block;
}
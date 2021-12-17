#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObject.h"
#include "SpringCube.h"
#include "Coin.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(int level = 1);
			~TutorialGame();

			inline void togglePlaneMagic() { showPlaneMagic = !showPlaneMagic; }

			virtual void UpdateGame(float dt, int mode);

		protected:
			void InitialiseAssets(int level);

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void InitWorldTest();
			
			void Lvl1();
			void Lvl1Updates(float dt);

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest(const Vector3& bridgePos = Vector3());
	
			bool SelectObject();
			void MoveSelectedObject();
			bool RayCastFromObject();
			void PlayWithPlane();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize = Vector3(200, 4, 200), const Quaternion orientation = Quaternion(), const bool& isOBB = false);
			GameObject* AddWallToWorld(const Vector3& position, const Vector3& wallSize = Vector3(4,50,100));
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool isHollow = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);

			Coin* AddBonusToWorld(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* spinningCapsule;
			GameObject* spinningCapsule2;
			GameObject* playerBall;
			GameObject* swingCapsule;
			GameObject* test; // For testing

			StateGameObject* AddStateObjectToWorld(const Vector3& position, const Vector3& dimensions = { 6, 6, 6 });
			StateGameObject* blockingBlock1;
			StateGameObject* blockingBlock2;

			SpringCube*		 AddSpringCubeToWorld(const Vector3& position, const Vector3& dimensions, const Vector3& hingePosition, const float& k);
			SpringCube*		 springBlock;

			bool showPlaneMagic;

			int gameLevel;

		};
	}
}


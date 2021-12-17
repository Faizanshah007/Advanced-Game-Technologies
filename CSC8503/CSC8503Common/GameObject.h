#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {

		class GameObject	{
		public:
			float specialValue;

			GameObject(string name = "", bool canCollide = true);
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool CanCollide() const {
				return !DoNotCollide;
			}

			bool IsActive() const {
				return isActive;
			}

			void Deactivate() {
				isActive = false;
			}

			void Activate() {
				isActive = true;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void Rename(const string& newName) {
				name = newName;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			virtual void Update() {}

			string state;

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;

			bool DoNotCollide;

			bool	isActive;
			int		worldID;
			string	name;

			Vector3 broadphaseAABB;
		};
	}
}


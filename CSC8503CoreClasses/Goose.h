#pragma once
#include "GameObject.h"
#include <string>

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "RenderObject.h"
#include "PhysicsObject.h"

#include "Debug.h"

namespace NCL::CSC8503 {
	class Goose :public GameObject {
	public:
		Goose(const std::string& objectName);
		~Goose();

		virtual void OnCollisionBegin(GameObject* otherObject) override;

		void FindWayToPlayer(Vector3 startPos, Vector3 endPos);
		void DisplayGoosePath(float dt);

		void SetDefaultColor(Vector3 color) { defaultColor = color; }
		void SetSafeModeColor(Vector3 color) { safeModeColor = color; }

		void UpdateGoose(float dt, Vector3 targetPos, Vector3 realPos);
		void GooseMove(float dt);

		vector<Vector3> goosePathNodes;

	protected:
		Vector3 defaultColor;
		Vector3 safeModeColor;

		Vector3 currentTargetPos;

		int targetNode;
	};
}
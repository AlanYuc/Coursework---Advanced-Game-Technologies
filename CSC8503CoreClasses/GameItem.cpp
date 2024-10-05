#include "GameItem.h"
#include "GameWorld.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "Debug.h"

using namespace NCL::CSC8503;

GameItem::GameItem(const std::string& objectName) :GameObject(name) {
	rebirth = false;
	rebirthTime = 0;
	name = objectName;
}

GameItem::~GameItem() {

}

void GameItem::UpdateAll(float dt) {
	if (rebirth) {
		rebirthTime += dt;
		//this->GetRenderObject()->SetColour(Vector4(0.5f, 0.4216f, 0, sin(rebirthTime * 20)));
		this->GetRenderObject()->SetColour(Vector4(defaultColor * 0.5f, sin(rebirthTime * 20)));
		this->GetTransform().SetPosition(pickedPos);

		if (rebirthTime >= 5.0f) {
			rebirth = false;//重生完成
			//rebirth
			//this->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, rebirthTime));
			this->GetRenderObject()->SetColour(Vector4(defaultColor, rebirthTime));
			this->SetBoundingVolume(defultVolume);
			this->GetPhysicsObject()->SetInverseMass(2.0f);
		}
	}
}

void GameItem::OnCollisionBegin(GameObject* otherObject) {
	//std::cout << "与玩家发生接触" << otherObject->GetName() << "\n";
	if (otherObject->GetName() == "Player") {
		this->SetBoundingVolume(nullptr);
		this->GetPhysicsObject()->SetInverseMass(0);
		rebirth = true;
		rebirthTime = 0;
		pickedPos = this->GetTransform().GetPosition();
		//Debug::Print("与玩家发生接触", Vector2(30, 30));
	}
}
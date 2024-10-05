#include "Player.h"

using namespace NCL::CSC8503;

Player::Player(const std::string& objectName) :GameObject(name) {
	playerScore = 0;
	playerHp = 100;
	name = objectName;
	isWin = false;
	isLose = false;
}

Player::~Player() {

}

void Player::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "EnemyRed") {
		playerHp -= 10;

		if (playerHp <= 0) {
			playerHp = 100;
			Reborn();
		}
	}

	if (otherObject->GetName() == "Goose") {
		playerHp = 100;
		Reborn();
		otherObject->GetTransform().SetPosition(Vector3(-190, 10, -190));
	}

	if (otherObject->GetName() == "Goat") {
		if (playerScore >= 2) {
			isWin = true;
		}
	}

	if (otherObject->GetName() == "Coin") {
		playerScore += 1;
	}

	if (otherObject->GetName() == "HealthPack") {
		playerHp += 10;
	}

	if (otherObject->GetName() == "PowerUp") {
		
	}

	if (otherObject->GetName() == "DamageImmunity") {

	}

	if (otherObject->GetName() == "SpeedUp") {

	}
	if (otherObject->GetName() == "Wall") {

	}
}


void Player::Reborn() {
	transform.SetPosition(this->GetRebornPos());
}

void Player::CheckRebornPos() {
	Vector3 currentPos = this->GetTransform().GetPosition();
	float posX = currentPos.x;
	float posZ = currentPos.z;

	if (posX <= -40 && posZ <= -40) {
		rebornPos = Vector3(-40, 5, -120);
	}
	else if (posX <= -40 && posZ >= 40) {
		rebornPos = Vector3(-40, 5, 120);
	}
}



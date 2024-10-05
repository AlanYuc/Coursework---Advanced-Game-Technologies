#pragma once
#include "GameObject.h"
#include "Window.h"
#include <string>
#include "PhysicsObject.h"
#include "Ray.h"


namespace NCL::CSC8503 {
	class Player : public GameObject {
	public:
		Player(const std::string& objectName);
		~Player();

		virtual void OnCollisionBegin(GameObject* otherObject) override;

		void SetPlayerHp(int hp) { playerHp += hp; }
		int GetPlayerHp() { return playerHp; }
		void SetPlayerScore(int score) { playerScore += score; }
		int GetPlayerScore() { return playerScore; }

		void SetRebornPos(Vector3 pos) { rebornPos = pos; }

		void CheckRebornPos();

		//void UpdatePlayerPosition(float dt);

		void SetRebornPos(const Vector3& position) { rebornPos = position; }
		Vector3 GetRebornPos() { return rebornPos; }
		void Reborn();

		bool IsWin() { return isWin; }
		bool IsLose() { return isLose; }

	protected:
		int playerScore;
		int playerHp;
		Vector3 rebornPos;

		bool isWin;
		bool isLose;

	};
}
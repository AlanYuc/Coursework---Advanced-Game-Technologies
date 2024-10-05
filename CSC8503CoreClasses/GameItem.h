#pragma once

#include "GameObject.h"
#include <string>

namespace NCL::CSC8503 {
	class GameItem :public GameObject {
	public:
		GameItem(const std::string& objectName);
		~GameItem();

		virtual void OnCollisionBegin(GameObject* otherObject) override;

		void SetDefultVolume(CollisionVolume* vol) { defultVolume = vol; }
		void SetDefultDimension(Vector3 dimension) { defultDimension = dimension; }
		void SetDefaultColor(Vector3 color) { defaultColor = color; }

		void UpdateAll(float dt);

	protected:
		CollisionVolume* defultVolume;
		Vector3 defultDimension;
		Vector3 defaultColor;

		bool rebirth;
		float rebirthTime;

		//记录玩家捡到道具时的位置
		Vector3 pickedPos;
	};
}
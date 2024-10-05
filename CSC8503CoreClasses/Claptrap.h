#pragma once
#include "GameObject.h"
#include <string>

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "Debug.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

#include "RenderObject.h"
#include "PhysicsObject.h"


namespace NCL::CSC8503 {

	enum ClaptrapState
	{
		Idle,
		Moving
	};

	class Claptrap : public GameObject {
	public:
		Claptrap(const std::string& objectName);
		~Claptrap();

		void ClaptrapPathfinding(Vector3 startPos, Vector3 endPos);
		void DisplayClaptrapPath(float dt);
		
		bool ClaptrapMove(float dt);

		void Update(float dt);

		void OnAwake(int id);
		void OnSleep();

		void SetPathID(int id) { pathID = id; }

		void ClaptrapBehaviourTree();
		void ResetBehaviourTree();
		void OnBehaviour();

		void SetInitColor(Vector4 color) { initColor = color; }

		vector<Vector3> claptrapPathNodes1;
		vector<Vector3> claptrapPathNodes2;
		vector<Vector3> claptrapPathNodes3;

	protected:

		void ChangeColour(float dt);

		//控制导航线颜色变化
		float timeCounter;

		//状态切换
		ClaptrapState claptrapState;

		//1：从原点向左侧目标前进的路线
		//2：从原点向右侧目标前进的路线
		//3：从左侧目标向右侧目标前进的路线
		int pathID;

		int targetNode;

		int sizeNums[3];

		BehaviourSequence* sequence;
		BehaviourState state;

		//到位置后获取宝藏的时间，就是让它在宝藏点停留一会
		float treasureTime;

		Vector4 initColor;
	};
}

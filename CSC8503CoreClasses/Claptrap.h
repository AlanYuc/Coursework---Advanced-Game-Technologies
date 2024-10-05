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

		//���Ƶ�������ɫ�仯
		float timeCounter;

		//״̬�л�
		ClaptrapState claptrapState;

		//1����ԭ�������Ŀ��ǰ����·��
		//2����ԭ�����Ҳ�Ŀ��ǰ����·��
		//3�������Ŀ�����Ҳ�Ŀ��ǰ����·��
		int pathID;

		int targetNode;

		int sizeNums[3];

		BehaviourSequence* sequence;
		BehaviourState state;

		//��λ�ú��ȡ���ص�ʱ�䣬���������ڱ��ص�ͣ��һ��
		float treasureTime;

		Vector4 initColor;
	};
}

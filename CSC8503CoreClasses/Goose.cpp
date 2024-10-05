#include "Goose.h"

using namespace NCL::CSC8503;

Goose::Goose(const std::string& objectName) :GameObject(name) {
	name = objectName;

	currentTargetPos = Vector3();

	targetNode = 0;
}

Goose::~Goose() {

}

void Goose::UpdateGoose(float dt, Vector3 targetPos, Vector3 realPos) {
	
	


	if (targetPos.x > -50 || targetPos.z > -50) {
		return;
	}
	//targetNode > goosePathNodes.size()

	Vector3 dis = realPos - this->GetTransform().GetPosition();
	if ( abs(dis.x)< 5 && abs(dis.z) < 5){
		this->GetTransform().SetPosition(realPos);
		targetNode = 1;
		return;
	}


	Vector3 delta = targetPos - currentTargetPos;
	float d = delta.Length();
	if (d > 1.0f) {//目标位置发生变化
		targetNode = 1;
		currentTargetPos = targetPos;
		goosePathNodes.clear();
		FindWayToPlayer(this->GetTransform().GetPosition() + Vector3(200, 0, 190), targetPos + Vector3(200, 0, 190));
	}

	GooseMove(dt);
}

void Goose::GooseMove(float dt) {
	float framespeed = 20.0f * dt;

	Vector3 currentPos = transform.GetPosition();
	Vector3 targetPos;

	targetPos = goosePathNodes[targetNode] ;

	Vector3 deltaPos = targetPos - currentPos;
	deltaPos.y = 0;
	float distanceToWaypoint = deltaPos.Length();

	Vector3 direction = deltaPos.Normalised();

	Vector3 newPos = (direction * framespeed) + currentPos;


	if (distanceToWaypoint > 0.01f) {
		transform.SetPosition(newPos);
	}
	else {
		//std::cout << "已经到达当前航点！";
		transform.SetPosition(Vector3(targetPos.x, newPos.y, targetPos.z));
		targetNode++;
	}
}

void Goose::FindWayToPlayer(Vector3 startPos, Vector3 endPos) {
	NavigationGrid grid("GooseGrid.txt");
	NavigationPath outPath;

	

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;

	while (outPath.PopWaypoint(pos)) {
		pos += Vector3(-250, 0, 0);
		goosePathNodes.push_back(pos);
	}
}

void Goose::DisplayGoosePath(float dt) {
	for (int i = 1; i < goosePathNodes.size(); i++) {
		//默认的地图我把放到左上角了，所以这里还要先移动到左下角
		Vector3 startPos = goosePathNodes[i - 1] ;
		Vector3 endPos = goosePathNodes[i];

		Debug::DrawLine(startPos, endPos, Vector4(0, 0.5f, 0.5f, 1));
	}
}

void Goose::OnCollisionBegin(GameObject* otherObject) {
	/*if (otherObject->GetName() == "Player") {
		this->GetTransform().SetPosition(Vector3(-190, 15, -190));
	}*/
	
}
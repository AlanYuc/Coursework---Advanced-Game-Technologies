#include "Claptrap.h"

using namespace NCL::CSC8503;

Claptrap::Claptrap(const std::string& objectName) :GameObject(name) {
	timeCounter = 0;
	claptrapState = Idle;
	pathID = 1;
	targetNode = 0;
	//sizeNums[3] = {0};
	for (int i = 0; i < 3; i++) {
		sizeNums[i] = 0;
	}
	sequence = new BehaviourSequence("Maze Sequence");
	state = Initialise;
	treasureTime = 0;
	//initColor = this->GetRenderObject()->GetColour();

	name = objectName;
}

Claptrap::~Claptrap() {

}

void Claptrap::ClaptrapPathfinding(Vector3 startPos, Vector3 endPos) {
	NavigationGrid grid("MazeGrid.txt");
	NavigationPath outPath;
	bool found = grid.FindPath(startPos, endPos, outPath);

	//Vector3 currentPos = transform.GetPosition();
	Vector3 pos;

	while (outPath.PopWaypoint(pos)) {
		/*Vector3 deltaPos = currentPos - pos;
		float distanceToWaypoint = deltaPos.Length();
		if (distanceToWaypoint < 0.01f) {
			claptrapPathNodes.push_back(pos);
		}
		else {
			outPath.PushWaypoint(pos);
			break;
		}*/
		switch (pathID)
		{
		case(1):claptrapPathNodes1.push_back(pos); break;
		case(2):claptrapPathNodes2.push_back(pos); break;
		case(3):claptrapPathNodes3.push_back(pos); break;
		}
	}

	switch (pathID)
	{
	case(1):sizeNums[0] = claptrapPathNodes1.size(); break;
	case(2):sizeNums[1] = claptrapPathNodes2.size(); break;
	case(3):sizeNums[2] = claptrapPathNodes3.size(); break;
	}
}

void Claptrap::DisplayClaptrapPath(float dt) {

	
	

	for (int i = 1; i < sizeNums[pathID - 1]; i++) {

		Vector3 start;
		Vector3 end;

		switch (pathID)
		{
		case(1):
			start = claptrapPathNodes1[i - 1];
			end = claptrapPathNodes1[i];
			break;
		case(2):
			start = claptrapPathNodes2[i - 1];
			end = claptrapPathNodes2[i];
			break;
		case(3):
			start = claptrapPathNodes3[i - 1];
			end = claptrapPathNodes3[i]; 
			break;
		}

		//向上抬高一点
		start += Vector3(0, 10, 0);
		end += Vector3(0, 10, 0);

		timeCounter += dt / 200.0f;
		if (timeCounter > 1.0f) {
			timeCounter -= 1.0f;
		}
		Debug::DrawLine(start, end, Vector4(0, timeCounter, 0, 1));
	}
}

bool Claptrap::ClaptrapMove(float dt) {
	float framespeed = 100.0f * dt;

	Vector3 currentPos = transform.GetPosition();

	Vector3 targetPos;
	switch (pathID)
	{
	case(1):targetPos = claptrapPathNodes1[targetNode]; break;
	case(2):targetPos = claptrapPathNodes2[targetNode]; break;
	case(3):targetPos = claptrapPathNodes3[targetNode]; break;
	}
	

	Vector3 deltaPos = targetPos - currentPos;
	deltaPos.y = 0;
	float distanceToWaypoint = deltaPos.Length();

	Vector3 direction = deltaPos.Normalised();

	Vector3 newPos = (direction * framespeed) + currentPos;
	

	if (distanceToWaypoint > 0.01f) {
		transform.SetPosition(newPos);
		return false;
	}
	else {
		//std::cout << "已经到达当前航点！";
		transform.SetPosition(Vector3(targetPos.x, newPos.y, targetPos.z));
		return true;
	}
}

void Claptrap::OnAwake(int id) {
	claptrapState = Moving;
	targetNode = 0;
	pathID = id;

}

void Claptrap::OnSleep() {
	claptrapState = Idle;
	targetNode = 0;
}

void Claptrap::Update(float dt) {
	/*if (claptrapState == Moving) {

		if (targetNode == sizeNums[pathID - 1]) {
			OnSleep();
			return;
		}

		if (ClaptrapMove(dt)) {
			targetNode++;
		}
	}*/
	

	if (state == Ongoing) {
		state = sequence->Execute(dt);
	}
}

void Claptrap::ClaptrapBehaviourTree() {

	BehaviourAction* goLeft = new BehaviourAction("Find Left Key",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Going left for a key!\n";
				state = Ongoing;

				OnAwake(1);
			}
			else if (state == Ongoing) {

				if (targetNode == sizeNums[pathID - 1]) {
					std::cout << "Found a left key!\n";
					OnSleep();
					return Success;
				}

				if (ClaptrapMove(dt)) {
					targetNode++;
				}
			}
			return state; 
		}
	);

	BehaviourAction* PickTreasure = new BehaviourAction("Pick Rreasure",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Picking Rreasure!\n";
				state = Ongoing;
				treasureTime = 0;
			}
			else if (state == Ongoing) {
				treasureTime += dt;
				if (treasureTime >2.0f) {
					std::cout << "Picked Rreasure!\n";
					return Success;
				}
				
				ChangeColour(1 - sin(treasureTime * 40));
			}
			return state; 
		}
	);

	BehaviourAction* goRight = new BehaviourAction("Find Right Key",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Going right for a key!\n";
				state = Ongoing;

				OnAwake(2);
			}
			else if (state == Ongoing) {

				if (targetNode == sizeNums[pathID - 1]) {
					std::cout << "Found a right key!\n";
					OnSleep();
					return Success;
				}

				if (ClaptrapMove(dt)) {
					targetNode++;
				}
			}
			return state; 
		}
	);

	BehaviourAction* PickTreasure2 = new BehaviourAction("Pick Rreasure",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Picking Rreasure!\n";
				state = Ongoing;
				treasureTime = 0;
			}
			else if (state == Ongoing) {
				treasureTime += dt;
				if (treasureTime > 2.0f) {
					std::cout << "Picked Rreasure!\n";
					return Success;
				}

				ChangeColour(1 - sin(treasureTime * 40));
			}
			return state;
		}
	);

	BehaviourAction* backToGate = new BehaviourAction("Back To Gate",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Going back!\n";
				state = Ongoing;

				OnAwake(3);
			}
			else if (state == Ongoing) {
				if (targetNode == sizeNums[pathID - 1]) {
					std::cout << "Took a key back!\n";
					OnSleep();
					return Success;
				}

				if (ClaptrapMove(dt)) {
					targetNode++;
				}
			}
			return state; 
		}
	);

	
	sequence->AddChild(goLeft);
	sequence->AddChild(PickTreasure);
	sequence->AddChild(goRight);
	sequence->AddChild(PickTreasure2);
	sequence->AddChild(backToGate);
}

void Claptrap::ResetBehaviourTree() {
	sequence->Reset();
}

void Claptrap::OnBehaviour() {
	state = Ongoing;
}

void Claptrap::ChangeColour(float dt) {
	//Vector4 currentColor = this->GetRenderObject()->GetColour();
	
	this->GetRenderObject()->SetColour(Vector4(dt, dt, 0, 1));
}
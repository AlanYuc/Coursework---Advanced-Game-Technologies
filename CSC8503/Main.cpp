#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

#include "GameMenu.h"

using namespace NCL;
using namespace CSC8503;
using namespace std;

#include <chrono>
#include <thread>
#include <sstream>


//两段寻路的测试代码
vector < Vector3 > testNodes;
void TestPathfinding() {
	/*NavigationGrid grid("TestGrid1.txt");
	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);*/

	NavigationGrid grid("MazeGrid.txt");
	Vector3 startPos(70, 0, 20);//这里要使用原始坐标，在NavigationGrid.cpp里再把导航的线移动过去
	Vector3 endPos(30, 0, 130);
	//Vector3 endPos(0, 0, 70);

	NavigationPath outPath;

	

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;

	//这里直接添加了PopWaypoint
	//对于 AI 角色，您将根据 AI 的当前位置进行距离检查，
	//只有在 AI 接近足够的节点时才弹出一个航点，以便它可以继续前进到下一个节点。
	//因此，AI寻路的判定需要在这里再做一些修改
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a + Vector3(0, 10, 0), b + Vector3(0, 10, 0), Vector4(0, 1, 0, 1));
	}
}

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)-> void
		{
			std::cout << "I'm in state A!\n";
			data++;
		}
	);
	
	State * B = new State([&](float dt)-> void
		{
			std::cout << "I'm in state B!\n";
			data--;
		}
	);

	StateTransition* stateAB = new StateTransition(A, B, [&](void)-> bool
		{
			return data > 10;
		}
	);
	StateTransition * stateBA = new StateTransition(B, A, [&](void)-> bool
		{
			return data < 0;
		}
	);

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);
	
	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f);
	}
}

//行为树测试代码
void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;

	BehaviourAction* findKey = new BehaviourAction("Find Key",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for a key!\n";
				behaviourTimer = rand() % 100;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				behaviourTimer -= dt;
				if (behaviourTimer <= 0.0f) {
					std::cout << "Found a key!\n";
					return Success;
				}
			}
			return state; // will be 'ongoing' until success
		}
	);
	BehaviourAction* goToRoom = new BehaviourAction("Go To Room",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Going to the loot room!\n";
				state = Ongoing;
			}
			else if (state == Ongoing) {
				distanceToTarget -= dt;
				if (distanceToTarget <= 0.0f) {
					std::cout << "Reached room!\n";
					return Success;
				}
			}
			return state; // will be 'ongoing' until success
		}
	);

	BehaviourAction* openDoor = new BehaviourAction("Open Door",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Opening Door!\n";
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* lookForTreasure = new BehaviourAction(
		"Look For Treasure",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for treasure!\n";
				return Ongoing;
			}
			else if (state == Ongoing) {
				bool found = rand() % 2;
				if (found) {
					std::cout << "I found some treasure!\n";
					return Success;
				}
				std::cout << "No treasure in here...\n";
				return Failure;
			}
			return state;
		}
	);

	BehaviourAction* lookForItems = new BehaviourAction(
		"Look For Items",
		[&](float dt, BehaviourState state) -> BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for items!\n";
				return Ongoing;
			}
			else if (state == Ongoing) {
				bool found = rand() % 2;
				if (found) {
					std::cout << "I found some items!\n";
					return Success;
				}
				std::cout << "No items in here...\n";
				return Failure;
			}
			return state;
		}
	);

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; ++i) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We're going on an adventure!\n";
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f); // fake dt
		}
		if (state == Success) {
			std::cout << "What a successful adventure!\n";
		}
		else if (state == Failure) {
			std::cout << "What a waste of time!\n";
		}
	}
	std::cout << "All done!\n";

}

//pushdown automata测试代码
class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		std::cout << "Press U to unpause game!\n";
	}
};
class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << "Coins mined: " << coinsMined << "\n";
			std::cout << "Press P to pause game, or F1 to return to main menu!\n";
			pauseReminder += 1.0f;
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::P)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::F1)) {
			std::cout << "Returning to main menu!\n";
			return PushdownResult::Pop;
		}

		if (rand() % 7 == 0) {
			coinsMined++;
		}

		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		std::cout << "Preparing to mine coins!\n";
	}

protected:
	int coinsMined = 0;
	float pauseReminder = 1;
};
class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
			*newState = new GameScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		std::cout << "Welcome to a really awesome game!\n";
		std::cout << "Press Space To Begin or escape to quit!\n";
	}
};
void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::H)) {
			std::cout << "reset!";
			machine.Reset(new IntroScreen());
		}
	}
} 

//Networking - Networking Protocols测试代码
class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;

			string msg = realPacket->GetStringFromData();

			cout << name << " received message: " << msg << endl;
		}
	}

protected:
	string name;
};

void TestNetworking() {
	NetworkBase::Initialise();

	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver("Client");

	int port = NetworkBase::GetDefaultPort();

	GameServer* server = new GameServer(port, 1);
	GameClient* client = new GameClient();

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);

	bool canConnect = client->Connect(127, 0, 0, 1, port);

	for (int i = 0; i < 100; ++i) {
		//StringPacket stringPacket = StringPacket("Server says hello! " + to_string(i));
		//不用上面的一句，因为= 右边的构造函数调用会创建一个临时对象，
		// 然后通过拷贝构造函数或者移动构造函数将其复制到 stringPacket，
		// 这会多出一次构造和复制的操作
		//使用下面一句会更加简洁
		StringPacket stringPacket("Server says hello! " + to_string(i));
		
		//server->SendGlobalPacket(StringPacket("Server says hello! " + to_string(i)));
		server->SendGlobalPacket(stringPacket);
		//client->SendPacket(StringPacket("Client says hello! " + to_string(i)));
		client->SendPacket(stringPacket);

		server->UpdateServer();
		client->UpdateClient();

		this_thread::sleep_for(chrono::milliseconds(10));
	}

	NetworkBase::Destroy();
}



/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	//AI 寻路测试
	//TestPathfinding();

	//行为树测试
	TestBehaviourTree();

	//pushdown automata测试
	//TestPushdownAutomata(w);

	//Networking - Networking Protocols测试
	//TestNetworking();

	TutorialGame* g = new TutorialGame();
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
			w->SetWindowPosition(0, 0);
		}

		/*if (Window::GetKeyboard()->KeyHeld(KeyCodes::Y)) {
			DisplayPathfinding();
		}*/

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}
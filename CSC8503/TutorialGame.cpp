#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"




using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;
	inPlayerMode	= false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
	
	m = new GameMenu();

	leftTime = 500;

	playerShootType = PUSH;

	//sceneMaze = new SimpleMaze();这句不能写在这里
	//player = new Player("Player");

	//srand(static_cast<unsigned int>(time(0)));

	/*claptrap = new Claptrap("c");
	claptrap->ClaptrapPathfinding(Vector3(70, 0, 20),Vector3(30, 0, 130));*/
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	gooseMesh   = renderer->LoadMesh("goose.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();

}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;

}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera().UpdateCamera(dt);
	}

	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();

		Vector3 camPos = Vector3();
		if (lockedObject->GetName() == "Player") {
			camPos = objPos + player->GetTransform().GetOrientation() * lockedOffset;
		}
		else {
			camPos = objPos + lockedOffset;
		}
		

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera().SetPosition(camPos);
		world->GetMainCamera().SetPitch(angles.x);
		world->GetMainCamera().SetYaw(angles.y);
	}
	
	/*if (Window::GetKeyboard()->KeyDown(KeyCodes::E)) {
		std::cout << "相机位置" << world->GetMainCamera().GetPosition().x << " "
			<< world->GetMainCamera().GetPosition().y << " "
			<< world->GetMainCamera().GetPosition().z << " "
			<< world->GetMainCamera().GetPitch() << " "
			<< world->GetMainCamera().GetYaw() << "\n";
	}*/

	UpdateKeys();

	//PushdownMachine machine(new StartScreen());
	//GameMenu* m = new GameMenu();
	
	//菜单更新
	//m->DrawPlayerInfo(10, 200);
	if (!startMenu) {
		m->DrawPlayerInfo(player->GetPlayerScore(), player->GetPlayerHp());
		leftTime -= dt;
		m->DrawTime(leftTime);

		if (player->IsWin()) {
			//Debug::Print("You win!", Vector2(40, 40));
			startMenu = true;
		}
		else if (leftTime <= 0) {
			
			startMenu = true;
		}

		m->DrawItemsLeft(gameItems.size());
		m->DrawFuncSelection(dt);
		m->DrawAttackMode(playerShootType);
		
		if (useGravity) {
			Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
		}
		else {
			Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
		}
	}
	else {
		m->DrawStartMenu();
		if (!player->IsWin() && leftTime<=0) {
			Debug::Print("You Lose!", Vector2(40, 65));
		}
		if (player->IsWin()) {
			Debug::Print("You win!", Vector2(40, 65));
		}
		Debug::Print("Your score: " + to_string(player->GetPlayerScore()), Vector2(37, 70));
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::E)) {
		startMenu = true;
		world->GetMainCamera().SetPitch(-9.95998f);
		world->GetMainCamera().SetYaw(162.341f);
		world->GetMainCamera().SetPosition(Vector3(-108.07, 165.02, 177.128));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::F)) {
		startMenu = false;
	}
	

	//玩家更新
	LockCameraToPlayer();
	if (!startMenu) {
		m->DrawPlayerModeInfo(inPlayerMode);
	}
	
	UpdatePlayerPosition(dt);
	player->CheckRebornPos();

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM4)) {
		playerShootType = PUSH;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
		playerShootType = PULL;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM6)) {
		playerShootType = SWING;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
		playerShootType = DESTROY;
	}
	ShootGrapplingHook(playerShootType);

	//敌人AI更新
	for (const auto& enemy : enemys) {
		enemy->Update(dt);

		if (highLevelEnemy % 4 == 0) {
			ShootToPlayer(enemy);
			enemy->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
		}
		highLevelEnemy++;
	}
	highLevelEnemy = 0;

	//大鹅更新
	//goose->UpdateGoose(dt, player->GetTransform().GetPosition());

	if (player->GetTransform().GetPosition().x <= -50 && player->GetTransform().GetPosition().z <= -50) {
		Vector3 tpos = CheckPlayerPos();
		goose->UpdateGoose(dt, tpos, player->GetTransform().GetPosition());
		//goose->UpdateGoose(dt, player->GetTransform().GetPosition());
		goose->DisplayGoosePath(dt);
	}
	//goose->FindWayToPlayer(Vector3(0, 5, 0), Vector3(30, 10, 110));
	


	//Claptrap更新
	//已经在移动的时候不能再按键，不然移动会出错
	/*if (Window::GetKeyboard()->KeyDown(KeyCodes::C)) {
		claptrap->OnAwake(1);
	}*/
	if (Window::GetKeyboard()->KeyDown(KeyCodes::C)) {
		//claptrap->OnAwake(2);
		claptrap->ResetBehaviourTree();
		claptrap->OnBehaviour();
	}
	claptrap->DisplayClaptrapPath(dt);
	claptrap->Update(dt);

	/*int asd = claptrap->claptrapPathNodes.size();
	for (int i = 1; i < asd;) {
		reachNextWayPoint = claptrap->ClaptrapMove(i, dt);
		if (reachNextWayPoint) {
			i++;
		}
	}*/

	//金币更新
	for (const auto& gameItem : gameItems) {
		gameItem->UpdateAll(dt);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::H)) {
		AddOBBs();
	}

	

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	

	//AI - State Machines添加的代码
	if (testStateObject) {
		testStateObject->Update(dt);
	}

	renderer->Render();
	Debug::UpdateRenderables(dt);

	
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	//world->GetMainCamera().SetPitch(-15.0f);
	//world->GetMainCamera().SetYaw(315.0f);
	//world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));

	world->GetMainCamera().SetPitch(-9.95998f);
	world->GetMainCamera().SetYaw(162.341f);
	world->GetMainCamera().SetPosition(Vector3(-108.07, 165.02, 177.128));

	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	InitGameExamples();

	

	//状态机测试
	//testStateObject = AddStateObjectToWorld(Vector3(0, 30, 0));

	//约束的桥
	BridgeConstraintTest();


	//添加场景
	
	//添加地板
	InitDefaultFloor();
	//添加围墙
	InitStaticWall();

	//添加玩家
	AddPlayer();

	//添加自动移动的AI
	AddEnemy();
	
	//添加迷宫
	InitLevelMaze();
	
	//添加左下地图的内墙
	InitLevelAI();

	//添加右下地图
	InitLevelPhysics();

	//添加Claptrap
	AddClaptrap();

	//添加游戏道具，包括金币，
	AddGameItem();

	//添加大鹅
	AddGoose();

	//添加羊
	InitSquare();
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	//cube->SetName("cube");

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddGoatToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("Goat");
	SphereVolume* volume  = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);
	
	//这里的bonusMesh有问题，显示不出来
	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	//StateGameObject* apple = new StateGameObject();

	//SphereVolume* volume = new SphereVolume(0.5f);
	//apple->SetBoundingVolume((CollisionVolume*)volume);
	//apple->GetTransform()
	//	.SetScale(Vector3(2, 2, 2))
	//	.SetPosition(position);


	////这里的bonusMesh有问题，显示不出来
	////apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	//apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	//apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	//apple->GetPhysicsObject()->SetInverseMass(1.0f);
	//apple->GetPhysicsObject()->InitSphereInertia();

	//world->AddGameObject(apple);

	//return apple;

	float meshSize = 3.0f;
	float inverseMass = 0.5f;//调整质量，质量越小，移动物体需要的力就越多

	StateGameObject* enemyRed = new StateGameObject("EnemyRed");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	enemyRed->SetBoundingVolume((CollisionVolume*)volume);

	enemyRed->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	enemyRed->SetRenderObject(new RenderObject(&enemyRed->GetTransform(), enemyMesh, nullptr, basicShader));
	enemyRed->SetPhysicsObject(new PhysicsObject(&enemyRed->GetTransform(), enemyRed->GetBoundingVolume()));

	enemyRed->GetPhysicsObject()->SetInverseMass(inverseMass);
	enemyRed->GetPhysicsObject()->InitCubeInertia();

	enemyRed->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	//调整初始方向
	//enemyRed->SetMoveDir(initMoveDir);
	int initMoveDir = enemyRed->GetInitMoveDir();
	if (initMoveDir == 1) {
		enemyRed->ChangeMoveDir(90.0f);
	}
	else {
		enemyRed->ChangeMoveDir(180.0f);
	}
	
	
	/*Quaternion currentOrientation = enemyRed->GetTransform().GetOrientation();
	Vector3 rotationAxis(0.0f, 1.0f, 0.0f);
	float degrees = 90.0f;
	Quaternion rotationQuaternion = Quaternion::AxisAngleToQuaterion(rotationAxis, degrees);
	Quaternion rotatedQuaternion = currentOrientation * rotationQuaternion;
	enemyRed->GetTransform().SetOrientation(rotatedQuaternion);*/

	world->AddGameObject(enemyRed);

	return enemyRed;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, 0, 0));
}

void TutorialGame::InitGameExamples() {
	AddGoatToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	//AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

//Physics - Constraints and Solvers代码
void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(4, 1, 4);

	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 12; // constraint distance
	float cubeDistance = 8; // distance between links

	//Vector3 startPos = Vector3(500, 500, 500);
	Vector3 startPos = Vector3(-160, 50, 180);
	
	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	
	GameObject * previous = start;
	
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);

	Debug::DrawLine(start->GetTransform().GetPosition(), Vector3(), Vector4(0, 1, 0, 1));
}


/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	if (!startMenu) {
		if (inSelectionMode) {
			Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

			if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {

				if (selectionObject) {	//set colour to deselected;
					selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
					selectionObject = nullptr;
				}

				Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

				RayCollision closestCollision;
				if (world->Raycast(ray, closestCollision, true)) {
					selectionObject = (GameObject*)closestCollision.node;
					selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
					return true;
				}
				else {
					return false;
				}
			}
			//确保player一直是蓝色
			player->GetRenderObject()->SetColour(Vector4(0, 0.75f, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
				if (selectionObject) {
					if (lockedObject == selectionObject) {
						lockedObject = nullptr;
					}
					else {
						lockedObject = selectionObject;
					}
				}
			}
		}
		else {
			Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
		}
	}
	
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	if (!startMenu) {
		Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	}
	
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	//如果出现点击反馈不太好，把buttonPressed换成buttonDown就行，这里暂时先不动
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}



//搭建游戏场景
void TutorialGame::InitStaticWall() {
	//外围墙
	AddWallToWorld(Vector3(-200,40,0), Vector3(2, 40, 200));
	AddWallToWorld(Vector3(200, 40, 0), Vector3(2, 40, 200));
	AddWallToWorld(Vector3(0, 40, -200), Vector3(200, 40, 2));
	AddWallToWorld(Vector3(0, 40, 200), Vector3(200, 40, 2));

	//四个分区的墙
	AddWallToWorld(Vector3(-40, 10, -164), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(-40, 10, -76), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(-120, 10, -40), Vector3(80, 10, 2));

	AddWallToWorld(Vector3(-40, 10, 164), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(-40, 10, 76), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(-120, 10, 40), Vector3(80, 10, 2));

	AddWallToWorld(Vector3(40, 10, -164), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(40, 10, -76), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(120, 10, -40), Vector3(80, 10, 2));
	
	AddWallToWorld(Vector3(40, 10, 164), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(40, 10, 76), Vector3(2, 10, 36));
	AddWallToWorld(Vector3(164, 10, 40), Vector3(36, 10, 2));
	AddWallToWorld(Vector3(76, 10, 40), Vector3(36, 10, 2));

}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, Vector3 dimensions) {
	GameObject* wall = new GameObject("Wall");

	Vector3 wallSize = dimensions;
	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform()
		.SetScale(wallSize * 2)
		.SetPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wall);

	return wall;
}

GameObject* TutorialGame::AddObstacleToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* obstacle = new GameObject("Obstacle");

	AABBVolume* volume = new AABBVolume(dimensions);
	obstacle->SetBoundingVolume((CollisionVolume*)volume);

	obstacle->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	obstacle->SetRenderObject(new RenderObject(&obstacle->GetTransform(), cubeMesh, basicTex, basicShader));
	obstacle->SetPhysicsObject(new PhysicsObject(&obstacle->GetTransform(), obstacle->GetBoundingVolume()));

	obstacle->GetPhysicsObject()->SetInverseMass(inverseMass);
	obstacle->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(obstacle);

	return obstacle;
}

Player* TutorialGame::AddPlayerToWorld(const Vector3& position, float radius, float inverseMass) {
	//float meshSize = 1.0f;
	//float inverseMass = 0.5f;

	Player* newPlayer = new Player("Player");

	SphereVolume* volume = new SphereVolume(radius);

	newPlayer->SetBoundingVolume((CollisionVolume*)volume);

	newPlayer->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(radius, radius, radius));

	newPlayer->SetRenderObject(new RenderObject(&newPlayer->GetTransform(), sphereMesh, nullptr, basicShader));
	newPlayer->SetPhysicsObject(new PhysicsObject(&newPlayer->GetTransform(), newPlayer->GetBoundingVolume()));

	newPlayer->GetPhysicsObject()->SetInverseMass(inverseMass);
	newPlayer->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(newPlayer);

	return newPlayer;
}

Claptrap* TutorialGame::AddClaptrapToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Claptrap* newClaptrap = new Claptrap("Claptrap");
	AABBVolume* volume = new AABBVolume(dimensions);
	newClaptrap->SetBoundingVolume((CollisionVolume*)volume);
	newClaptrap->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	newClaptrap->SetRenderObject(new RenderObject(&newClaptrap->GetTransform(), cubeMesh, nullptr, basicShader));
	newClaptrap->SetPhysicsObject(new PhysicsObject(&newClaptrap->GetTransform(), newClaptrap->GetBoundingVolume()));

	newClaptrap->GetPhysicsObject()->SetInverseMass(0);
	newClaptrap->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(newClaptrap);

	return newClaptrap;
}

GameItem* TutorialGame::AddGameItemToWorld(const Vector3& position, float radius, float inverseMass, string name) {
	GameItem* newGameItem = new GameItem(name);
	SphereVolume* volume = new SphereVolume(radius);
	newGameItem->SetBoundingVolume((CollisionVolume*)volume);
	newGameItem->GetTransform()
		.SetScale(Vector3(radius, radius, radius))
		.SetPosition(position);

	newGameItem->SetRenderObject(new RenderObject(&newGameItem->GetTransform(), sphereMesh, nullptr, basicShader));
	newGameItem->SetPhysicsObject(new PhysicsObject(&newGameItem->GetTransform(), newGameItem->GetBoundingVolume()));

	newGameItem->GetPhysicsObject()->SetInverseMass(inverseMass);
	newGameItem->GetPhysicsObject()->InitSphereInertia();

	newGameItem->SetDefultDimension(Vector3(radius, radius, radius));
	newGameItem->SetDefultVolume((CollisionVolume*)volume);

	world->AddGameObject(newGameItem);

	return newGameItem;
}

Goose* TutorialGame::AddGooseToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Goose* newGoose = new Goose("Goose");
	AABBVolume* volume = new AABBVolume(dimensions);
	newGoose->SetBoundingVolume((CollisionVolume*)volume);
	newGoose->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions);

	newGoose->SetRenderObject(new RenderObject(&newGoose->GetTransform(), gooseMesh, nullptr, basicShader));
	newGoose->SetPhysicsObject(new PhysicsObject(&newGoose->GetTransform(), newGoose->GetBoundingVolume()));

	newGoose->GetPhysicsObject()->SetInverseMass(inverseMass);
	newGoose->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(newGoose);

	return newGoose;
}

void  TutorialGame::InitLevelMaze() {
	sceneMaze = new SimpleMaze();
	//std::cout << sceneMaze->GetGridWidth();
	sceneMaze->GenerateMazePos("MazeGrid.txt");
	//std::cout << "测试一下行不行:" << sceneMaze->GetGridHeight() << "\n";
	
	size_t size = std::min(sceneMaze->mazePos.size(), sceneMaze->types.size());

	auto posIter = sceneMaze->mazePos.begin();
	auto typeIter = sceneMaze->types.begin();

	int nodeSizes = sceneMaze->GetNodeSize();

	for (size_t i = 0; i < size; ++i) {
		Vector3* pos = *posIter;
		char ch = *typeIter;

		/*if (i == 37) {
			cout << "37的钥匙位置为：" << pos->x << " " << pos->z << "\n";
		}
		else if (i == 198) {
			cout << "198的钥匙位置为：" << pos->x << " " << pos->z << "\n";
		}
		else if (i == 105) {
			cout << "105的钥匙位置为：" << pos->x << " " << pos->z << "\n";
		}*/


		if (ch == 'x') {
			AddWallToWorld(*pos, Vector3(nodeSizes / 2, nodeSizes, nodeSizes / 2));
		}

		++posIter;
		++typeIter;

		
	}
}

Vector3 TutorialGame::CheckPlayerPos() {
	size_t size = std::min(sceneAIMaze->mazePos.size(), sceneAIMaze->types.size());

	auto posIter = sceneAIMaze->mazePos.begin();
	auto typeIter = sceneAIMaze->types.begin();

	int nodeSizes = sceneAIMaze->GetNodeSize();

	Vector3 posP = player->GetTransform().GetPosition();
	Vector3 dis;
	Vector3 target;
	float d;
	float mind = 100;
	for (size_t i = 0; i < size; ++i) {
		Vector3* pos = *posIter;
		char ch = *typeIter;

		dis = posP - (*pos + Vector3(-250,0,0));
		d = dis.Length();
		if (d < mind && ch!='x') {
			mind = d;
			target = *pos;
		}

		++posIter;
		++typeIter;
	}
	return target + Vector3(-250, 0, 0);
}

void TutorialGame::InitLevelAI() {
	sceneAIMaze = new SimpleMaze();
	sceneAIMaze->GenerateMazePos("GooseGrid.txt");

	size_t size = std::min(sceneAIMaze->mazePos.size(), sceneAIMaze->types.size());

	auto posIter = sceneAIMaze->mazePos.begin();
	auto typeIter = sceneAIMaze->types.begin();

	int nodeSizes = sceneAIMaze->GetNodeSize();

	for (size_t i = 0; i < size; ++i) {
		Vector3* pos = *posIter;
		char ch = *typeIter;

		if (i == 0) {
			cout << "0的位置为：" << pos->x << " " << pos->z << "\n";
		}
		if (i == 14) {
			cout << "14的位置为：" << pos->x << " " << pos->z << "\n";
		}
		if (i == 134) {
			cout << "134的位置为：" << pos->x << " " << pos->z << "\n";
		}
		if (i == 149) {
			cout << "149的位置为：" << pos->x << " " << pos->z << "\n";
		}

		if (ch == 'x') {
			AddWallToWorld(*pos + Vector3(-250, 0, 0), Vector3(nodeSizes / 2, nodeSizes, nodeSizes / 2));
		}

		++posIter;
		++typeIter;
	}
}

//添加物理系统
void TutorialGame::InitLevelPhysics() {
	AddObstacleToWorld(Vector3(-160, 5, 120), Vector3(4, 5, 20), 0);
	GameItem* coin = AddGameItemToWorld(Vector3(-180, 10, 120), 1.0f, 2.0f, "Coin");
	coin->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));
	coin->SetDefaultColor(Vector3(1.0f, 0.8432f, 0.0f));
	gameItems.push_back(coin);

	for (int x = -120; x <= -100; x += 10) {
		for (int z = 80; z <= 100; z += 10) {
			if (x == -110 && z == 90) {
				GameItem* coin2 = AddGameItemToWorld(Vector3(x, 10, z), 1.0f, 2.0f, "Coin");
				coin2->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));
				coin2->SetDefaultColor(Vector3(1.0f, 0.8432f, 0.0f));
				gameItems.push_back(coin2);
			}
			else {
				AddObstacleToWorld(Vector3(x, 5, z), Vector3(5, 5, 5), 0);
			}
		}
	}


	AddObstacleToWorld(Vector3(-100, 5, 60), Vector3(4, 15, 4));
	GameItem* coin3 = AddGameItemToWorld(Vector3(-100, 35, 60), 1.0f, 2.0f, "Coin");
	coin3->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));
	coin3->SetDefaultColor(Vector3(1.0f, 0.8432f, 0.0f));
	gameItems.push_back(coin3);


	AddObstacleToWorld(Vector3(-100, 5, 160), Vector3(4, 15, 4));
	GameItem* coin4 = AddGameItemToWorld(Vector3(-100, 35, 160), 1.0f, 2.0f, "Coin");
	coin4->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));
	coin4->SetDefaultColor(Vector3(1.0f, 0.8432f, 0.0f));
	gameItems.push_back(coin4);
	
}

void TutorialGame::InitSquare() {
	goat = AddGoatToWorld(Vector3(120, 5, 120));
	goat->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	

}

//添加游戏对象
void TutorialGame::AddPlayer() {
	player = AddPlayerToWorld(Vector3(15, 5, 0), 1);
	player->GetRenderObject()->SetColour(Vector4(0,0.75f,1,1));
}

void TutorialGame::AddGoose() {
	goose = AddGooseToWorld(Vector3(-190, 10, -190), Vector3(1, 1, 1));
	goose->GetRenderObject()->SetColour(Vector4(1.0f, 0.5f, 0.0f, 1.0f));
	goose->SetDefaultColor(Vector4(1.0f, 0.5f, 0.0f, 1.0f));
	goose->SetSafeModeColor(Vector4(0.0f, 0.5f, 1.0f, 1.0f));
}

void TutorialGame::AddOBBs() {
	GameObject* cube1 = AddCubeToWorld(Vector3(120, 10, 80), Vector3(2, 2, 2));
	OBBVolume* vol = new OBBVolume(Vector3(2, 2, 2));
	cube1->SetBoundingVolume((CollisionVolume*)vol);

	GameObject* cube2 = AddCubeToWorld(Vector3(120, 10, 100), Vector3(2, 2, 2));
	OBBVolume* vol1 = new OBBVolume(Vector3(2, 2, 2));
	cube2->SetBoundingVolume((CollisionVolume*)vol1);


	AddObstacleToWorld(Vector3(-160, 5, 120), Vector3(4, 5, 20), 0);

	for (int y = 5; y <= 15; y += 10) {
		for (int x = 60; x <= 70; x += 10) {
			for (int z = 60; z <= 70; z += 10) {
				GameObject* ObbObstacle = AddObstacleToWorld(Vector3(x, y, z), Vector3(1, 1, 1));
				OBBVolume* vol3 = new OBBVolume(Vector3(2, 2, 2));
				ObbObstacle->SetBoundingVolume((CollisionVolume*)vol3);
			}
		}
	}
}

void TutorialGame::AddEnemy() {

	for (int i = 0; i < 20; i++) {
		//srand(static_cast<unsigned int>(time(0)));
		/*time_t currentTime = time(0);
		srand(static_cast<unsigned int>(currentTime));*/

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<int> distribution(-180, -60);

		int randomPosX = distribution(gen);
		int randomPosZ = distribution(gen);

		//int randomPosX = rand() % (160 + 1) - 195;
		//int randomPosZ = rand() % (160 + 1) - 195;

		StateGameObject* enemy = AddStateObjectToWorld(Vector3(randomPosX, 25, randomPosZ));
		enemys.push_back(enemy);
	}
}

void TutorialGame::AddClaptrap() {
	claptrap = AddClaptrapToWorld(Vector3(50, 5, -120), Vector3(1, 1, 1));
	claptrap->GetRenderObject()->SetColour(Vector4(1, 0.8f, 0.2f, 1));
	claptrap->SetInitColor(Vector4(1, 0.8f, 0.2f, 1));
	//claptrap->ClaptrapPathfinding(Vector3(70, 0, 20), Vector3(30, 0, 130));

	claptrap->SetPathID(1);
	claptrap->ClaptrapPathfinding(Vector3(0, 0, 70), Vector3(70, 0, 20));

	claptrap->SetPathID(2);
	claptrap->ClaptrapPathfinding(Vector3(70, 0, 20), Vector3(30, 0, 130));

	claptrap->SetPathID(3);
	claptrap->ClaptrapPathfinding(Vector3(30, 0, 130), Vector3(0, 0, 70));

	claptrap->ClaptrapBehaviourTree();
	
}

void TutorialGame::AddGameItem() {
	//添加金币
	/*GameItem* coin = AddGameItemToWorld(Vector3(-120, 10, -180), 1.0f, 2.0f, "Coin");
	coin->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));*/
	
	for (int x = -180; x <= -60; x += 60) {
		for (int z = -180; z <= -60; z += 60) {
			GameItem* coin = AddGameItemToWorld(Vector3(x, 10, z), 1.0f, 2.0f, "Coin");
			coin->GetRenderObject()->SetColour(Vector4(1.0f, 0.8432f, 0.0f, 1.0f));
			coin->SetDefaultColor(Vector3(1.0f, 0.8432f, 0.0f));
			gameItems.push_back(coin);
		}
	}
	GameItem* healthPack = AddGameItemToWorld(Vector3(-150, 10, -150), 1.0f, 2.0f, "HealthPack");
	healthPack->GetRenderObject()->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	healthPack->SetDefaultColor(Vector3(1.0f, 0.0f, 0.0f));
	gameItems.push_back(healthPack);

	GameItem* powerUp = AddGameItemToWorld(Vector3(-150, 10, -90), 1.0f, 2.0f, "PowerUp");
	powerUp->GetRenderObject()->SetColour(Vector4(0.8f, 0.4f, 0.0f, 1.0f));
	powerUp->SetDefaultColor(Vector3(0.8f, 0.4f, 0.0f));
	gameItems.push_back(powerUp);

	GameItem* damageImmunity = AddGameItemToWorld(Vector3(-90, 10, -150), 1.0f, 2.0f, "DamageImmunity");
	damageImmunity->GetRenderObject()->SetColour(Vector4(0.0f, 0.2f, 0.8f, 1.0f));
	damageImmunity->SetDefaultColor(Vector3(0.0f, 0.2f, 0.8f));
	gameItems.push_back(damageImmunity);

	GameItem* speedUp = AddGameItemToWorld(Vector3(-90, 10, -90), 1.0f, 2.0f, "SpeedUp");
	speedUp->GetRenderObject()->SetColour(Vector4(0.5f, 0.0f, 0.5f, 1.0f));
	speedUp->SetDefaultColor(Vector3(0.5f, 0.0f, 0.5f));
	gameItems.push_back(speedUp);
	

	
}
//************

void TutorialGame::LockCameraToPlayer() {
	if (Window::GetKeyboard()->KeyDown(NCL::KeyCodes::Z)) {
		inPlayerMode = true;
		lockedObject = player;
		//selectionObject = player;
		//m->DrawPlayerModeInfo(inPlayerMode);
	}

	if (Window::GetKeyboard()->KeyDown(NCL::KeyCodes::X)) {
		inPlayerMode = false;
		lockedObject = nullptr;
		//m->DrawPlayerModeInfo(inPlayerMode);
		//selectionObject = nullptr;
	}
}

void TutorialGame::ShootGrapplingHook(PlayerShootType playerShootType) {
	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyHeld(KeyCodes::V)) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = player->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = player->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		GameObject* target = nullptr;

		if (world->Raycast(r, closestCollision, true, player)) {
			target = (GameObject*)closestCollision.node;
			//target->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
			/*if (target->GetName() == "Obstacle") {
				
			}*/
			Debug::DrawLine(player->GetTransform().GetPosition(), target->GetTransform().GetPosition(), Vector4(1, 0, 1, 1));
		}

		if (target) {
			switch (playerShootType) {
			case(PUSH):ShootAndPush(target, r, closestCollision); break;
			case(PULL):ShootAndPull(target, r, closestCollision); break;
			case(SWING):ShootAndSwing(r,target); break;
			case(DESTROY):ShootAndDestroy(target); break;
			}
			
		}
		
	}
}

void TutorialGame::ShootAndDestroy(GameObject* rayObject) {

	if (rayObject->GetName() == "Obstacle") {
		world->RemoveGameObject(rayObject);
	}
}

void TutorialGame::ShootAndPush(GameObject* rayObject, Ray ray, RayCollision rayCollision) {
	if (rayObject->GetName() == "Obstacle") {
		rayObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * 100.0f, rayCollision.collidedAt);
	}
	
}

void TutorialGame::ShootAndPull(GameObject* rayObject, Ray ray, RayCollision rayCollision) {
	if (rayObject->GetName() == "Obstacle") {
		rayObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * (-100.0f), rayCollision.collidedAt);
	}
}

void TutorialGame::ShootAndSwing(Ray ray, GameObject* rayObject) {
	if (rayObject->GetName() == "Obstacle") {
		player->GetPhysicsObject()->AddForce((ray.GetDirection() + Vector3(0, 1, 0)) * 200.0f);
	}
	
}

void TutorialGame::ShootToPlayer(StateGameObject* enemy) {
	RayCollision closestCollision;
	Vector3 rayPos;
	Vector3 rayDir;

	rayDir = enemy->GetTransform().GetOrientation() * Vector3(0, 0, -1);

	rayPos = enemy->GetTransform().GetPosition();

	Ray r = Ray(rayPos, rayDir);

	Vector3 toPlayer = player->GetTransform().GetPosition() - rayPos;

	float dir = Vector3::Dot(toPlayer, rayDir);
	float dis = toPlayer.Length();
	//(player->GetTransform().GetPosition().x < -50 && player->GetTransform().GetPosition().z < -50)
	if (dir >= 0 && dis <= 10){
		Debug::DrawLine(enemy->GetTransform().GetPosition(), player->GetTransform().GetPosition(), Vector4(1, 0, 0.5f, 1));
		player->GetPhysicsObject()->AddForce((r.GetDirection() + Vector3(0, 1, 0)) * 200.0f);
	}

	//if (world->Raycast(r, closestCollision, true)) {
	//	target = (GameObject*)closestCollision.node;
	//	//target->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	//	if (target->GetName() == "Player") {
	//		player->GetPhysicsObject()->AddForce((r.GetDirection() + Vector3(0, 1, 0)) * 200.0f);
	//	}
	//	Debug::DrawLine(enemy->GetTransform().GetPosition(), target->GetTransform().GetPosition(), Vector4(1, 0, 0.5f, 1));
	//}

}

void TutorialGame::UpdatePlayerPosition(float dt) {
	playerYaw -= Window::GetMouse()->GetRelativePosition().x;
	playerPitch -= Window::GetMouse()->GetRelativePosition().y;
	Quaternion qx = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), playerYaw);
	Quaternion qy = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), playerPitch);
	Quaternion q = qx * qy;
	player->GetTransform().SetOrientation(q);

	float frameSpeed = 40 * dt;

	if (inPlayerMode) {
		Vector3 currentPos = player->GetTransform().GetPosition();
		if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::W)) {
			//player->GetTransform().SetPosition(currentPos + fwdAxis * frameSpeed);
			player->GetPhysicsObject()->AddForce(
				player->GetTransform().GetOrientation() *
				Vector3(0, 0, -1) * 50
			);
		}
		if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::A)) {
			//player->GetTransform().SetPosition(currentPos - rightAxis * frameSpeed);
			player->GetPhysicsObject()->AddForce(
				player->GetTransform().GetOrientation() *
				Vector3(-1, 0, 0) * 50
			);
		}
		if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::S)) {
			//player->GetTransform().SetPosition(currentPos - fwdAxis * frameSpeed);
			player->GetPhysicsObject()->AddForce(
				player->GetTransform().GetOrientation() *
				Vector3(0, 0, 1) * 50
			);
		}
		if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::D)) {
			//player->GetTransform().SetPosition(currentPos + rightAxis * frameSpeed);
			player->GetPhysicsObject()->AddForce(
				player->GetTransform().GetOrientation() *
				Vector3(1, 0, 0) * 50
			);
		}

		if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::SPACE)) {
			player->GetPhysicsObject()->AddForce(Vector3(0, 400, 0));
		}
	}
}
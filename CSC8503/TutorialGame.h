#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "StateGameObject.h"
#include "GameMenu.h"
#include "Player.h"
#include "SimpleMaze.h"
#include "Claptrap.h"
#include "GameItem.h"
#include "Goose.h"

namespace NCL {
	namespace CSC8503 {

		enum PlayerShootType
		{
			PUSH,
			PULL,
			SWING,
			DESTROY
		};

		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			//开始搭建游戏场景
			void InitSquare();//全部的游戏场景
			void InitStaticWall();//生成墙
			void InitLevelPhysics();//物理系统的关卡
			void InitLevelAI();//敌对AI的关卡
			void InitLevelMaze();//迷宫关卡
			

			//游戏内对象的加载
			void AddPlayer();
			void AddEnemy();
			void AddClaptrap();
			void AddGameItem();
			void AddGoose();
			void AddOBBs();

			//将相机视角切换到玩家
			void LockCameraToPlayer();

			//控制玩家移动
			void UpdatePlayerPosition(float dt);

			//玩家发射射线
			void ShootGrapplingHook(PlayerShootType playerShootType);
			void ShootAndPush(GameObject* rayObject, Ray ray, RayCollision rayCollision);
			void ShootAndPull(GameObject* rayObject, Ray ray, RayCollision rayCollision);
			void ShootAndSwing(Ray ray, GameObject* rayObject);
			void ShootAndDestroy(GameObject* rayObject);

			void ShootToPlayer(StateGameObject* enemy);
			

			//计算玩家在左下角地图的位置
			Vector3 CheckPlayerPos();




			//Physics - Constraints and Solvers代码
			void BridgeConstraintTest();

			void InitDefaultFloor();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameObject* AddGoatToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions);
			Player* AddPlayerToWorld(const Vector3& position, float radius, float inverseMass = 0.4f);
			Claptrap* AddClaptrapToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 2.0f);
			GameItem* AddGameItemToWorld(const Vector3& position, float radius, float inverseMass, string name);
			Goose* AddGooseToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 2.0f);
			GameObject* AddObstacleToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);



			//AI - State Machines 的代码
			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject;

			GameMenu* m;
			Player* player;
			bool inPlayerMode;

			vector<StateGameObject*> enemys;
			vector<GameItem*> gameItems;

			SimpleMaze* sceneMaze;
			SimpleMaze* sceneAIMaze;

			Claptrap* claptrap;

			float playerYaw;
			float playerPitch;

			Goose* goose;

			PlayerShootType playerShootType;

			GameObject* goat;

			float leftTime;

			bool startMenu = true;

			int highLevelEnemy = 0;


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			KeyboardMouseController controller;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;

			Texture*	basicTex	= nullptr;
			Shader*		basicShader = nullptr;

			//Coursework Meshes
			Mesh*	charMesh	= nullptr;
			Mesh*	enemyMesh	= nullptr;
			Mesh*	bonusMesh	= nullptr;
			Mesh*   gooseMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
		};
	}
}


#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

#include <iostream>
#include <random>

using namespace NCL;
using namespace CSC8503;
using namespace std;

StateGameObject::StateGameObject(const std::string& objectName) :GameObject(name) {
    counter = 0.0f;

    name = objectName;

    //随机生成对象的方向
    /*srand(static_cast<unsigned int>(time(0)));
    initMoveDir = rand() % 2;*/
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 1);
    initMoveDir = distribution(gen);

    stateMachine = new StateMachine();

    State* stateA = new State([&](float dt) -> void {
        this->MoveLeft(dt);
        });

    State* stateB = new State([&](float dt) -> void {
        this->MoveRight(dt);
        });

    State* stateC = new State([&](float dt) -> void {
        this->MoveForward(dt);
        });

    State* stateD = new State([&](float dt) -> void {
        this->MoveBackward(dt);
        });

    if (initMoveDir) {
        stateMachine->AddState(stateA);
        stateMachine->AddState(stateB);

        stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]() -> bool {
            return this->counter > 3.0f;
            }));

        stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]() -> bool {
            return this->counter < 0.0f;
            }));

        movingDir = 1;
    }
    else {
        stateMachine->AddState(stateC);
        stateMachine->AddState(stateD);

        stateMachine->AddTransition(new StateTransition(stateC, stateD, [&]() -> bool {
            return this->counter > 3.0f;
            }));

        stateMachine->AddTransition(new StateTransition(stateD, stateC, [&]() -> bool {
            return this->counter < 0.0f;
            }));

        movingDir = 3;
    }

    currentDir = movingDir;
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
    stateMachine->Update(dt);

    //当方向发生改变时
    if (currentDir != movingDir) {
        ChangeMoveDir(180);
        currentDir = movingDir;
    }
}

void StateGameObject::ChangeMoveDir(float degrees) {
    Quaternion currentOrientation = this->GetTransform().GetOrientation();
    Vector3 rotationAxis(0.0f, 1.0f, 0.0f);
    //float degrees = 90.0f;
    Quaternion rotationQuaternion = Quaternion::AxisAngleToQuaterion(rotationAxis, degrees);
    Quaternion rotatedQuaternion = currentOrientation * rotationQuaternion;
    this->GetTransform().SetOrientation(rotatedQuaternion);
}

//void StateGameObject::ChangeMoveDir(Vector3 movingDir) {
//    Quaternion rotationQuaternion = Quaternion::AxisAngleToQuaterion(movingDir, 0.0f);
//    this->GetTransform().SetOrientation(rotationQuaternion);
//}

void StateGameObject::MoveLeft(float dt) {
    GetPhysicsObject()->AddForce({ -30, 0, 0 });
    counter += dt;
    movingDir = 1;
    //std::cout << "正在向左移动！\n";
}

void StateGameObject::MoveRight(float dt) {
    GetPhysicsObject()->AddForce({ 30, 0, 0 });
    counter -= dt;
    movingDir = 2;
    //std::cout << "正在向右移动！\n";
}

void StateGameObject::MoveForward(float dt) {
    GetPhysicsObject()->AddForce({ 0, 0, 30 });
    counter += dt;
    movingDir = 3;
    //std::cout << "正在向前移动！\n";
}

void StateGameObject::MoveBackward(float dt) {
    GetPhysicsObject()->AddForce({ 0, 0, -30 });
    counter -= dt;
    movingDir = 4;
    //std::cout << "正在向后移动！\n";
}
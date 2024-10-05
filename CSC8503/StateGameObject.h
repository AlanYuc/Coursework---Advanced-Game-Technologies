#pragma once
#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject(const std::string& objectName);
            ~StateGameObject();

            virtual void Update(float dt);

            //void SetMoveDir(bool toggleMoveDir) { this->toggleMoveDir = toggleMoveDir; }
            int GetInitMoveDir() { return initMoveDir; }

            void ChangeMoveDir(float degrees);
            //void ChangeMoveDir(Vector3 movingDir);

        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);

            void MoveForward(float dt);
            void MoveBackward(float dt);

            

            StateMachine* stateMachine;
            float counter;

            //初始方向的设置，1为x轴方向，0为z轴方向
            int initMoveDir;

            //移动时的方向设置,1,2,3,4分别表示左右前后
            int movingDir;
            //记录当前方向
            int currentDir;
        };
    }
}

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

            //��ʼ��������ã�1Ϊx�᷽��0Ϊz�᷽��
            int initMoveDir;

            //�ƶ�ʱ�ķ�������,1,2,3,4�ֱ��ʾ����ǰ��
            int movingDir;
            //��¼��ǰ����
            int currentDir;
        };
    }
}

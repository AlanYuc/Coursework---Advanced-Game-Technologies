#pragma once
#include "BehaviourNodeWithChildren.h"

class BehaviourSelector: public BehaviourNodeWithChildren {
public:
	BehaviourSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~BehaviourSelector() {}
	BehaviourState Execute(float dt) override {
		//std::cout << "Executing selector " << name << "\n";
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);
			//���ݽڵ�״̬���أ�����ڵ�״̬�ǳɹ����������У����޸ĵ�ǰ״̬
			//������еĽڵ㶼��ʧ�ܣ����շ���ʧ�ܣ��κθ���һ���Ľڵ㶼��������һ֡������
			switch (nodeState) {
				case Failure: continue;
				case Success:
				case Ongoing:
				{
					currentState = nodeState;
					return currentState;
				}
			}
		}
		return Failure;
	}
};
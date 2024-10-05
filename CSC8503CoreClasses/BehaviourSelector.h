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
			//根据节点状态返回，如果节点状态是成功或正在运行，就修改当前状态
			//如果所有的节点都是失败，最终返回失败，任何更进一步的节点都不会在这一帧中运行
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
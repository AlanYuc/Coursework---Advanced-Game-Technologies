#pragma once
#include "BehaviourNodeWithChildren.h"

class BehaviourSequence : public BehaviourNodeWithChildren {
public:
	BehaviourSequence(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~BehaviourSequence() {}
	BehaviourState Execute(float dt) override {
		//std::cout << "Executing sequence " << name << "\n";
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);

			//序列节点和选择节点的顺序有所不同，
			switch (nodeState) {
				case Success: continue;
				case Failure:
				case Ongoing:
				{
					currentState = nodeState;
					return nodeState;
				}
			}
		}
		return Success;
	}
};
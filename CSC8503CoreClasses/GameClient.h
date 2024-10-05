#pragma once
#include "NetworkBase.h"
#include <stdint.h>
#include <thread>
#include <atomic>

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class GameClient : public NetworkBase {
		public:
			GameClient();
			~GameClient();

			bool Connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int portNum);

			//这句在调用时会出现，非常量引用必须是左值的报错
			//因为在调用时，传递的参数是在main.cpp里临时创建的
			//void SendPacket(GamePacket&  payload);
			void SendPacket(GamePacket& payload);

			void UpdateClient();
		protected:	
			_ENetPeer*	netPeer;
		};
	}
}


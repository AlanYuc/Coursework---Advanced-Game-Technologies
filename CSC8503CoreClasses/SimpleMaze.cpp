#include "SimpleMaze.h"
#include "Assets.h"
#include <fstream>
#include <iostream>

using namespace NCL::CSC8503;
using std::string;

SimpleMaze::SimpleMaze() {
	nodeSize = 0;
	gridWidth = 0;
	gridHeight = 0;
}

void SimpleMaze::GenerateMazePos(const std::string& name) {
	std::ifstream infile(Assets::DATADIR + name);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	char type;

	for (int z = 0; z < gridHeight; ++z) {
		for (int x = 0; x < gridWidth; ++x) {
			infile >> type;
			types.push_back(type);
			

			int posX, posZ;
			posX = x * nodeSize + 50;
			posZ = z * nodeSize - 190;

			//n.position = Vector3((float)(x * nodeSize), 0, (float)(y * nodeSize));

			//调整迷宫的位置到左上房间里
			//n.position += Vector3(90, 10, -180);
			//mazePos[x][z] = Vector3(posX, 20, posZ);
			Vector3* vectorToAdd = new Vector3(posX, 5, posZ);
			mazePos.push_back(vectorToAdd);
			std::cout << "当前读取的是" << posX << " " << posZ << "\n";
		}
	}
}
#pragma once

#include "NavigationGrid.h"
#include "NavigationMesh.h"

using std::vector;

namespace NCL::CSC8503 {
	class SimpleMaze {
	public:
		SimpleMaze();
		~SimpleMaze() {};

		void GenerateMazePos(const std::string& name);

		int GetGridWidth() { return gridWidth; }
		int GetGridHeight() { return gridHeight; }
		int GetNodeSize() { return nodeSize; }
		//Vector3 GetMazePos() { return mazePos; }
		//const vector<Vector3>& GetMazePos() const { return mazePos; }

		vector<Vector3*> mazePos;
		vector<char> types;

	protected:
		int nodeSize;
		int gridWidth;
		int gridHeight;
		//Vector3 mazePos[20][20];
		
	};
}
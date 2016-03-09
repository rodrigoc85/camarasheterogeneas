//#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "masterPly.h"

class Model_PLY
{
	private:
        int* numberFaces;
        FaceStruct* faces;

        HINSTANCE shareMeshLibrary;

		float* Faces_Quads;
		float* Vertex_Buffer;
		float* Normals;

	public:
		Model_PLY();
		bool MemoryLoad();
		void Load(string filename);

		float* Faces_Triangles;

		int TotalConnectedTriangles;
		int TotalPoints;
		int TotalFaces;
		int Id;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};


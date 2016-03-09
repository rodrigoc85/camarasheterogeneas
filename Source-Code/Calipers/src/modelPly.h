#ifndef MODELPLY_H
#define MODELPLY_H

#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "masterPly.h"

class Model_PLY
{
	public:
	    ofxSharedMemory<int*> memoryMappedMeshId;
        int* id;
        ofxSharedMemory<FaceStruct*> memoryMappedMesh;
        FaceStruct* faces;
	    ofxSharedMemory<int*> memoryMappedMeshSize;
        int* numberFaces;

        bool isConnectedId;
        bool isConnectedFaces;
        bool isConnectedNFaces;

		Model_PLY();
		void MemoryLoadCalibrator(FaceStruct* faces, int numberFaces);
		void MemoryLoad();
		void Load(string filename);
		float* calculateNormal( float *coord1, float *coord2, float *coord3 );

		float* Faces_Quads;
		float* Vertex_Buffer;
		float* Normals;

		float* Faces_Triangles;

		int TotalConnectedTriangles;
		int TotalPoints;
		int TotalFaces;
		int Id;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

#endif


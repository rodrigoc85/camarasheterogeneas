#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "MainBuffer/MainBuffer.h"
#include "MainBuffer/MainBufferRT.h"
#include "ServerGlobalData.h"
#include "MeshThreadedGenerator.h"
#include "MeshCollector.h"
#include "IServer.h"
//#include "ofxSharedMemory.h"

/*struct FaceStruct
{
    float p1[3];
    float p2[3];
    float p3[3];
};*/

class MeshGenerator : public ofThread {
	public:
        MeshGenerator() {
            buffer = NULL;
            nframe          = 0;
            currTProcesor   = 0;
            __idle          = true;
            started         = false;
            b_exit          = false;
        }

        ~MeshGenerator() {
            if(!started) return;
            delete threads;
            delete collector;
            ofRemoveListener(ofEvents().update, this, &MeshGenerator::processFrame);
        }

        //HINSTANCE generateMeshLibrary;
        //HINSTANCE memorySharedLibrary;
		void threadedFunction();
		void processFrame(ofEventArgs &e);
		void processFrame();
		void exit();
		MainBufferRT * buffer;
		IServer * srvinst;
		MeshThreadedGenerator * threads;
		MeshCollector * collector;
		t_data * sys_data;
        int nframe;
        int currTProcesor;
        //ofxSharedMemory<int*> nFacesMemoryMappedFile;
        //int* numberFaces;

        //ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;
        //FaceStruct* faces;

        bool isConnected;
        bool __idle;
        bool started;
        bool b_exit;
};

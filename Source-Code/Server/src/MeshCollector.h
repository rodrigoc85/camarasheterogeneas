#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "ServerGlobalData.h"
#include "MeshThreadedGenerator.h"
#include "FreeImage.h"
#include <iostream>
#include <list>
#include <string>
#include <cctype>

typedef void (*f_ShareMesh)(int* meshId, int* numberFaces, FaceStruct* faces, int* index);
typedef void (*f_ShareMeshSetup)(int* meshId, int* index);
typedef void (*f_ShareImage)(int* imageId, unsigned char* pixels, int* index);
typedef void (*f_ShareImageSetup)(int* imageId, int* wPixels, int* hPixels, int* index);

class MeshCollector : public ofThread {
	public:

        MeshCollector() {
            currProc   = 0;
            currFrame  = -1;
            char* dllName = "MeshGenerator.dll";
            generateMeshLibrary =  LoadLibraryA(dllName);
            if (!generateMeshLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }

            dllName = "SharedMemory.dll";
            memorySharedLibrary =  LoadLibraryA(dllName);
            if (!memorySharedLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }
            b_exit = false;
        }

        ~MeshCollector() {
        }

        HINSTANCE generateMeshLibrary;
        HINSTANCE memorySharedLibrary;

        t_data * sys_data;
        MeshThreadedGenerator * threads;
        int currProc;
        int currFrame;
        f_ShareMesh ShareMesh;
        f_ShareMeshSetup ShareMeshSetup;
        f_ShareImage ShareImage;
        f_ShareImageSetup ShareImageSetup;
        std::list<GeneratedResult *> list;
        std::list<GeneratedResult *>::iterator it;

        int outMeshId;
        std::list<RGBDataOutput *> outListRGB;
        std::list<RGBDataOutput *>::iterator outItRGB;

		void threadedFunction();
		void processFrame(ofEventArgs &e);
		void processFrame();
		void shareNextCompleteFrame();
		void shareFrame(GeneratedResult *);
		void exit();
		bool b_exit;
};

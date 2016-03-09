#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>

class Model_IMG
{
	public:
	    ofxSharedMemory<int*> memoryMappedImageId;
        int* id;
        ofxSharedMemory<char*> memoryMappedImage;
        char* pixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeW;
        int* wPixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeH;
        int* hPixels;

        bool isConnectedId;
        bool isConnectedPixels;
        bool isConnectedWPixels;
        bool isConnectedHPixels;

		Model_IMG();
		void MemoryLoad();
		void Load(string filename);

        int Id;
		char* Pixels;
		int Width;
		int Height;
};


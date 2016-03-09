//#include "ofxSharedMemory.h"
#include "ofMain.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "FreeImage.h"

class Model_IMG
{
	private:
        unsigned char* pixels;

        bool isConnectedId;
        bool isConnectedPixels;
        bool isConnectedWPixels;
        bool isConnectedHPixels;

        bool primeraVez;

        HINSTANCE shareImageLibrary;

	public:
		Model_IMG();
		bool MemoryLoad();
		bool MemoryCheck();
		void Load(string filename);

        int Id;
		unsigned char* Pixels;
		int Width;
		int Height;
};


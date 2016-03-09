#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "FreeImage.h"

class Model_SET
{
	private:

        HINSTANCE shareSettingLibrary;

        float* values;
        int* nValues;

        bool isConnectedValues;
        bool isConnectedNValues;

	public:
		Model_SET();
		void MemoryLoad();
		void FileLoad();

		float* ValuesA;
		float* ValuesB;
		int* IdsValues;
		int NValues;
        float alfaCoord;
};


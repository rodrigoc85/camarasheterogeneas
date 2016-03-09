#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glut.h>
#include "timer.h"

using namespace std;

class Model_XYZ
{
	public:
		Model_XYZ();
		int Load(string fileName, float alfa);
		int Include(Model_XYZ* model, GLdouble* m);
		void ToImage();
		void ToCloud();
		void Diff();
		int Clear();

		timer* tt;

		vector<float> Points;
		vector<unsigned char> PixelsX;
		vector<unsigned char> PixelsY;
		vector<unsigned char> PixelsZ;
		vector<float> NewPoints;

		int TotalPoints;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};


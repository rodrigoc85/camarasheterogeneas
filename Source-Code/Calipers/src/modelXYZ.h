#ifndef MODELXYZ_H
#define MODELXYZ_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glut.h>

using namespace std;

class Model_XYZ
{
	public:
		Model_XYZ();
		int Load(string fileName, float alfa, float* matrix);
		int Convert(int index, float* matrix);
		int Include(Model_XYZ* model, GLdouble* m);
		int Clear();

		vector<float> Points;
		vector<float> ColorPoints;

		float* tMatrix;
		float* transformPoints;

		bool hasColor;

		int TotalPoints;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

#endif

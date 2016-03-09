#include "modelXYZ.h"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>
#include "timer.h"


Model_XYZ::Model_XYZ() {
    tt = new timer();
}

int Model_XYZ::Load(string fileName, float alfa) {

	TotalPoints = 0;
    std::ifstream in(fileName.c_str());
    std::stringstream buffer;
    buffer << in.rdbuf();

    string line;
    while (getline(buffer, line, '\n')) {
        istringstream subBuffer(line);
        string point;
        for (int i = 0; i < 3 && getline(subBuffer, point, ' '); i++) {
            float value = ::atof(point.c_str());
            Points.push_back(value);
        }
        TotalPoints++;
    }
    in.close();

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();

    MinCoord = std::numeric_limits<float>::max();
    MaxCoord = std::numeric_limits<float>::min();
    for (int i = 0; i < TotalPoints * 3; i++) {
        if (Points[i] < MinCoord) {
            MinCoord = Points[i];
        }
        if (Points[i] > MaxCoord) {
            MaxCoord = Points[i];
        }
        if (i % 3 == 0) {
            if (Points[i] < minX) {
                minX = Points[i];
            }
            if (Points[i] > maxX) {
                maxX = Points[i];
            }
        }
        if (i % 3 == 1) {
            if (Points[i] < minY) {
                minY = Points[i];
            }
            if (Points[i] > maxY) {
                maxY = Points[i];
            }
        }
        if (i % 3 == 2) {
            if (Points[i] < minZ) {
                minZ = Points[i];
            }
            if (Points[i] > maxZ) {
                maxZ = Points[i];
            }
        }
    }

    if (alfa == 0) {
        AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    } else {
        AlfaCoord = alfa;
    }

    float deltaX = (maxX + minX) / 2;
    float deltaY = (maxY + minY) / 2;
    float deltaZ = (maxZ + minZ) / 2;
    for (int i = 0; i < TotalPoints * 3; i++) {
        if (i % 3 == 0) {
            Points[i] = Points[i] - deltaX;
        }
        if (i % 3 == 1) {
            Points[i] = Points[i] - deltaY;
        }
        if (i % 3 == 2) {
            Points[i] = Points[i] - deltaZ;
        }
        Points[i] = (Points[i] / AlfaCoord) * 10;
    }

	return TotalPoints;
}

int Model_XYZ::Include(Model_XYZ* model, GLdouble* m) {

    for (int i = 0; i < model->TotalPoints * 3; i += 3) {
        float x = model->Points[i];
        float y = model->Points[i+1];
        float z = model->Points[i+2];
        float w = 1;
        Points.push_back(m[0]*x + m[1]*y + m[2]*z + m[12]);
        Points.push_back(m[4]*x + m[5]*y + m[6]*z + m[13]);
        Points.push_back(m[8]*x + m[9]*y + m[10]*z + m[14]);
    }
    TotalPoints += model->TotalPoints;
	return TotalPoints;
}

void Model_XYZ::ToImage() {

    tt->start();

    for (int i = 0; i < TotalPoints * 3; i += 3) {
        int x = 1000000.f + Points[i] * 10.f;
        unsigned char xA = x / (256*256);
        unsigned char xB = (x/256) % 256;
        unsigned char xC = x % 256;
        PixelsX.push_back(xA);
        PixelsX.push_back(xB);
        PixelsX.push_back(xC);

        int y = 1000000.f + Points[i+1] * 10.f;
        unsigned char yA = y / (256*256);
        unsigned char yB = (y/256) % 256;
        unsigned char yC = y % 256;
        PixelsY.push_back(yA);
        PixelsY.push_back(yB);
        PixelsY.push_back(yC);

        int z = 1000000.f + Points[i+2] * 10.f;
        unsigned char zA = z / (256*256);
        unsigned char zB = (z/256) % 256;
        unsigned char zC = z % 256;
        PixelsZ.push_back(zA);
        PixelsZ.push_back(zB);
        PixelsZ.push_back(zC);
    }
    int timeX = tt->get_elapsed_ms();
    tt->start();
}

void Model_XYZ::ToCloud() {

    for (int i = 0; i < TotalPoints * 3; i += 3) {
        int x = PixelsX[i] * (256*256);
        x += PixelsX[i+1] * (256);
        x += PixelsX[i+2];
        x -= 1000000;
        float xF = ((double)x) / 10.f;
        NewPoints.push_back(xF);

        int y = PixelsY[i] * (256*256);
        y += PixelsY[i+1] * (256);
        y += PixelsY[i+2];
        y -= 1000000;
        float yF = ((double)y) / 10.f;
        NewPoints.push_back(yF);

        int z = PixelsZ[i] * (256*256);
        z += PixelsZ[i+1] * (256);
        z += PixelsZ[i+2];
        z -= 1000000;
        float zF = ((double)z) / 10.f;
        NewPoints.push_back(zF);
    }
    int timeX = tt->get_elapsed_ms();
    tt->start();
}

void Model_XYZ::Diff() {

    for (int i = 0; i < TotalPoints * 3; i++) {
        float fOld = Points[i];
        float fNew = NewPoints[i];
    }
}

int Model_XYZ::Clear() {

    TotalPoints = 0;
    Points.clear();
	return 0;
}

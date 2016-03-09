#include "modelXYZ.h"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>


Model_XYZ::Model_XYZ() {
    tMatrix = new float[16];

    tMatrix[0] = 0.271041; tMatrix[1] = 0.505676; tMatrix[2] = 0.819041; tMatrix[3] = -787.281;
    tMatrix[4] = -0.345301; tMatrix[5] = 0.845337; tMatrix[6] = -0.407643; tMatrix[7] = 298.289;
    tMatrix[8] = -0.898501; tMatrix[9] = -0.172328; tMatrix[10] = 0.403732; tMatrix[11] = 607.169;
    tMatrix[12] = 0; tMatrix[13] = 0; tMatrix[14] = 0; tMatrix[15] = 1;

    //0.289235 -0.34238 -0.893934 862.062 0.476605 0.86138 -0.175705 261.583 0.830175 -0.375234 0.412322 523.656 0 0 0 1

    /*tMatrix[0] = 0.289235; tMatrix[1] = -0.34238; tMatrix[2] = -0.893934; tMatrix[3] = 862.062;
    tMatrix[4] = 0.476605; tMatrix[5] = 0.86138; tMatrix[6] = -0.175705; tMatrix[7] = 261.583;
    tMatrix[8] = 0.830175; tMatrix[9] = -0.375234; tMatrix[10] = 0.412322; tMatrix[11] = 523.656;
    tMatrix[12] = 0; tMatrix[13] = 0; tMatrix[14] = 0; tMatrix[15] = 1;*/
}

int Model_XYZ::Convert(int index, float* matrix) {
    float w = matrix[3] + matrix[7] + matrix[11] + matrix[15];
    float px = Points[index * 3];
    float py = Points[index * 3 + 1];
    float pz = Points[index * 3 + 2];
    Points[index * 3] = px * matrix[0] + py * matrix[1] + pz * matrix[2] + matrix[3];
    Points[index * 3 + 1] = px * matrix[4] + py * matrix[5] + pz * matrix[6] + matrix[7];
    Points[index * 3 + 2] = px * matrix[8] + py * matrix[9] + pz * matrix[10] + matrix[11];

    if (index == 150000 / 3) {
        //cout << "original " << px << " " << py << " " << pz << endl;
        //cout << "transformado " << Points[index * 3] << " " << Points[index * 3 + 1] << " " << Points[index * 3 + 2] << endl;
    }

    return 0;
}

int Model_XYZ::Load(string fileName, float alfa, float* matrix) {

	TotalPoints = 0;
    std::ifstream in(fileName.c_str());
    std::stringstream buffer;
    buffer << in.rdbuf();

    string line;
    hasColor = false;
    while (getline(buffer, line, '\n')) {
        istringstream subBuffer(line);
        string point;
        for (int i = 0; i < 6 && getline(subBuffer, point, ' '); i++) {

            if (i < 3){
               float value = ::atof(point.c_str());
               Points.push_back(value);
           }
           else{
           //cout << "entro load" << endl;
               hasColor = true;
               float value = ::atof(point.c_str());
               ColorPoints.push_back(value);
           }
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

        if (i % 3 == 0 && i + 3 < TotalPoints * 3) {
            Convert(i / 3, matrix);
        }
        if (i == 150000) {
            //cout << "Original de Archivo: " << Points[i] << endl;
        }
        if (alfa == 0 && i % 3 == 0 && i + 3 < TotalPoints * 3) {//
            //Convert(i / 3);
        }
    }

    //cout << "alfa " << AlfaCoord << endl;

    for (int i = 0; i < TotalPoints * 3; i++) {

        Points[i] = (Points[i] / AlfaCoord) * 10;
        if (i == 150000) {
            //cout << "Dividido alfacoord " << Points[i] << endl;
        }
    }

	return TotalPoints;
}

int Model_XYZ::Include(Model_XYZ* model, GLdouble* m) {

    /*cout << "Z" << endl;
    for (int i = 0; i < 16; i++) {
        cout << m[i] << " ";
    }*/

    //cout << "alfa2222 " << AlfaCoord << endl;
    for (int i = 0; i < model->TotalPoints * 3; i += 3) {
       float x = model->Points[i];
       float y = model->Points[i+1];
       float z = model->Points[i+2];

       float w = 1;
       float newx = m[0]*x + m[1]*y + m[2]*z + m[12];
       float newy = m[4]*x + m[5]*y + m[6]*z + m[13];
       float newz = m[8]*x + m[9]*y + m[10]*z + m[14];
       /*float newx = m[0]*x + m[1]*y + m[2]*z + 10.0*m[3]/model->AlfaCoord;
       float newy = m[4]*x + m[5]*y + m[6]*z + 10.0*m[7]/model->AlfaCoord;
       float newz = m[8]*x + m[9]*y + m[10]*z + 10.0*m[11]/model->AlfaCoord;*/
       Points.push_back(newx);
       Points.push_back(newy);
       Points.push_back(newz);

        if (i == 150000) {
            //cout << "Antes de transformar: " << x << " " << y << " " << z << endl;
            //cout << "Despues de transformar: "<<newx << " " << newy << " " << newz << endl;
        }

       if(model->hasColor)
       {
           float r = model->ColorPoints[i];
           float g = model->ColorPoints[i+1];
           float b = model->ColorPoints[i+2];
           ColorPoints.push_back(m[0]*r + m[1]*g + m[2]*b + m[12]);
           ColorPoints.push_back(m[4]*r + m[5]*g + m[6]*b + m[13]);
           ColorPoints.push_back(m[8]*r + m[9]*g + m[10]*b + m[14]);
       }
       else
       {
           float r = 1;
           float g = 1;
           float b = 1;
           ColorPoints.push_back(m[0]*r + m[1]*g + m[2]*b + m[12]);
           ColorPoints.push_back(m[4]*r + m[5]*g + m[6]*b + m[13]);
           ColorPoints.push_back(m[8]*r + m[9]*g + m[10]*b + m[14]);
       }
    }
    TotalPoints += model->TotalPoints;
	return TotalPoints;
}

int Model_XYZ::Clear() {

    TotalPoints = 0;
    Points.clear();
    ColorPoints.clear();
    hasColor = true;
	return 0;
}

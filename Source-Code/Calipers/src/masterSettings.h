#ifndef MASTERSETTINGS_H
#define MASTERSETTINGS_H
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <vector>
#include "matrix4x4.h"
#include <stdlib.h>
#include <windows.h>

using namespace std;

struct MasterTransform {
    float value;
    int type;
};

struct MasterTexture {
    float viewer[3];
    float rotate[3];
    vector<MasterTransform*> history;
    GLdouble matrix[16];

    Matrix4x4f TextureTransform;
    float MVmatrix[16];
};

struct MasterIntrinsic {
    float k1;
    float k2;
    float k3;
    float k4;
    float fx;
    float fy;
    float cx;
    float cy;
};

struct MasterMesh {
    float viewer[3];
    float rotate[3];
    GLdouble matrix[16];
};

struct MasterCamera {
    int idCamera;
    bool is2D;
    MasterTexture* masterTexture;
    bool is3D;
    MasterMesh* masterMesh;
};

struct MasterClient {
    int idClient;
    vector<MasterCamera*> cameras;
};


class MasterSettings
{
    public:
        MasterSettings(int textureCount, MasterTexture* textureMaster, int meshCount, MasterMesh* meshMaster);
        void loadCalibration();
        void loadTextureCalibration();
        void loadMeshCalibration();
        void loadIntrinsicCalibration();
        //void saveTextureCalibration();
        void saveMeshCalibration();
        static void CalculateMatrix(MasterMesh master, GLdouble* m);
        static void CalculateMatrix(vector<MasterTransform*> history, GLdouble* m, bool flag);
        MasterMesh* meshMaster;
        MasterTexture* textureMaster;
        MasterIntrinsic* intrinsicMaster;
    protected:
    private:
        int textureCount;
        int meshCount;
};

#endif // MASTERSETTINGS_H

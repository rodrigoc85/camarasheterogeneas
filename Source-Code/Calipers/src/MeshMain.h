#ifndef MESHMAIN_H
#define MESHMAIN_H

#include "modelPly.h"
#include <stdlib.h>
#include <windows.h>
#include "masterSettings.h"
#include "modelXYZ.h"
#include "ofxXmlSettings.h"


struct NubePuntos{
    float* x;
    float* y;
    float* z;
    int largo;
};

class MeshMain
{
    public:

        bool REAL_TIME;
        int REAL_TIME_FPS;
        int REAL_TIME_PORT;


        GLfloat colors[7][3];

        MasterSettings* settings;

        Model_XYZ** cloudModel;//tienen la nube y ubicacion cuando fueron cargados
        MasterMesh* cloudMaster;//tienen la informacion de transformacion y ubicacion

        int meshCount;
        int meshIndex;
        bool generatingMesh;

        int cameraAxis;
        int cameraMove;
        bool cameraAll;

        bool cameraLight;

        FaceStruct* faces;
        int* numberFaces;

        float cameraFactor;

        int doubleClickTime;
        int clickCount;

        MeshMain();

        void display(void);
        void mouseMove(int x, int y);
        void mouse(int btn, int state, int x, int y);
        void keys(unsigned char key, int x, int y);

        virtual ~MeshMain();

    protected:
    private:
        void generarNubeUnida();
        void drawText(const char* text, int length, int x, int y);
        void drawAllText();
        void generarMalla(NubePuntos* nube);
        void saveXmlFile();
        void setPointVertex(int index, double* m);
        void draw3D();
        void IncludeMesh(Model_XYZ* model, Model_XYZ* newModel, MasterMesh master);
};

#endif // MESHMAIN_H

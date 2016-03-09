#ifndef TEXTUREMAIN_H
#define TEXTUREMAIN_H

#include "modelPly.h"
#include "modelImg.h"
#include <stdlib.h>
#include <windows.h>
#include "masterSettings.h"
#include "modelXYZ.h"
#include "ofxXmlSettings.h"

class TextureMain
{
    public:

        GLuint *textures;

        MasterSettings* settings;

        /* Texture */

        Model_IMG* textureImage;
        Model_PLY* textureModel;
        MasterTexture* textureMaster;
        bool textureViewMode;
        bool drawFast;
        bool textureWire;

        int textureCount;
        int textureIndex;

        int facesCount;
        int** faces;
        float frustum[6][4];
        float drawXmin;
        float drawXmax;
        float drawYmin;
        float drawYmax;

        /* Camera */

        float cameraFactor;
        int cameraAxis;
        int cameraMove;

        bool cameraLight;

        TextureMain();
        virtual ~TextureMain();

        void display(void);
        void keys(unsigned char key, int x, int y);
        void mouse(int btn, int state, int x, int y);
        void mouseMove(int x, int y);

        void loadLightMapTexture(Model_IMG* model, string file);

    protected:
    private:

        void drawText(const char* text, int length, int x, int y);
        void drawAllText();
        void setNormal(float* points);
        void setFaceVertex(int index, bool isFront);
        void setPointVertex(int index);
        float isFrontFacePoints(float* points);
        bool isFrontFace(int index);

        void draw2DElement(int index);
        void draw3D();
        void draw2DView();

        void ExtractFrustum();
        bool PointInFrustum(float x, float y, float z);

        void draw2DCalibrationFull();
        void draw2DCalibrationFast();
        void draw2DBackground();
        void calcBackground(GLfloat* vert);

        void applyTransformations(vector<MasterTransform*> history, bool flag);
        void UpdateHistory (int id);
        void textureProjection(Matrix4x4f &mv);
        void stepTransformTexture();
        void stepTexture();
        void stepClearTexture();

        void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master);

};

#endif // TEXTUREMAIN_H

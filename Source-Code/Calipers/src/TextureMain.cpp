#include "TextureMain.h"


TextureMain::TextureMain()
{
    textures = new GLuint[3];

    settings = NULL;

    /* Texture */

    textureImage = NULL;
    textureModel = NULL;
    textureMaster = NULL;
    textureViewMode = false;
    drawFast = true;
    textureWire = true;

    textureCount = 1;
    textureIndex = 0;

    facesCount = 200000;
    faces;
    frustum[6][4];
    drawXmin = std::numeric_limits<float>::max();
    drawXmax = std::numeric_limits<float>::min();
    drawYmin = std::numeric_limits<float>::max();
    drawYmax = std::numeric_limits<float>::min();
    /* Camera */

    cameraFactor = 1.0;

    cameraAxis = -1;
    cameraMove = -1;

    cameraLight = true;
}

void TextureMain::drawText(const char* text, int length, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    double* matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0, winW, 0, winH, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

void TextureMain::drawAllText() {

    int positionY = 10;
    for (int i = 0; i <= textureCount; i++) {
        MasterTexture* masterNow = &textureMaster[i];
        std::ostringstream intIndex; intIndex << i;
        std::ostringstream intX; intX << masterNow->viewer[0];
        std::ostringstream intY; intY << masterNow->viewer[1];
        std::ostringstream intZ; intZ << masterNow->viewer[2];
        std::ostringstream intA; intA << masterNow->rotate[0];
        std::ostringstream intB; intB << masterNow->rotate[1];
        std::ostringstream intC; intC << masterNow->rotate[2];

        string textTexture = "Texture ";
        textTexture = textTexture + intIndex.str() + " :: ";
        textTexture = textTexture + " [ Position: " + intX.str() + " | " + intY.str() + " | " + intZ.str() + " ]";
        textTexture = textTexture + " [ Rotation: " + intA.str() + " | " + intB.str() + " | " + intC.str() + " ]";
        drawText(textTexture.data(), textTexture.size(), 10, positionY);
        positionY += 20;
    }

    std::ostringstream intPoints; intPoints << textureModel->TotalPoints;
    std::ostringstream intFaces; intFaces << textureModel->TotalFaces;
    std::ostringstream intCoordMin; intCoordMin << textureModel->MinCoord;
    std::ostringstream intCoordMax; intCoordMax << textureModel->MaxCoord;
    std::ostringstream intCoordAlfa; intCoordAlfa << textureModel->AlfaCoord;
    string textMesh = "Mesh: ";
    textMesh = textMesh + " [ Points: " + intPoints.str() + " ]";
    textMesh = textMesh + " [ Faces: " + intFaces.str() + " ]";
    textMesh = textMesh + " [ Coordinates: " + intCoordMin.str() + " | " + intCoordMax.str() + " | " + intCoordAlfa.str() + " ]";
    drawText(textMesh.data(), textMesh.size(), 10, positionY);
    positionY += 20;

    string textMode = "Mode: ";
    textMode = textMode + (textureIndex == 0 ? "View" : "Calibration");
    drawText(textMode.data(), textMode.size(), 10, positionY);
    positionY += 20;

    string textTitle = "2D CALIBRATION";
    drawText(textTitle.data(), textTitle.size(), 10, positionY);
    positionY += 20;
}


void TextureMain::setNormal(float* points) {
    GLfloat p1[3] = { points[0], points[1], points[2] };
    GLfloat p2[3] = { points[3], points[4], points[5] };
    GLfloat p3[3] = { points[6], points[7], points[8] };

    float va[3], vb[3], vr[3], val;
	va[0] = p2[0] - p1[0];
	va[1] = p2[1] - p1[1];
	va[2] = p2[2] - p1[2];

	vb[0] = p3[0] - p1[0];
	vb[1] = p3[1] - p1[1];
	vb[2] = p3[2] - p1[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];
	float norm = sqrt(vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);

    glNormal3f(vr[0] / norm, vr[1] / norm, vr[2] / norm);
}

void TextureMain::setFaceVertex(int index, bool isFront) {
    GLfloat vert[3] = { textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);

    index = index / 3;
    if (isFront) {
        float points[9] = { textureModel->Faces_Triangles[index * 9], textureModel->Faces_Triangles[index * 9 + 1], textureModel->Faces_Triangles[index * 9 + 2],
                            textureModel->Faces_Triangles[index * 9 + 3], textureModel->Faces_Triangles[index * 9 + 4], textureModel->Faces_Triangles[index * 9 + 5],
                            textureModel->Faces_Triangles[index * 9 + 6], textureModel->Faces_Triangles[index * 9 + 7], textureModel->Faces_Triangles[index * 9 + 8] };
        setNormal(points);
    } else {
        float points[9] = { textureModel->Faces_Triangles[index * 9], textureModel->Faces_Triangles[index * 9 + 1], textureModel->Faces_Triangles[index * 9 + 2],
                            textureModel->Faces_Triangles[index * 9 + 6], textureModel->Faces_Triangles[index * 9 + 7], textureModel->Faces_Triangles[index * 9 + 8],
                            textureModel->Faces_Triangles[index * 9 + 3], textureModel->Faces_Triangles[index * 9 + 4], textureModel->Faces_Triangles[index * 9 + 5] };
        setNormal(points);
    }
}

GLdouble mv[16];
GLdouble mvCamera[10][16];

float TextureMain::isFrontFacePoints(float* points) {
    GLfloat p1[3] = { points[0], points[1], points[2] };
    GLfloat p2[3] = { points[3], points[4], points[5] };
    GLfloat p3[3] = { points[6], points[7], points[8] };

    GLfloat nx[4] = { mv[2] + mv[3], mv[6] + mv[7], mv[10] + mv[11], mv[14] + mv[15] };

    nx[3] = mv[14] < -1 ? nx[3] : -nx[3];

    nx[0] /= nx[3];
    nx[1] /= nx[3];
    nx[2] /= nx[3];

    float va[3], vb[3], vr[3];
	va[0] = p2[0] - p1[0];
	va[1] = p2[1] - p1[1];
	va[2] = p2[2] - p1[2];

	vb[0] = p3[0] - p1[0];
	vb[1] = p3[1] - p1[1];
	vb[2] = p3[2] - p1[2];

	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

    return (nx[0]*vr[0] + nx[1]*vr[1] + nx[2]*vr[2]) / (sqrt(nx[0]*nx[0] + nx[1]*nx[1] + nx[2]*nx[2]) * sqrt(vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2]));
}

bool TextureMain::isFrontFace(int index) {
    float points[9] = { textureModel->Faces_Triangles[index * 9], textureModel->Faces_Triangles[index * 9 + 1], textureModel->Faces_Triangles[index * 9 + 2],
                        textureModel->Faces_Triangles[index * 9 + 3], textureModel->Faces_Triangles[index * 9 + 4], textureModel->Faces_Triangles[index * 9 + 5],
                        textureModel->Faces_Triangles[index * 9 + 6], textureModel->Faces_Triangles[index * 9 + 7], textureModel->Faces_Triangles[index * 9 + 8] };
    float angle = isFrontFacePoints(points);
    return angle > -1 && angle < 0;
}

void TextureMain::draw2DElement(int index) {

    glColor3f(1.0f, 1.0f, 1.0f);
    if (textureViewMode) {
        //glGetDoublev(GL_MODELVIEW_MATRIX, mv);
        for (int i = 0; i < 16; i++) {
            mv[i] = mvCamera[textureIndex][i];
        }
        if (isFrontFace(index)) {
            glBegin(GL_POLYGON);
                glColor3f(1.0f, 0.0f, 0.0f); // ---------------- rojo
                setFaceVertex(index * 3, true);
                setFaceVertex(index * 3 + 1, true);
                setFaceVertex(index * 3 + 2, true);
            glEnd();
        } else {
            glBegin(GL_POLYGON);
                glColor3f(0.0f, 1.0f, 0.0f); // ---------------- verde
                setFaceVertex(index * 3, false);
                setFaceVertex(index * 3 + 2, false);
                setFaceVertex(index * 3 + 1, false);
            glEnd();
        }
    } else {
        glGetDoublev(GL_MODELVIEW_MATRIX, mv);
        for (int i = 0; i < 16; i++) {
            mvCamera[textureIndex][i] = mv[i];
        }
        if (isFrontFace(index)) {
            glBegin(GL_POLYGON);
                glColor3f(1.0f, 0.0f, 0.0f); // ---------------- rojo
                setFaceVertex(index * 3, true);
                setFaceVertex(index * 3 + 1, true);
                setFaceVertex(index * 3 + 2, true);
            glEnd();
        } else {
            glBegin(GL_POLYGON);
                glColor3f(0.0f, 1.0f, 0.0f); // ---------------- verde
                setFaceVertex(index * 3, false);
                setFaceVertex(index * 3 + 2, false);
                setFaceVertex(index * 3 + 1, false);
            glEnd();
        }
    }
    glColor3f(1.0f, 1.0f, 1.0f);
}

void TextureMain::calcBackground(GLfloat* vert) {
    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble pos3D_x, pos3D_y, pos3D_z;
    pos3D_x = vert[0];
    pos3D_y = vert[1];
    pos3D_z = vert[2];
    GLdouble winX, winY, winZ;
    gluProject(pos3D_x, pos3D_y, pos3D_z,
        model_view, projection, viewport,
        &winX, &winY, &winZ);

    if (winX > drawXmax) {
        drawXmax = winX;
    }
    if (winX < drawXmin) {
        drawXmin = winX;
    }
    if (winY > drawYmax) {
        drawYmax = winY;
    }
    if (winY < drawYmin) {
        drawYmin = winY;
    }
}

void TextureMain::draw2DBackground() {

   if (textureIndex > 0) {
       glColor3f(1.0f, 1.0f, 1.0f);
       float wImg = textureImage[textureIndex-1].Width / 58.0;
       float hImg = textureImage[textureIndex-1].Height / 58.0;
       GLfloat vert1[3] = { -wImg, -hImg, -19.0 };
       GLfloat vert2[3] = { -wImg, hImg, -19.0 };
       GLfloat vert3[3] = { wImg, hImg, -19.0 };
       GLfloat vert4[3] = { wImg, -hImg, -19.0 };
       float points[9] = { vert1[0], vert1[1], vert1[2], vert4[0], vert4[1], vert4[2], vert3[0], vert3[1], vert3[2] };
       //float angle = isFrontFacePoints(points);
       glBegin(GL_POLYGON);
           glVertex3fv(vert1);
           setNormal(points);
           glVertex3fv(vert4);
           setNormal(points);
           glVertex3fv(vert3);
           setNormal(points);
           glVertex3fv(vert2);
           setNormal(points);
       glEnd();
       calcBackground(vert1);
       calcBackground(vert2);
       calcBackground(vert3);
       calcBackground(vert4);
   }
}

void TextureMain::draw2DView() {
    cout << "view" << endl;
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int hits = 0;
        for (int k = 1; k <= textureCount; k++) {
            hits = max(hits, faces[k][i]);
        }
        if (hits > 0 && faces[textureIndex][i] == hits) {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);
            draw2DElement(i);
        }
    }
}

void TextureMain::ExtractFrustum() {
    float   proj[16];
    float   modl[16];
    float   clip[16];
    float   t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv( GL_PROJECTION_MATRIX, proj );

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );

    /* Combine the two matrices (multiply projection by modelview) */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    /* Extract the numbers for the RIGHT plane */
    frustum[0][0] = clip[ 3] - clip[ 0];
    frustum[0][1] = clip[ 7] - clip[ 4];
    frustum[0][2] = clip[11] - clip[ 8];
    frustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    frustum[1][0] = clip[ 3] + clip[ 0];
    frustum[1][1] = clip[ 7] + clip[ 4];
    frustum[1][2] = clip[11] + clip[ 8];
    frustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
    frustum[1][0] /= t;
    frustum[1][1] /= t;
    frustum[1][2] /= t;
    frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    frustum[2][0] = clip[ 3] + clip[ 1];
    frustum[2][1] = clip[ 7] + clip[ 5];
    frustum[2][2] = clip[11] + clip[ 9];
    frustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
    frustum[2][0] /= t;
    frustum[2][1] /= t;
    frustum[2][2] /= t;
    frustum[2][3] /= t;

    /* Extract the TOP plane */
    frustum[3][0] = clip[ 3] - clip[ 1];
    frustum[3][1] = clip[ 7] - clip[ 5];
    frustum[3][2] = clip[11] - clip[ 9];
    frustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
    frustum[3][0] /= t;
    frustum[3][1] /= t;
    frustum[3][2] /= t;
    frustum[3][3] /= t;

    /* Extract the FAR plane */
    frustum[4][0] = clip[ 3] - clip[ 2];
    frustum[4][1] = clip[ 7] - clip[ 6];
    frustum[4][2] = clip[11] - clip[10];
    frustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
    frustum[4][0] /= t;
    frustum[4][1] /= t;
    frustum[4][2] /= t;
    frustum[4][3] /= t;

    /* Extract the NEAR plane */
    frustum[5][0] = clip[ 3] + clip[ 2];
    frustum[5][1] = clip[ 7] + clip[ 6];
    frustum[5][2] = clip[11] + clip[10];
    frustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
    frustum[5][0] /= t;
    frustum[5][1] /= t;
    frustum[5][2] /= t;
    frustum[5][3] /= t;
}

bool TextureMain::PointInFrustum(float x, float y, float z) {
    int p;
    for (p = 0; p < 6; p++) {
        if (frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0) {
            return false;
        }
    }
    return true;
}

void TextureMain::draw2DCalibrationFull() {
    cout << "full" << endl;
    ExtractFrustum();
    GLuint queries[textureModel->TotalFaces];
    GLuint sampleCount;
    glGenQueriesARB(textureModel->TotalFaces, queries);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int index = i * 3;
        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);
            draw2DElement(i);
            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
    }
    glEnable(GL_BLEND);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);

    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glGetDoublev(GL_MODELVIEW_MATRIX, mv);

    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int index = i * 3;
        if (textureIndex > 0) {
            faces[textureIndex][i] = 0;
        }

        GLdouble pos3D_x, pos3D_y, pos3D_z;
        pos3D_x = textureModel->Faces_Triangles[index * 3];
        pos3D_y = textureModel->Faces_Triangles[index * 3 + 1];
        pos3D_z = textureModel->Faces_Triangles[index * 3 + 2];
        GLdouble winX, winY, winZ;
        gluProject(pos3D_x, pos3D_y, pos3D_z,
            model_view, projection, viewport,
            &winX, &winY, &winZ);

        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
            glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
            if (sampleCount > 0) {
                if (winX > drawXmin && winX < drawXmax && winY > drawYmin && winY < drawYmax && textureIndex > 0) {
                    faces[textureIndex][i] = sampleCount;
                }
                glEnable(GL_CULL_FACE);
                glFrontFace(GL_CW);
                glCullFace(GL_FRONT);
                draw2DElement(i);
            }
        }
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void TextureMain::draw2DCalibrationFast() {

    for (int i = 0; i < textureModel->TotalFaces; i++) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        draw2DElement(i);
    }
}

void TextureMain::applyTransformations(vector<MasterTransform*> history, bool flag) {
    if (flag) {
        for (int i = 0; i < history.size(); i++) {
            MasterTransform* trans = history[i];
            if (trans->type == 0) { glTranslatef(trans->value, 0, 0); }
            if (trans->type == 1) { glTranslatef(0, trans->value, 0); }
            if (trans->type == 2) { glTranslatef(0, 0, trans->value); }
            if (trans->type == 3) { glRotatef(trans->value, 1.0f,0.0f,0.0f); }
            if (trans->type == 4) { glRotatef(trans->value, 0.0f,1.0f,0.0f); }
            if (trans->type == 5) { glRotatef(trans->value, 0.0f,0.0f,1.0f); }
        }
    } else {
        for (int i = 0; i < history.size(); i++) {
            MasterTransform* trans = history[history.size()-i-1];
            if (trans->type == 0) { glTranslatef(-trans->value, 0, 0); }
            if (trans->type == 1) { glTranslatef(0, -trans->value, 0); }
            if (trans->type == 2) { glTranslatef(0, 0, -trans->value); }
            if (trans->type == 3) { glRotatef(-trans->value, 1.0f,0.0f,0.0f); }
            if (trans->type == 4) { glRotatef(-trans->value, 0.0f,1.0f,0.0f); }
            if (trans->type == 5) { glRotatef(-trans->value, 0.0f,0.0f,1.0f); }
        }
    }
}

void TextureMain::textureProjection(Matrix4x4f &mv) {

    Matrix4x4f inverseMV = Matrix4x4f::invertMatrix(mv);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f,0.5f,0.0f);
    float wImg = textureImage[textureIndex-1].Width;
    float hImg = textureImage[textureIndex-1].Height;
    if (wImg < hImg) {
        glScalef(1.0f,(1.f*wImg)/hImg,3.0f);
    } else {
        glScalef((1.f*hImg)/wImg,1.0f,3.0f);
    }
    glFrustum(-0.035,0.035,-0.035,0.035,0.04,2.0);
    glMultMatrixf(inverseMV.getMatrix());
    glMatrixMode(GL_MODELVIEW);
}


void TextureMain::stepTransformTexture() {
    if (textureViewMode) {
        glTranslatef(0, 0, -20);
        applyTransformations(textureMaster[0].history, true);
        glTranslatef(0, 0, 20);

        glTranslatef(0, 0, -20);

//        GLdouble m[16];
//        MasterSettings::CalculateMatrix(textureMaster[textureIndex].history, m, false);
//        glMultMatrixd(m);
        applyTransformations(textureMaster[textureIndex].history, false);
        glTranslatef(0, 0, 20);
    } else {
        glRotatef(0, 1.0f,0.0f,0.0f);
        glRotatef(0, 0.0f,1.0f,0.0f);
        glRotatef(0, 0.0f,0.0f,1.0f);
    }
}

void TextureMain::stepTexture() {
    glPushMatrix();
    glLoadIdentity();

    if (textureIndex > 0) {
        glActiveTextureARB(GL_TEXTURE0 + textureIndex - 1);
        glEnable(GL_TEXTURE_2D);
        stepTransformTexture();
    }

    glGetFloatv(GL_MODELVIEW_MATRIX, textureMaster[textureIndex].MVmatrix);
    textureMaster[textureIndex].TextureTransform.setMatrix(textureMaster[textureIndex].MVmatrix);
    glPopMatrix();
    textureProjection(textureMaster[textureIndex].TextureTransform);
}

void TextureMain::stepClearTexture() {
    for (int i = 0; i < textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glDisable(GL_TEXTURE_2D);
    }
}

void TextureMain::display(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if (cameraLight) {
       glEnable(GL_LIGHTING);
       glEnable(GL_LIGHT0);

       glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

       glPushMatrix();
       glLoadIdentity();
       GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
       GLfloat mat_shininess[] = { 50.0 };
       GLfloat light_color[] = { 1., 1., 1., 0.5 };
       GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

       glShadeModel (GL_SMOOTH);
       glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
       glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
       glLightfv(GL_LIGHT0, GL_POSITION, light_position);
       glPopMatrix();
   }

    if (textureViewMode) {
        for (int i = 1; i <= textureCount; i++) {
            textureIndex = i;
            stepTexture();
            glLoadIdentity();
            glTranslatef(0, 0, -20);
            applyTransformations(textureMaster[0].history, true);

            draw2DView();
            stepClearTexture();
        }
        textureIndex = 0;

    } else {
        stepTexture();
        glLoadIdentity();
        glTranslatef(0, 0, -20);
//        GLdouble m[16];
//        MasterSettings::CalculateMatrix(textureMaster[textureIndex].history, m, true);
//        glMultMatrixd(m);
        applyTransformations(textureMaster[textureIndex].history, true);
        if (drawFast) {
            draw2DCalibrationFast();
        } else {
            draw2DCalibrationFull();
        }

        glPushMatrix();
        glLoadIdentity();
        draw2DBackground();
        glPopMatrix();

        stepClearTexture();
    }

    if (cameraLight) {

       //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
       glDisable(GL_LIGHT0);
       glDisable(GL_LIGHTING);
    }

    drawAllText();

    glFlush();
    glutSwapBuffers();
}


void CalculateRotateMatrix(vector<MasterTransform*> history, GLdouble* m) {
    glPushMatrix();
    glLoadIdentity();
    for (int i = 0; i < history.size(); i++) {
        MasterTransform* trans = history[i];
        if (trans->type == 3) { glRotatef(trans->value, 1.0f,0.0f,0.0f); }
        if (trans->type == 4) { glRotatef(trans->value, 0.0f,1.0f,0.0f); }
        if (trans->type == 5) { glRotatef(trans->value, 0.0f,0.0f,1.0f); }
    }
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
}

void CalculateTranslationX(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[12] * v + m[15]
    rx = (m[0] * v + m[3]) / rw;
    ry = (m[4] * v + m[7]) / rw;
    rz = (m[8] * v + m[11]) /rw;
}

void CalculateTranslationY(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[13] * v + m[15]
    rx = (m[1] * v + m[3]) / rw;
    ry = (m[5] * v + m[7]) / rw;
    rz = (m[9] * v + m[11]) /rw;
}

void CalculateTranslationZ(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[114] * v + m[15]
    rx = (m[2] * v + m[3]) / rw;
    ry = (m[6] * v + m[7]) / rw;
    rz = (m[10] * v + m[11]) / rw;
}


void TextureMain::UpdateHistory (int id) {
    float valueX = 0;
    float valueY = 0;
    float valueZ = 0;
    float valueA = 0;
    float valueB = 0;
    float valueC = 0;

    if (textureMaster[id].viewer[0] != 0) {
        CalculateTranslationX(textureMaster[id].history, textureMaster[id].viewer[0], valueX, valueY, valueZ);

    } else if (textureMaster[id].viewer[1] != 0) {
        CalculateTranslationY(textureMaster[id].history, textureMaster[id].viewer[1], valueX, valueY, valueZ);

    } else if (textureMaster[id].viewer[2] != 0) {
        CalculateTranslationZ(textureMaster[id].history, textureMaster[id].viewer[2], valueX, valueY, valueZ);

    }
    if (textureMaster[id].rotate[0] != 0) {
        CalculateTranslationX(textureMaster[id].history, textureMaster[id].rotate[0], valueA, valueB, valueC);

    } else if (textureMaster[id].rotate[1] != 0) {
        CalculateTranslationY(textureMaster[id].history, textureMaster[id].rotate[1], valueA, valueB, valueC);

    } else if (textureMaster[id].rotate[2] != 0) {
        CalculateTranslationZ(textureMaster[id].history, textureMaster[id].rotate[2], valueA, valueB, valueC);

    }

    textureMaster[id].viewer[0] = 0;
    textureMaster[id].viewer[1] = 0;
    textureMaster[id].viewer[2] = 0;
    textureMaster[id].rotate[0] = 0;
    textureMaster[id].rotate[1] = 0;
    textureMaster[id].rotate[2] = 0;

    float values[6] = { valueX, valueY, valueZ, valueA, valueB, valueC };

    for (int i = 0; i < 6; i++) {
        if (values[i] != 0) {
            MasterTransform* trans = NULL;
            if (textureMaster[id].history.size() == 0 || textureMaster[id].history.back()->type != i) {
                trans = new MasterTransform();
                trans->value = values[i];
                trans->type = i;
                textureMaster[id].history.push_back(trans);
            } else {
                trans = textureMaster[id].history.back();
                trans->value += values[i];
            }
        }
    }
}


void TextureMain::keys(unsigned char key, int x, int y) {

    if (key == 'l') {
        settings->loadTextureCalibration();
        for (int i = 1; i <= textureCount; i++) {
            textureIndex = i;
            display();
        }
        textureViewMode = true;
        textureIndex = 0;
    }
    if (key == 'p') {
        cameraLight = !cameraLight;
    }
    if(key == 'v') {
        drawFast = false;
        display();
        drawFast = true;
        textureViewMode = true;
        textureIndex = 0;
    }
    if(key >= '1' && key <= '9' && (key - 48 <= textureCount)) {
        drawFast = false;
        display();
        drawFast = true;
        textureViewMode = false;
        textureIndex = key - 48;
        display();
    }

    if(key == '+') cameraFactor *= 1.25;
    if(key == '-') cameraFactor *= 0.8;

    if(key == 'w') textureMaster[textureIndex].rotate[0] += 2.0 * cameraFactor;
    if(key == 's') textureMaster[textureIndex].rotate[0] -= 2.0 * cameraFactor;
    if(key == 'a') textureMaster[textureIndex].rotate[1] += 2.0 * cameraFactor;
    if(key == 'd') textureMaster[textureIndex].rotate[1] -= 2.0 * cameraFactor;
    if(key == 'e') textureMaster[textureIndex].rotate[2] += 2.0 * cameraFactor;
    if(key == 'q') textureMaster[textureIndex].rotate[2] -= 2.0 * cameraFactor;

    if(key == 'm') textureMaster[textureIndex].viewer[0] += 0.2 * cameraFactor;
    if(key == 'b') textureMaster[textureIndex].viewer[0] -= 0.2 * cameraFactor;
    if(key == 'h') textureMaster[textureIndex].viewer[1] += 0.2 * cameraFactor;
    if(key == 'n') textureMaster[textureIndex].viewer[1] -= 0.2 * cameraFactor;
    if(key == 'j') textureMaster[textureIndex].viewer[2] += 0.2 * cameraFactor;
    if(key == 'g') textureMaster[textureIndex].viewer[2] -= 0.2 * cameraFactor;

    if (key == 'w' || key == 's' || key == 'a' || key == 'd' || key == 'e' || key == 'q' ||
        key == 'm' || key == 'b' || key == 'h' || key == 'n' || key == 'j' || key == 'g') {
        UpdateHistory(textureIndex);
    }

    display();
}

void TextureMain::mouse(int btn, int state, int x, int y) {

    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }
    display();
}

void TextureMain::mouseMove(int x, int y) {

    if (cameraAxis != -1) {
        float deltaMove = (y - cameraMove) * 0.1f * cameraFactor;
        cameraMove = y;
        if (cameraAxis == GLUT_LEFT_BUTTON) {
            textureMaster[textureIndex].viewer[0] += deltaMove;
        } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
            textureMaster[textureIndex].viewer[1] += deltaMove;
        } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
            textureMaster[textureIndex].viewer[2] += deltaMove;
        }
        if (cameraAxis == GLUT_LEFT_BUTTON || cameraAxis == GLUT_RIGHT_BUTTON || cameraAxis == GLUT_MIDDLE_BUTTON) {
            UpdateHistory(textureIndex);
        }
        display();
    }
}

void TextureMain::loadLightMapTexture(Model_IMG* model, string file) {

    GLfloat eyePlaneS[] =  {1.0f, 0.0f, 0.0f, 0.0f};
    GLfloat eyePlaneT[] =  {0.0f, 1.0f, 0.0f, 0.0f};
    GLfloat eyePlaneR[] =  {0.0f, 0.0f, 1.0f, 0.0f};
    GLfloat eyePlaneQ[] =  {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat borderColor[] = {1.f, 1.f, 1.f, 1.0f};

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    glTexGenfv(GL_S,GL_EYE_PLANE,eyePlaneS);
    glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    glTexGenfv(GL_T,GL_EYE_PLANE,eyePlaneT);
    glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    glTexGenfv(GL_R,GL_EYE_PLANE,eyePlaneR);
    glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    glTexGenfv(GL_Q,GL_EYE_PLANE,eyePlaneQ);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    model->Load(file);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,model->Width,model->Height,GL_BGR,GL_UNSIGNED_BYTE,model->Pixels);
}



TextureMain::~TextureMain()
{
    //dtor
}

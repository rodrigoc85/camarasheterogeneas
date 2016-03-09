#include "main.h"

float* Faces_Triangles;
float frustum[6][4];


void ExtractFrustum() {
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

bool PointInFrustum(float x, float y, float z) {
    int p;
    for (p = 0; p < 6; p++) {
        if (frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0) {
            return false;
        }
    }
    return true;
}

void setFaceVertex(int index)
{
    GLfloat vert[3] = { Faces_Triangles[index * 3], Faces_Triangles[index * 3 + 1], Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
    ///
//    glNormal3f(textureModel->Normals[index * 3], textureModel->Normals[index * 3 + 1], textureModel->Normals[index * 3 + 2]);
}

bool isFrontFace(int index) {
    GLfloat p1[3] = { Faces_Triangles[index * 9], Faces_Triangles[index * 9 + 1], Faces_Triangles[index * 9 + 2] };
    GLfloat p2[3] = { Faces_Triangles[index * 9 + 3], Faces_Triangles[index * 9 + 4], Faces_Triangles[index * 9 + 5] };
    GLfloat p3[3] = { Faces_Triangles[index * 9 + 6], Faces_Triangles[index * 9 + 7], Faces_Triangles[index * 9 + 8] };

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

	return vr[2] <= 0;
}

void draw2DElement(int index)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        if (isFrontFace(index)) {
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 1);
            setFaceVertex(index * 3 + 2);
        } else {
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 2);
            setFaceVertex(index * 3 + 1);
        }
    glEnd();


    /*glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        setFaceVertex(index * 3);
        setFaceVertex(index * 3 + 1);
        setFaceVertex(index * 3 + 2);
    glEnd();*/
}

void DLL_EXPORT OcclusionCulling(int textureIndex,int TotalFaces, float* Faces_TrianglesParm, int*** faces)
{
    ExtractFrustum();

    Faces_Triangles = Faces_TrianglesParm;
    glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
    glGenQueriesARB = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
    glEndQueryARB = (PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
    glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuivARB");
    glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)wglGetProcAddress("glDeleteQueriesARB");
    GLuint* queries = new GLuint[TotalFaces];
    GLuint sampleCount;
    glGenQueriesARB(TotalFaces, queries);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < TotalFaces; i++)
    {
        int index = i * 3;
        if (PointInFrustum(Faces_Triangles[index * 3], Faces_Triangles[index * 3 + 1], Faces_Triangles[index * 3 + 2])) {
            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
            draw2DElement(i);
            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
    }
    glEnable(GL_BLEND);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < TotalFaces; i++)
    {
        int index = i * 3;
        if (PointInFrustum(Faces_Triangles[index * 3], Faces_Triangles[index * 3 + 1], Faces_Triangles[index * 3 + 2])) {
        glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
            if (sampleCount > 0)
            {
                if (textureIndex > 0)
                {
                    (*faces)[textureIndex][i] = sampleCount;
                }
                /*glEnable(GL_CULL_FACE);
                glFrontFace(GL_CW);
                glCullFace(GL_FRONT);*/
                draw2DElement(i);
            }
        }
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDeleteQueriesARB(TotalFaces, queries);
    delete [] queries;
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        break;

    case DLL_THREAD_ATTACH:
        // attach to thread
        break;

    case DLL_THREAD_DETACH:
        // detach from thread
        break;
    }
    return TRUE; // succesful
}

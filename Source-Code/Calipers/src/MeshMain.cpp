#include "MeshMain.h"


typedef void (*f_funci)(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);

MeshMain::MeshMain()
{
    REAL_TIME = true;
    REAL_TIME_FPS = 10;
    REAL_TIME_PORT = 3232;

    meshCount = 3;
    meshIndex = 0;
    generatingMesh = false;

    cameraAxis = -1;
    cameraMove = -1;
    cameraAll = false;

    cameraLight = true;

    settings = NULL;

    cloudModel = NULL;
    cloudMaster = NULL;

    faces = NULL;
    numberFaces = 0;

    cameraFactor = 1.0;


    doubleClickTime = 500;
    clickCount = 0;

    colors[0][0] = 1.0; colors[0][1] = 1.0; colors[0][2] = 1.0;
    colors[1][0] = 1.0; colors[1][1] = 0.0; colors[1][2] = 0.0;
    colors[2][0] = 0.0; colors[2][1] = 1.0; colors[2][2] = 0.0;
    colors[3][0] = 0.0; colors[3][1] = 0.0; colors[3][2] = 1.0;
    colors[4][0] = 1.0; colors[4][1] = 1.0; colors[4][2] = 0.0;
    colors[5][0] = 1.0; colors[5][1] = 0.0; colors[5][2] = 1.0;
    colors[6][0] = 0.0; colors[5][1] = 1.0; colors[5][2] = 1.0;

}

void MeshMain::drawText(const char* text, int length, int x, int y) {
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

void MeshMain::drawAllText() {

    int positionY = 10;

    if (generatingMesh) {
        string textMode = "Generating the mesh ...";
        drawText(textMode.data(), textMode.size(), 10, positionY);
        positionY += 20;

    } else {
        for (int i = 0; i <= meshCount; i++) {
            MasterMesh* masterNow = &cloudMaster[i];
            std::ostringstream intIndex; intIndex << i;
            std::ostringstream intPoints; intPoints << cloudModel[i]->TotalPoints;
            std::ostringstream intX; intX << masterNow->viewer[0];
            std::ostringstream intY; intY << masterNow->viewer[1];
            std::ostringstream intZ; intZ << masterNow->viewer[2];
            std::ostringstream intA; intA << masterNow->rotate[0];
            std::ostringstream intB; intB << masterNow->rotate[1];
            std::ostringstream intC; intC << masterNow->rotate[2];

            string textMesh = "Mesh ";
            textMesh = textMesh + intIndex.str() + " :: ";
            textMesh = textMesh + " [ Points: " + intPoints.str() + " ]";
            textMesh = textMesh + " [ Position: " + intX.str() + " | " + intY.str() + " | " + intZ.str() + " ]";
            textMesh = textMesh + " [ Rotation: " + intA.str() + " | " + intB.str() + " | " + intC.str() + " ]";
            drawText(textMesh.data(), textMesh.size(), 10, positionY);
            positionY += 20;
        }

        string textMode = "Mode: ";
        textMode = textMode + (meshIndex == 0 ? "View" : "Calibration");
        drawText(textMode.data(), textMode.size(), 10, positionY);
        positionY += 20;
    }

    string textTitle = "3D CALIBRATION";
    drawText(textTitle.data(), textTitle.size(), 10, positionY);
    positionY += 20;
}

void MeshMain::generarMalla(NubePuntos* nube){
    char* dllName = "C:\\Users\\Rodrigo\\Documents\\GitHub\\camarasheterogeneas\\Proyectos\\GenerarMallas\\bin\\Release\\GenerarMallas.dll";
    HINSTANCE hGetProcIDDLL =  LoadLibraryA(dllName);
    if (!hGetProcIDDLL) {
        std::cout << "Failed to load the library: " << dllName << std::endl;
    }
    f_funci funci = (f_funci)GetProcAddress(hGetProcIDDLL, "generarMallaCalibrador");

    faces = new FaceStruct;
    numberFaces = new int;

    cout << "Generando la nube..." << endl;
    (funci) (nube, &faces, numberFaces, 0);
    cout << "Fin de a generacion..." << endl;
}

void MeshMain::generarNubeUnida(){
    NubePuntos* nube = new NubePuntos;
    nube->largo = cloudModel[0]->TotalPoints;
    nube->x = new float[cloudModel[0]->TotalPoints];
    nube->y = new float[cloudModel[0]->TotalPoints];
    nube->z = new float[cloudModel[0]->TotalPoints];
    for (int i = 0; i < cloudModel[0]->TotalPoints; i++){

        nube->x[i] = cloudModel[0]->Points[i*3];
        nube->y[i] = cloudModel[0]->Points[i*3+1];
        nube->z[i] = cloudModel[0]->Points[i*3+2];
    }
    generarMalla(nube);
}

void MeshMain::saveXmlFile() {
	ofxXmlSettings settings;

	settings.addTag("settings");
	settings.pushTag("settings");

	settings.setValue("realTime", REAL_TIME);
	settings.setValue("realTimeFPS", REAL_TIME_FPS);
	settings.setValue("realTimePort", REAL_TIME_PORT);

	settings.addTag("cameras");
	settings.pushTag("cameras");

	for(int i = 0; i < meshCount; i++) {
		settings.addTag("camera");
		settings.pushTag("camera", i);

		settings.addValue("id", i);
		settings.addValue("resolutionX", 800);//800
		settings.addValue("resolutionY", 600);//600
		settings.addValue("resolutionDownSample", 1);
		settings.addValue("FPS", 30);// camera i fps
		settings.addValue("colorRGB", true);// camera i color rgb
		settings.addValue("use2D", true);// camera i use2d
		settings.addValue("use3D", true);// camera i use 3d
		settings.addValue("dataContext", "");// data context

		settings.addTag("depthSettings");
		settings.pushTag("depthSettings");
		settings.addValue("near", 10);
		settings.addValue("far", 100);
		settings.addValue("pointsDownSample", 1);

		settings.popTag();

		settings.addTag("matrix");
		settings.pushTag("matrix");

        GLdouble m[16];
        MasterSettings::CalculateMatrix(cloudMaster[i], m);

		for(int j = 0; j < 16; j++) {
            std::stringstream cellM;
            cellM << "r" << j / 4 << j % 4;
            settings.addValue(cellM.str(), m[j]);
		}

		settings.popTag();
		settings.popTag();
	}

	settings.popTag();
	settings.popTag();

	settings.saveFile("settings.xml");
}

void MeshMain::setPointVertex(int index, double* m) {
    GLfloat vert[3] = { cloudModel[meshIndex]->Points[index * 3], cloudModel[meshIndex]->Points[index * 3 + 1], cloudModel[meshIndex]->Points[index * 3 + 2] };
    glVertex3fv(vert);

    float newv[3] = { 0, 0, -1 };
    /*newv[0] = vert[0] * m[0] + vert[1] * m[1] + vert[2] * m[2] + m[3];
    newv[1] = vert[0] * m[4] + vert[1] * m[5] + vert[2] * m[6] + m[7];
    newv[2] = vert[0] * m[8] + vert[1] * m[9] + vert[2] * m[10] + m[11];
    newv[3] = vert[0] * m[12] + vert[1] * m[13] + vert[2] * m[14] + m[15];

    newv[0] = newv[0] / newv[3];
    newv[1] = newv[1] / newv[3];
    newv[2] = newv[2] / newv[3];*/

    glNormal3fv(newv);
}

void MeshMain::draw3D() {
    glPointSize(0.2);
    glColor3fv(colors[meshIndex]);

    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);

    for (int i = 0; i < cloudModel[meshIndex]->TotalPoints; i += 4) {
        glBegin(GL_POINTS);
            if (cloudModel[meshIndex]->hasColor){
                glColor3f(cloudModel[meshIndex]->ColorPoints[i * 3], cloudModel[meshIndex]->ColorPoints[i * 3 + 1], cloudModel[meshIndex]->ColorPoints[i * 3 + 2] );
            }
            setPointVertex(i, m);
        glEnd();
    }
}

void MeshMain::IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master) {
    GLdouble m[16];
    MasterSettings::CalculateMatrix(master, m);
    model->Include(newModel, m);
    /*GLdouble mm[16];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mm[i*4+j] = master.matrix[i*4+0] * m[0*4+j] + master.matrix[i*4+1] * m[1*4+j] + master.matrix[i*4+2] * m[2*4+j] + master.matrix[i*4+3] * m[3*4+j];
        }
    }
    model->Include(newModel, mm);*/
}

void MeshMain::display(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(cloudMaster[0].viewer[0], cloudMaster[0].viewer[1], cloudMaster[0].viewer[2] - 10);
    glRotatef(cloudMaster[0].rotate[0], -1.0f,0.0f,0.0f);
    glRotatef(cloudMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
    glRotatef(cloudMaster[0].rotate[2], 0.0f,0.0f,-1.0f);

    if (meshIndex != 0) {
        glTranslatef(cloudMaster[meshIndex].viewer[0], cloudMaster[meshIndex].viewer[1], cloudMaster[meshIndex].viewer[2]);
        glRotatef(cloudMaster[meshIndex].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(cloudMaster[meshIndex].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(cloudMaster[meshIndex].rotate[2], 0.0f,0.0f,-1.0f);
    }
    if (cameraLight){
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glPushMatrix();
        glLoadIdentity();
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
        GLfloat mat_shininess[] = { 50.0 };
        GLfloat light_color[] = { 1., 1., 1., 0.5 };
        GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

        glShadeModel (GL_SMOOTH);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colors[meshIndex]);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, colors[meshIndex]);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glPopMatrix();
    }

    draw3D();

    if (meshIndex != 0) {
        int meshIndexOld = meshIndex;
        meshIndex = 0;

        glLoadIdentity();
        glTranslatef(cloudMaster[0].viewer[0], cloudMaster[0].viewer[1], cloudMaster[0].viewer[2] - 10);
        glRotatef(cloudMaster[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(cloudMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(cloudMaster[0].rotate[2], 0.0f,0.0f,-1.0f);

        if (cameraLight) {
            glPushMatrix();
            glLoadIdentity();
            GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
            GLfloat mat_shininess[] = { 50.0 };
            GLfloat light_color[] = { 1., 1., 1., 0.5 };
            GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

            glShadeModel (GL_SMOOTH);
            glMaterialfv(GL_FRONT, GL_SPECULAR, colors[meshIndex]);
            glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, colors[meshIndex]);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            glPopMatrix();/**/
        }
        draw3D();

        meshIndex = meshIndexOld;
    }

    if (cameraLight) {
       glDisable(GL_LIGHT0);
       glDisable(GL_LIGHTING);
    }

    drawAllText();

	glFlush();
	glutSwapBuffers();
}


void CalculateTranslationX(float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    float rw = 1;//m[12] * v + m[15]
    rx = (m[0] * v + m[3]) / rw;
    ry = (m[4] * v + m[7]) / rw;
    rz = (m[8] * v + m[11]) /rw;
}

void CalculateTranslationY(float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    float rw = 1;//m[13] * v + m[15]
    rx = (m[1] * v + m[3]) / rw;
    ry = (m[5] * v + m[7]) / rw;
    rz = (m[9] * v + m[11]) /rw;
}

void CalculateTranslationZ(float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    float rw = 1;//m[114] * v + m[15]
    rx = (m[2] * v + m[3]) / rw;
    ry = (m[6] * v + m[7]) / rw;
    rz = (m[10] * v + m[11]) / rw;
}

void MeshMain::mouseMove(int x, int y) {
	if (cameraAxis != -1) {
		float deltaMove = (y - cameraMove) * 0.1f * cameraFactor;
		cameraMove = y;
        if (cameraAll) {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                cloudMaster[0].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                cloudMaster[0].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                cloudMaster[0].viewer[2] += deltaMove;
            }
        } else {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                cloudMaster[meshIndex].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                cloudMaster[meshIndex].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                cloudMaster[meshIndex].viewer[2] += deltaMove;
            }
        }
		display();
	}
}

void MeshMain::mouse(int btn, int state, int x, int y) {
    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
        clickCount++;
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }
	display();
}

void MeshMain::keys(unsigned char key, int x, int y) {

    if (key == 'l') {
        settings->loadMeshCalibration();
    }
    if (key == 'k') {
        settings->saveMeshCalibration();
    }
    if (key == 'p') {
        cameraLight = !cameraLight;
    }
    if(key == 'v') {
        meshIndex = 0;
        cloudModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            IncludeMesh(cloudModel[0], cloudModel[i], cloudMaster[i]);
        }
        generatingMesh = true;
        display();
        generarNubeUnida();
        generatingMesh = false;
    }
    if(key >= '1' && key <= '9' && (key - 48 <= meshCount)) {
        meshIndex = key - 48;
        cloudModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            if (i != meshIndex) {
                IncludeMesh(cloudModel[0], cloudModel[i], cloudMaster[i]);
            }
        }
    }
    if(key == '+') cameraFactor *= 1.25;
    if(key == '-') cameraFactor *= 0.8;

    if (cameraAll) {
        if(key == 'W' || key == 'w') cloudMaster[0].rotate[0] += 2.0 * cameraFactor;
        if(key == 'S' || key == 's') cloudMaster[0].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'A' || key == 'a') cloudMaster[0].rotate[1] += 2.0 * cameraFactor;
        if(key == 'D' || key == 'd') cloudMaster[0].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'E' || key == 'e') cloudMaster[0].rotate[2] += 2.0 * cameraFactor;
        if(key == 'Q' || key == 'q') cloudMaster[0].rotate[2] -= 2.0 * cameraFactor;

        if(key == 'M' || key == 'm') cloudMaster[0].viewer[0] += 0.2 * cameraFactor;
        if(key == 'B' || key == 'b') cloudMaster[0].viewer[0] -= 0.2 * cameraFactor;
        if(key == 'H' || key == 'h') cloudMaster[0].viewer[1] += 0.2 * cameraFactor;
        if(key == 'N' || key == 'n') cloudMaster[0].viewer[1] -= 0.2 * cameraFactor;
        if(key == 'J' || key == 'j') cloudMaster[0].viewer[2] += 0.2 * cameraFactor;
        if(key == 'G' || key == 'g') cloudMaster[0].viewer[2] -= 0.2 * cameraFactor;

    } else {
        if(key == 'w') cloudMaster[meshIndex].rotate[0] += 2.0 * cameraFactor;
        if(key == 's') cloudMaster[meshIndex].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'a') cloudMaster[meshIndex].rotate[1] += 2.0 * cameraFactor;
        if(key == 'd') cloudMaster[meshIndex].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'e') cloudMaster[meshIndex].rotate[2] += 2.0 * cameraFactor;
        if(key == 'q') cloudMaster[meshIndex].rotate[2] -= 2.0 * cameraFactor;

        if(key == 'W') cloudMaster[0].rotate[0] += 2.0 * cameraFactor;
        if(key == 'S') cloudMaster[0].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'A') cloudMaster[0].rotate[1] += 2.0 * cameraFactor;
        if(key == 'D') cloudMaster[0].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'E') cloudMaster[0].rotate[2] += 2.0 * cameraFactor;
        if(key == 'Q') cloudMaster[0].rotate[2] -= 2.0 * cameraFactor;

        if(key == 'm') cloudMaster[meshIndex].viewer[0] += 0.2 * cameraFactor;
        if(key == 'b') cloudMaster[meshIndex].viewer[0] -= 0.2 * cameraFactor;
        if(key == 'h') cloudMaster[meshIndex].viewer[1] += 0.2 * cameraFactor;
        if(key == 'n') cloudMaster[meshIndex].viewer[1] -= 0.2 * cameraFactor;
        if(key == 'j') cloudMaster[meshIndex].viewer[2] += 0.2 * cameraFactor;
        if(key == 'g') cloudMaster[meshIndex].viewer[2] -= 0.2 * cameraFactor;

        if(key == 'M') cloudMaster[0].viewer[0] += 0.2 * cameraFactor;
        if(key == 'B') cloudMaster[0].viewer[0] -= 0.2 * cameraFactor;
        if(key == 'H') cloudMaster[0].viewer[1] += 0.2 * cameraFactor;
        if(key == 'N') cloudMaster[0].viewer[1] -= 0.2 * cameraFactor;
        if(key == 'J') cloudMaster[0].viewer[2] += 0.2 * cameraFactor;
        if(key == 'G') cloudMaster[0].viewer[2] -= 0.2 * cameraFactor;
    }


    /*float valueX = 0;
    float valueY = 0;
    float valueZ = 0;
    float valueA = 0;
    float valueB = 0;
    float valueC = 0;

    if(key == 'W' || key == 'w') CalculateTranslationX(2.0 * cameraFactor, valueA, valueB, valueC);
    if(key == 'S' || key == 's') CalculateTranslationX(-2.0 * cameraFactor, valueA, valueB, valueC);
    if(key == 'A' || key == 'a') CalculateTranslationY(2.0 * cameraFactor, valueA, valueB, valueC);
    if(key == 'D' || key == 'd') CalculateTranslationY(-2.0 * cameraFactor, valueA, valueB, valueC);
    if(key == 'E' || key == 'e') CalculateTranslationZ(2.0 * cameraFactor, valueA, valueB, valueC);
    if(key == 'Q' || key == 'q') CalculateTranslationZ(-2.0 * cameraFactor, valueA, valueB, valueC);

    if(key == 'M' || key == 'm') CalculateTranslationX(0.2 * cameraFactor, valueX, valueY, valueZ);
    if(key == 'B' || key == 'b') CalculateTranslationX(-0.2 * cameraFactor, valueX, valueY, valueZ);
    if(key == 'H' || key == 'h') CalculateTranslationY(0.2 * cameraFactor, valueX, valueY, valueZ);
    if(key == 'N' || key == 'n') CalculateTranslationY(-0.2 * cameraFactor, valueX, valueY, valueZ);
    if(key == 'J' || key == 'j') CalculateTranslationZ(0.2 * cameraFactor, valueX, valueY, valueZ);
    if(key == 'G' || key == 'g') CalculateTranslationZ(-0.2 * cameraFactor, valueX, valueY, valueZ);

    bool cameraAllChange = cameraAll || (key >= 'A' && key <= 'Z');

    if (cameraAllChange) {
        cloudMaster[0].viewer[0] += valueX;
        cloudMaster[0].viewer[1] += valueY;
        cloudMaster[0].viewer[2] += valueZ;
        cloudMaster[0].rotate[0] += valueA;
        cloudMaster[0].rotate[1] += valueB;
        cloudMaster[0].rotate[2] += valueC;
    } else {
        cloudMaster[meshIndex].viewer[0] += valueX;
        cloudMaster[meshIndex].viewer[1] += valueY;
        cloudMaster[meshIndex].viewer[2] += valueZ;
        cloudMaster[meshIndex].rotate[0] += valueA;
        cloudMaster[meshIndex].rotate[1] += valueB;
        cloudMaster[meshIndex].rotate[2] += valueC;
    }*/

	display();
}

MeshMain::~MeshMain()
{
    //dtor
}

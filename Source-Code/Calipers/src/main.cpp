#include "MeshMain.h"
#include "TextureMain.h"
#include <GL/glut.h>
#include "masterPly.h"
#include "masterSettings.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "matrix4x4.h"

using namespace std;

int REAL_TIME_FPS = 10;
int REAL_TIME_PORT = 3232;
int CLI_PORT = 15000;
string SERVER_IP = "127.0.0.1";
int SERVER_PORT = 11969;
int REAL_TIME = 1;
int PERSISTENCE = 0;
int LOG_LEVEL = 0;
int FPS = 10;
int MAX_PACKAGE_SIZE = 60000;

MeshMain* mMain;
TextureMain* tMain;

bool modeMesh = true;

vector<MasterClient*> clients;

void saveXmlClient(ofxXmlSettings* pSettings, MasterClient* client, bool isPlayer) {

    ofxXmlSettings settings = *pSettings;
    settings.setValue("cliId", client->idClient);
    int count2D = 0;
    int count3D = 1;

    if (!isPlayer) {
        settings.setValue("cliPort", CLI_PORT++);
        settings.setValue("serverIp", SERVER_IP);
        settings.setValue("serverPort", SERVER_PORT);
        settings.setValue("realTime", REAL_TIME);
        settings.setValue("persistence", PERSISTENCE);
        settings.setValue("logLevel", LOG_LEVEL);
        settings.setValue("fps", FPS);
        settings.setValue("maxPackageSize", MAX_PACKAGE_SIZE);
    }
    settings.setValue("alfaCoord", mMain->cloudModel[1]->AlfaCoord);

    settings.addTag("cameras");
    settings.pushTag("cameras");

    for (int i = 0; i < client->cameras.size(); i++) {
        MasterCamera* camera = client->cameras[i];
        settings.addTag("camera");
        settings.pushTag("camera", i);

        settings.addValue("id", camera->idCamera);
        if (camera->is3D) {
            settings.addValue("deviceInstance", count3D);
            count3D++;
        } else {
            settings.addValue("deviceInstance", count2D);
            count2D++;
        }
        if (!isPlayer) {
            settings.addValue("resolutionX", 640);//800
            settings.addValue("resolutionY", 480);//600
            settings.addValue("resolutionDownSample", 1);
            settings.addValue("FPS", 30);// camera i fps
            settings.addValue("colorRGB", true);// camera i color rgb
            settings.addValue("dataContext", "");// data context

            settings.addTag("depthSettings");
            settings.pushTag("depthSettings");
            settings.addValue("near", 10);
            settings.addValue("far", 100);
            settings.addValue("pointsDownSample", 4);
            settings.popTag();
        }
        settings.addValue("use2D", camera->is2D);// camera i use 2d
        settings.addValue("use3D", camera->is3D);// camera i use 3d

        if (camera->is2D && !isPlayer) {
            settings.addTag("coefficients2D");
            settings.pushTag("coefficients2D");
            settings.addValue("k1", tMain->settings->intrinsicMaster[i+1].k1);
            settings.addValue("k2", tMain->settings->intrinsicMaster[i+1].k2);
            settings.addValue("k3", tMain->settings->intrinsicMaster[i+1].k3);
            settings.addValue("k4", tMain->settings->intrinsicMaster[i+1].k4);
            settings.addValue("fx", tMain->settings->intrinsicMaster[i+1].fx);
            settings.addValue("fy", tMain->settings->intrinsicMaster[i+1].fy);
            settings.addValue("cx", tMain->settings->intrinsicMaster[i+1].cx);
            settings.addValue("cy", tMain->settings->intrinsicMaster[i+1].cy);
            settings.popTag();
        }
        if (camera->is2D && isPlayer) {
            settings.addTag("matrixA2D");
            settings.pushTag("matrixA2D");
            GLdouble mA[16];
            MasterSettings::CalculateMatrix(camera->masterTexture->history, mA, true);
            for(int j = 0; j < 16; j++) {
                std::stringstream cellM;
                cellM << "m" << j / 4 << j % 4;
                settings.addValue(cellM.str(), (float)mA[j]);
            }
            settings.popTag();

            settings.addTag("matrixB2D");
            settings.pushTag("matrixB2D");
            GLdouble mB[16];
            MasterSettings::CalculateMatrix(camera->masterTexture->history, mB, false);
            for(int j = 0; j < 16; j++) {
                std::stringstream cellM;
                cellM << "m" << j / 4 << j % 4;
                settings.addValue(cellM.str(), (float)mB[j]);
            }
            settings.popTag();
        }
        if (camera->is3D && !isPlayer) {
            settings.addTag("matrix3D");
            settings.pushTag("matrix3D");
            MasterMesh* master = camera->masterMesh;
            GLdouble m[16];
            MasterSettings::CalculateMatrix(*master, m);

            GLdouble mm[16];
            for (int k = 0; k < 4; k++) {
                for (int j = 0; j < 4; j++) {
                    mm[k*4+j] = master->matrix[k*4+0] * m[0*4+j] + master->matrix[k*4+1] * m[1*4+j] + master->matrix[k*4+2] * m[2*4+j] + master->matrix[k*4+3] * m[3*4+j];
                }
            }

            for(int j = 0; j < 16; j++) {
                std::stringstream cellM;
                cellM << "m" << j / 4 << j % 4;
                settings.addValue(cellM.str(), (float)mm[j]);
            }
            settings.popTag();
        }

        settings.popTag();
    }

    settings.popTag();
}

void saveXmlFiles(vector<MasterClient*> clients) {
    //Player
    if (clients.size() > 0) {
        ofxXmlSettings settings;
        settings.addTag("settings");
        settings.pushTag("settings");
        for (int k = 0; k < clients.size(); k++) {
            settings.addTag("client");
            settings.pushTag("client", k);
            saveXmlClient(&settings, clients[k], true);
            settings.popTag();
        }
        settings.popTag();

        std::stringstream fileName;
        fileName << "settings.xml";
        settings.saveFile(fileName.str());
    }
    //Clients
    for (int k = 0; k < clients.size(); k++) {
        ofxXmlSettings settings;
        settings.addTag("settings");
        settings.pushTag("settings");
        saveXmlClient(&settings, clients[k], false);
        settings.popTag();

        std::stringstream fileName;
        fileName << "settings" << k << ".xml";
        settings.saveFile(fileName.str());
    }
}

void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master) {
    GLdouble m[16];
    MasterSettings::CalculateMatrix(master, m);
    model->Include(newModel, m);
}

void display(void) {
    if (modeMesh)
        mMain->display();
    else
        tMain->display();
}

void keys(unsigned char key, int x, int y) {
    if (key == 't'){
        if (mMain->faces != NULL){
            modeMesh = false;
            tMain->textureModel->MemoryLoadCalibrator(mMain->faces,*(mMain->numberFaces));
        }
    }
    if (key == ' ') {
        mMain->cameraAll = !mMain->cameraAll;
    }
    if (modeMesh)
        mMain->keys(key, x, y);
    else
        tMain->keys(key, x, y);

    if (key == 'o' && !modeMesh){
        saveXmlFiles(clients);
    }
}

void mouse(int btn, int state, int x, int y) {
    if (modeMesh)
        mMain->mouse(btn, state, x, y);
    else
        tMain->mouse(btn, state, x, y);
}

void mouseMove(int x, int y) {
    if (modeMesh)
        mMain->mouseMove(x,y);
    else
        tMain->mouseMove(x,y);
}

void myReshape(int w, int h) {
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    if (w <= h) {
        glFrustum(-.5, .5, -.5 * (GLfloat)h / (GLfloat)w, .5 * (GLfloat)h / (GLfloat)w, .5, 20.0);
    } else {
        glFrustum(-.5 * (GLfloat)w / (GLfloat)h, .5 *(GLfloat)w / (GLfloat)h, -.5, .5, .5, 20.0);
    }
	glMatrixMode(GL_MODELVIEW);
}

void timerFunction(int arg) {
    if (modeMesh){
        glutTimerFunc(mMain->doubleClickTime, timerFunction, 0);
        if (mMain->clickCount > 1) {
            mMain->cameraAll = !mMain->cameraAll;
        }
        mMain->clickCount = 0;
    }
    else{
    }
}

vector<string> getCloudFiles() {
    vector<string> files;
    ofDirectory directory("data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    ofDirectory cameraDirectory(allClientFiles[j].path());
                    cameraDirectory.allowExt("xyz");
                    cameraDirectory.listDir();
                    if (cameraDirectory.numFiles() > 0){
                        files.push_back(cameraDirectory.getPath(0));
                    }
                }
            }
        }
    }
    return files;
}

vector<string> getImageFiles() {
    vector<string> files;
    ofDirectory directory("data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    ofDirectory cameraDirectory(allClientFiles[j].path());
                    cameraDirectory.allowExt("png");
                    cameraDirectory.allowExt("bmp");
                    cameraDirectory.allowExt("jpg");
                    cameraDirectory.listDir();
                    if (cameraDirectory.numFiles() > 0){
                        files.push_back(cameraDirectory.getPath(0));
                    }
                }
            }
        }
    }

    return files;
}

vector<MasterClient*> getClients(int textureCount, MasterTexture* textureMaster, int meshCount, MasterMesh* cloudMaster) {
    vector<MasterClient*> clients;
    int idClient = 0;
    int idCameraM = 0;
    int idCameraT = 0;
    ofDirectory directory("data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            MasterClient* client = new MasterClient();
            int idCamera = 0;

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    MasterCamera* camera = new MasterCamera();

                    ofDirectory cludDirectory(allClientFiles[j].path());
                    cludDirectory.allowExt("xyz");
                    cludDirectory.listDir();

                    camera->is3D = cludDirectory.numFiles() > 0 && idCameraM < meshCount;
                    if (camera->is3D) {
                        camera->masterMesh = &cloudMaster[idCameraM+1];
                        idCameraM++;
                    }

                    ofDirectory imageDirectory(allClientFiles[j].path());
                    imageDirectory.allowExt("png");
                    imageDirectory.allowExt("bmp");
                    imageDirectory.allowExt("jpg");
                    imageDirectory.listDir();

                    camera->is2D = imageDirectory.numFiles() > 0 && idCameraT < textureCount;
                    if (camera->is2D) {
                        camera->masterTexture = &textureMaster[idCameraT+1];
                        idCameraT++;
                    }

                    if (camera->is2D || camera->is3D) {
                        camera->idCamera = idCamera++;
                        client->cameras.push_back(camera);
                    }
                }
            }

            if (client->cameras.size() > 0) {
                client->idClient = idClient++;
                clients.push_back(client);
            }
        }
    }
    return clients;
}

int main(int argc, char **argv) {

    mMain = new MeshMain();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
    glutInitWindowPosition(300, 300);
	glutCreateWindow("Calibration project");
	glutTimerFunc(mMain->doubleClickTime,timerFunction,0);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glEnable(GL_DEPTH_TEST);

    ///INICIALIZACION CALIBRACION 3D

    /* Cloud */
    vector<string> files3D = getCloudFiles();
    mMain->meshCount = files3D.size();
    mMain->cloudMaster = new MasterMesh[mMain->meshCount + 1];
    mMain->cloudModel = new Model_XYZ*[mMain->meshCount + 1];
    for (int i = 0; i <= mMain->meshCount; i++) {
        for (int j = 0; j < 3; j++) {
            mMain->cloudMaster[i].viewer[j] = 0.0;
            mMain->cloudMaster[i].rotate[j] = 0.0;
        }
        mMain->cloudModel[i] = new Model_XYZ();
    }

    /* Settings and files */
    mMain->settings = new MasterSettings(0, NULL, mMain->meshCount, mMain->cloudMaster);
    mMain->settings->loadMeshCalibration();

    for (int i = 1; i <= mMain->meshCount; i++) {
        for (int j = 0; j < 16; j++) {
            mMain->cloudMaster[i].matrix[j] = mMain->settings->meshMaster[i].matrix[j];
        }
    }

    /* LoadClouds */
    for (int i = 0; i < mMain->meshCount; i++) {
        float mm[16];
        for (int j = 0; j < 16; j++) {
            mm[j] = mMain->cloudMaster[i+1].matrix[j];
        }
        if (i == 0) {
            mMain->cloudModel[i+1]->Load(files3D[i].c_str(), 0, mm);
        } else {
            mMain->cloudModel[i+1]->Load(files3D[i].c_str(), mMain->cloudModel[1]->AlfaCoord, mm);
        }
    }

    mMain->cloudModel[0]->Clear();
    for (int i = 1; i <= mMain->meshCount; i++) {
        IncludeMesh(mMain->cloudModel[0], mMain->cloudModel[i], mMain->cloudMaster[i]);
    }
    mMain->meshIndex = 0;

    ///INICIALIZACION CALIBRACION 2D

    tMain = new TextureMain();
    /* Mesh */
    tMain->textureModel = new Model_PLY();

    /* Texture */
    vector<string> files2D = getImageFiles();
    tMain->textureCount = files2D.size();
    tMain->textureMaster = new MasterTexture[tMain->textureCount + 1];
    tMain->faces = new int*[tMain->textureCount + 1];
    for (int i = 0; i <= tMain->textureCount; i++) {
        for (int j = 0; j < 3; j++) {
            tMain->textureMaster[i].viewer[j] = 0.0;
            tMain->textureMaster[i].rotate[j] = 0.0;
        }
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                tMain->textureMaster[i].matrix[j * 4 + k] = j == k ? 1 : 0;
            }
        }
        tMain->faces[i] = new int[tMain->facesCount];
        for (int k = 0; k < tMain->facesCount; k++) {
            tMain->faces[i][k] = 0;
        }
    }

    /* Settings and files */
    clients = getClients(tMain->textureCount, tMain->textureMaster, mMain->meshCount, mMain->cloudMaster);
    tMain->settings = new MasterSettings(tMain->textureCount, tMain->textureMaster, 0, NULL);
    tMain->settings->loadIntrinsicCalibration();

    /* Texture config */
    glGenTextures(tMain->textureCount, tMain->textures);
    glActiveTextureARB       = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB     = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
    glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
    glGenQueriesARB = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
    glEndQueryARB = (PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
    glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuivARB");
    if( !glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB ) {
        MessageBox(NULL,"One or more GL_ARB_multitexture functions were not found", "ERROR",MB_OK|MB_ICONEXCLAMATION);
        return -1;
    }

    /* LoadImages */
    tMain->textureImage = new Model_IMG[tMain->textureCount];
    for (int i = 0; i < tMain->textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tMain->textures[i]);
        glDisable(GL_TEXTURE_2D);
        tMain->textureIndex = i + 1;
        tMain->loadLightMapTexture(&(tMain->textureImage[i]), files2D[i]);
    }
    tMain->textureIndex = 0;
	glutMainLoop();
}


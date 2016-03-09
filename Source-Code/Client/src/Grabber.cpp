#include "Grabber.h"
#include "Thread2D.h"

#include "ofxSimpleGuiToo.h"

ThreadONI * tONI;
bool compare_texture(DebugTexture * first, DebugTexture * second) {
    if(first->tipo == second->tipo) {
        return (first->id < second->id);
    }
    return (first->tipo < second->tipo);
}

void Grabber::setConnected(bool con) {
    connected = con;
}

//--------------------------------------------------------------
void Grabber::setup() {
    pthread_mutex_init(&uiImagesMutex, NULL);
    gdata       = new GlobalData();
    connected   = false;
    gdata->loadCalibData("settings.xml");
    ofLogLevel(OF_LOG_VERBOSE);
    b_exit          = false;
    b_exit_fired    = false;
    b_exit_pressed  = false;
    setupGui();
    downsampling    = 4;
    switch(gdata->sys_data->logLevel) {
        case 0: ofSetLogLevel(OF_LOG_VERBOSE); break;
        case 1: ofSetLogLevel(OF_LOG_NOTICE); break;
        case 2: ofSetLogLevel(OF_LOG_WARNING); break;
        case 3: ofSetLogLevel(OF_LOG_ERROR); break;
        case 4: ofSetLogLevel(OF_LOG_FATAL_ERROR); break;
        case 5: ofSetLogLevel(OF_LOG_SILENT); break;
        default:ofSetLogLevel(OF_LOG_VERBOSE); break;
    }

    FrameUtils::init();

    ofSetFrameRate(gdata->sys_data->fps);

    total2D     = gdata->total2D;    //Hacer que se cargue dinámico.
    total3D     = gdata->total3D;    //Hacer que se cargue dinámico.
    totalONI    = gdata->totalONI;   //Hacer que se cargue dinámico.

    if((gdata->total2D + gdata->total3D + gdata->totalONI) > 0) {
        tData = new ThreadData[gdata->total2D + gdata->total3D + gdata->totalONI];
        for(int w = 0; w < (gdata->total2D + gdata->total3D + gdata->totalONI); w++) {
            tData[w].cliId = gdata->sys_data->cliId;
        }
    } else {
        tData = NULL;
    }

    t_camera * camera = gdata->camera;

    if(gdata->total2D > 0) {
        t2D   = new Thread2D[gdata->total2D];
    }

    if(gdata->total3D > 0) {
        t3D   = new Thread3D[gdata->total3D];
    }

    if(gdata->totalONI > 0) {
        tONI  = new ThreadONI[gdata->totalONI];
    }

    int i2D     = 0;
    int i3D     = 0;
    int iONI    = 0;

    while(camera != NULL) {
        if(camera->useONI) {
            tONI[iONI]._id = i3D + iONI;
            tONI[iONI].sys_data                   = gdata->sys_data;
            tONI[iONI].context                    = camera;
            tONI[iONI].startThread(true, false);
            iONI ++;
        } else if(camera->use3D) {
            t3D[i3D]._id = i3D + iONI;
            t3D[i3D].sys_data                   = gdata->sys_data;
            t3D[i3D].context                    = camera;
            t3D[i3D].startThread(true, false);
            i3D ++;
        } else {
            t2D[i2D].sys_data                   = gdata->sys_data;
            t2D[i2D].context                    = camera;
            t2D[i2D].startThread(true, false);
            i2D ++;
        }
        camera = camera->sig;
    }
    ofLogVerbose() << "[Grabber::setup] " << gdata->sys_data->goLive;
    if(gdata->sys_data->goLive == 1) { //Si desde el Calibrator se indicó que se debe trasmitir en vivo.
        transmitter.grabber     = this;
        transmitter.sys_data    = gdata->sys_data;
        transmitter.startThread(true, true);
    }
    ofVideoGrabber vid;
    vid.listDevices();
    myfont.loadFont("HelveticaNeueLTStd Bd.otf", 8);
}

void Grabber::setupGui() {
    gui.addFPSCounter();
    gui.addTitle("CLI ID: " + ofToString(gdata->sys_data->cliId));
    gui.addToggle("Connect", connected);

    std::string exstr = "Exit";
    gui.addButton(exstr, b_exit_pressed);
    gui.show();
}

//--------------------------------------------------------------
void Grabber::update() {
    if(b_exit_fired) return;
    if(b_exit) {
        connected = false;
        if((transmitter.state == -1) || (transmitter.state == 3) || (!transmitter.started)) {
            if(!b_exit_fired) {
                b_exit_fired = true;
                ofExit();
            }
        }
        return;
    }

    if(b_exit_pressed) {
        b_exit = true;
    }

}

std::string Grabber::getCamTag(int id, int tipo) {
    std::string stipo;
    if(tipo == 0) {
        stipo = "RGB";
    } else if(tipo == 1) {
        stipo = "KINECT";
    } else {
        stipo = "ONI";
    }

    std::string title   = "TYPE: " + stipo + ", ID CAM: " + ofToString(id);
    return title;
}

void Grabber::drawTag(std::string msj, int x, int y) {
    y += 15;
    ofSetColor(0, 0, 0);
    myfont.drawString(msj, x+1, y+145+1);
    ofSetColor(250, 250, 250);
    myfont.drawString(msj, x, y+145);
    ofSetColor(256, 256, 256);
}

//ofImage     tmpImg;
ofImage     tmpImg2D[20];
int inc2D   = 0;
ofImage     tmpImg3D[20];
int inc3D   = 0;
ofImage     tmpImgONI[20];
int incONI  = 0;

int ySep    = 170;
int items   = 1;

//--------------------------------------------------------------
void Grabber::draw() {

    if(b_exit) return;
    if(gui.isOn()) {
        items       = 1;
        int y       = ySep * items;
        int x       = 15;
        int i;

        for(i=0; i<gdata->total2D; i++) {
            if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) {
                pthread_mutex_lock(&t2D[i].uiMutex);
                drawTag(getCamTag(t2D[i].context->id, 0), x, y);
                if(t2D[i].img.isAllocated()) {
                    if(inc2D < i) { inc2D = i; }
                    pthread_mutex_lock(&uiImagesMutex);
                    tmpImg2D[i].clone(t2D[i].img);
                    pthread_mutex_unlock(&uiImagesMutex);
                }
                items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
                pthread_mutex_unlock(&t2D[i].uiMutex);
            }
        }

        for(i=0; i<gdata->total3D; i++) {
            if(t3D[i].isDeviceInitted()) { //Si la cámara está inicializada.
                if(t3D[i].context->use2D == 1) {
                    pthread_mutex_lock(&t3D[i].uiMutex);
                    drawTag(getCamTag(t3D[i].context->id, 1), x, y);
                    if(t3D[i].img.isAllocated()) {
                        if(inc3D < i) { inc3D = i; }
                        pthread_mutex_lock(&uiImagesMutex);
                        tmpImg3D[i].clone(t3D[i].img);
                        pthread_mutex_unlock(&uiImagesMutex);
                    }
                    items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
                    pthread_mutex_unlock(&t3D[i].uiMutex);
                }
            }
        }

        for(i=0; i<gdata->totalONI; i++) {
            if(tONI[i].isDeviceInitted()) { //Si la cámara está inicializada.
                if(tONI[i].context->use2D == 1) {
                    pthread_mutex_lock(&tONI[i].uiMutex);
                    drawTag(getCamTag(tONI[i].context->id, 2), x, y);
                    if(tONI[i].img.isAllocated()) {
                        if(incONI < i) { incONI = i; }
                        pthread_mutex_lock(&uiImagesMutex);
                        tmpImgONI[i].clone(tONI[i].img);
                        pthread_mutex_unlock(&uiImagesMutex);
                    }
                    items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
                    pthread_mutex_unlock(&tONI[i].uiMutex);
                }
            }
        }

        items   = 1;
        y       = ySep * items;
        x       = 15;

        /*for(i=0; i<=inc2D; i++) {
            pthread_mutex_lock(&uiImagesMutex);
            try {
                if(tmpImg2D[i].isAllocated()) {
                    tmpImg2D[i].draw(x, y, 200, 150);
                }
            } catch (...) {
                ofLogWarning() << "[FrameUtils::getFrameSize] - Exception occurred.";
            }
            pthread_mutex_unlock(&uiImagesMutex);
            items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
        }*/
        for(i=0; i<=inc3D; i++) {
            pthread_mutex_lock(&uiImagesMutex);
            if(tmpImg3D[i].isAllocated()) {
                tmpImg3D[i].draw(x, y, 200, 150);
            }
            pthread_mutex_unlock(&uiImagesMutex);
            items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
        }
        for(i=0; i<=incONI; i++) {
            pthread_mutex_lock(&uiImagesMutex);
            if(tmpImgONI[i].isAllocated()) {
                tmpImgONI[i].draw(x, y, 200, 150);
            }
            pthread_mutex_unlock(&uiImagesMutex);
            items++; if(items >= 3) { x+=216; items = 0; } y = ySep * items;
        }
        /**/
        gui.draw();
        return;
    }
}

bool Grabber::isConnected() {
    return connected;
}


//Operación invocada por Transmitter para refrezcar la información a trasmitir.
void Grabber::updateThreadData() {
    //return;

    int di      = 0;
    int i       = 0;
    ofxCvColorImage cvim;

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 2D
    */
    ofLogVerbose() << "ACTUALIZO LA INFO DE LAS CÁMARAS 2D";
    for(i; i<gdata->total2D; i++) {
        tData[di].camId         = t2D[i].context->id;
        tData[di].state         = 0;
        tData[di].cameraType    = 1;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la cámara está inicializada.
            tData[di].state  = DEVICE_2D; // 2D
            tData[di].compressed    = t2D[i].context->useCompression;

            pthread_mutex_lock(&t2D[i].uiMutex);

            if(t2D[i].context->hasCoef2D) {
                cvim.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.getWidth(), t2D[i].img.getHeight());
                IplImage* src = cvim.getCvImage();
                cvim.undistort( t2D[i].context->coef2Da.x, t2D[i].context->coef2Da.y, t2D[i].context->coef2Da.z, t2D[i].context->coef2Da.w,
                                t2D[i].context->coef2Db.x, t2D[i].context->coef2Db.y, t2D[i].context->coef2Db.z, t2D[i].context->coef2Db.w);
                tData[di].img.setFromPixels(cvim.getPixels(), cvim.getWidth(), cvim.getHeight(), OF_IMAGE_COLOR);
            } else {
                tData[di].img.clone(t2D[i].img);
            }

            int imgW                = (int)t2D[i].img.getWidth()*t2D[i].context->resolutionDownSample;
            int imgH                = (int)t2D[i].img.getHeight()*t2D[i].context->resolutionDownSample;

            pthread_mutex_unlock(&t2D[i].uiMutex);

            tData[di].qfactor       = t2D[i].context->rgbCompressionQuality;

            tData[di].imgWidth      = imgW;
            tData[di].imgHeight     = imgH;

            if(t2D[i].context->resolutionDownSample != 1) {
                tData[di].img.resize(imgW, imgH);
            }

            tData[di].img.mirror(false, true);

            tData[di].imgrow1.set(t2D[i].context->imgrow1.x, t2D[i].context->imgrow1.y, t2D[i].context->imgrow1.z, t2D[i].context->imgrow1.w);
            tData[di].imgrow2.set(t2D[i].context->imgrow2.x, t2D[i].context->imgrow2.y, t2D[i].context->imgrow2.z, t2D[i].context->imgrow2.w);
            tData[di].imgrow3.set(t2D[i].context->imgrow3.x, t2D[i].context->imgrow3.y, t2D[i].context->imgrow3.z, t2D[i].context->imgrow3.w);
            tData[di].imgrow4.set(t2D[i].context->imgrow4.x, t2D[i].context->imgrow4.y, t2D[i].context->imgrow4.z, t2D[i].context->imgrow4.w);

        }
        tData[di].nubeW = tData[di].nubeH = 0;
        gettimeofday(&tData[di].curTime, NULL);
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 3D
    */
    ofLogVerbose() << "ACTUALIZO LA INFO DE LAS CÁMARAS 3D";
    i = 0;

    for(i; i<gdata->total3D; i++) {
        tData[di].state         = 0;
        tData[di].camId         = t3D[i].context->id;
        tData[di].cameraType    = 2;

        if(t3D[i].isDeviceInitted()) { //Si la cámara está inicializada.

            if(t3D[i].context->use2D == 1) {
                tData[di].state    = DEVICE_2D;
                //Clono la imágen
                tData[di].compressed    = t3D[i].context->useCompression;

                pthread_mutex_lock(&t3D[i].uiMutex);

                if(t3D[i].context->hasCoef2D) {
                    cvim.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.getWidth(), t3D[i].img.getHeight());
                    IplImage* src = cvim.getCvImage();
                    cvim.undistort( t3D[i].context->coef2Da.x, t3D[i].context->coef2Da.y, t3D[i].context->coef2Da.z, t3D[i].context->coef2Da.w,
                                    t3D[i].context->coef2Db.x, t3D[i].context->coef2Db.y, t3D[i].context->coef2Db.z, t3D[i].context->coef2Db.w);
                    tData[di].img.setFromPixels(cvim.getPixels(), cvim.getWidth(), cvim.getHeight(), OF_IMAGE_COLOR);
                } else {
                    tData[di].img.clone(t3D[i].img);
                }

                tData[di].imgWidth      = (int)t3D[i].img.getWidth()*t3D[i].context->resolutionDownSample;
                tData[di].imgHeight     = (int)t3D[i].img.getHeight()*t3D[i].context->resolutionDownSample;

                if(t3D[i].context->resolutionDownSample != 1) {
                    tData[di].img.resize(tData[di].imgWidth, tData[di].imgHeight);
                }
                pthread_mutex_unlock(&t3D[i].uiMutex);

                tData[di].qfactor   = t3D[i].context->rgbCompressionQuality;
                tData[di].nubeW     = tData[di].nubeH = 0;
            }

            if(t3D[i].context->use3D == 1) {
                ((t3D[i].context->use2D == 1) ? tData[di].state = DEVICE_2D_3D : tData[di].state = DEVICE_3D);
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.
                pthread_mutex_lock(&t3D[i].uiMutex);
                tData[di].nubeW = t3D[i].spix.getWidth();
                tData[di].nubeH = t3D[i].spix.getHeight();

                rawPix          = t3D[i].spix.getPixels();
                pthread_mutex_unlock(&t3D[i].uiMutex);
                Xn_depth        = &t3D[i].openNIRecorder->getDepthGenerator();

                y   = 0;
                x   = 0;
                d   = 0;
                tData[di].nubeLength    = 0;

                tmpX = NULL;
                tmpY = NULL;
                tmpZ = NULL;

                if((tData[di].nubeW > 0) && (tData[di].nubeH > 0)) {
                    tmpX = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpY = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpZ = new float[tData[di].nubeW * tData[di].nubeH];
                }

                tData[di].row1.set(t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w);
                tData[di].row2.set(t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w);
                tData[di].row3.set(t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w);
                tData[di].row4.set(t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                matrix.set( t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w,
                            t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w,
                            t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w,
                            t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                for(y=0; y < tData[di].nubeH; y += t3D[i].context->pcDownSample) {
                    for(x=0; x < tData[di].nubeW; x += t3D[i].context->pcDownSample) {
                        d = rawPix[y * tData[di].nubeW + x];
                        if(d != 0) {
                            Point2D.X   = x;
                            Point2D.Y   = y;
                            Point2D.Z   = (float)d;
                            try {

                                Xn_depth->ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                v1.set(Point3D.X, Point3D.Y, Point3D.Z);

                                vt = transformPoint(v1, matrix);
                                if(((int)vt->x == 0) && ((int)vt->x == 0) && ((int)vt->x == 0)) {

                                } else {
                                    tmpX[tData[di].nubeLength] = vt->x;
                                    tmpY[tData[di].nubeLength] = vt->y;
                                    tmpZ[tData[di].nubeLength] = vt->z;
                                    tData[di].nubeLength ++;
                                }
                                delete vt;

                            } catch(...) {
                                ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepción al transformar los puntos.";
                            }
                        }
                    }
                }

                if(tData[di].xpix != NULL) {
                    delete tData[di].xpix;
                    delete tData[di].ypix;
                    delete tData[di].zpix;
                    tData[di].xpix = NULL;
                    tData[di].ypix = NULL;
                    tData[di].zpix = NULL;
                }

                if(tData[di].nubeLength > 0) {
                    tData[di].xpix = new float[tData[di].nubeLength];
                    tData[di].ypix = new float[tData[di].nubeLength];
                    tData[di].zpix = new float[tData[di].nubeLength];

                    memcpy(tData[di].xpix,     tmpX,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].ypix,     tmpY,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].zpix,     tmpZ,     sizeof(float) * tData[di].nubeLength);
                }

                if(tmpX != NULL) {
                    delete tmpX;
                    delete tmpY;
                    delete tmpZ;
                }

            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS ONI
    */
    ofLogVerbose() << "ACTUALIZO LA INFO DE LAS CÁMARAS ONI" << endl;
    i = 0;
    for(i; i<gdata->totalONI; i++) {
        tData[di].state         = 0;
        tData[di].camId         = tONI[i].context->id;
        tData[di].cameraType    = 2;

        if(tONI[i].isDeviceInitted()) { //Si la cámara está inicializada.

            if(tONI[i].context->use2D == 1) {
                tData[di].state    = DEVICE_2D;
                //Clono la imágen
                tData[di].compressed    = tONI[i].context->useCompression;

                pthread_mutex_lock(&tONI[i].uiMutex);

                if(tONI[i].context->hasCoef2D) {
                    cvim.setFromPixels(tONI[i].img.getPixels(), tONI[i].img.getWidth(), tONI[i].img.getHeight());
                    IplImage* src = cvim.getCvImage();
                    cvim.undistort( tONI[i].context->coef2Da.x, tONI[i].context->coef2Da.y, tONI[i].context->coef2Da.z, tONI[i].context->coef2Da.w,
                                    tONI[i].context->coef2Db.x, tONI[i].context->coef2Db.y, tONI[i].context->coef2Db.z, tONI[i].context->coef2Db.w);
                    tData[di].img.setFromPixels(cvim.getPixels(), cvim.getWidth(), cvim.getHeight(), OF_IMAGE_COLOR);
                } else {
                    tData[di].img.clone(tONI[i].img);
                }

                tData[di].imgWidth      = (int)tONI[i].img.getWidth()*tONI[i].context->resolutionDownSample;
                tData[di].imgHeight     = (int)tONI[i].img.getHeight()*tONI[i].context->resolutionDownSample;

                if(tONI[i].context->resolutionDownSample != 1) {
                    tData[di].img.resize(tData[di].imgWidth, tData[di].imgHeight);
                }

                pthread_mutex_unlock(&tONI[i].uiMutex);

                tData[di].qfactor       = tONI[i].context->rgbCompressionQuality;

                tData[di].nubeW = tData[di].nubeH = 0;
            }

            if(tONI[i].context->use3D == 1) {

                ((tONI[i].context->use2D == 1) ? tData[di].state = DEVICE_2D_3D : tData[di].state = DEVICE_3D);
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.
                pthread_mutex_lock(&tONI[i].uiMutex);

                tData[di].nubeW = tONI[i].spix.getWidth();
                tData[di].nubeH = tONI[i].spix.getHeight();
                rawPix          = tONI[i].spix.getPixels();

                pthread_mutex_unlock(&tONI[i].uiMutex);

                Xn_depth        = &tONI[i].openNIRecorder->getDepthGenerator();

                y   = 0;
                x   = 0;
                d   = 0;
                tData[di].nubeLength    = 0;

                tmpX = NULL;
                tmpY = NULL;
                tmpZ = NULL;

                if((tData[di].nubeW > 0) && (tData[di].nubeH > 0)) {
                    tmpX = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpY = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpZ = new float[tData[di].nubeW * tData[di].nubeH];
                }

                tData[di].row1.set(tONI[i].context->row1.x, tONI[i].context->row1.y, tONI[i].context->row1.z, tONI[i].context->row1.w);
                tData[di].row2.set(tONI[i].context->row2.x, tONI[i].context->row2.y, tONI[i].context->row2.z, tONI[i].context->row2.w);
                tData[di].row3.set(tONI[i].context->row3.x, tONI[i].context->row3.y, tONI[i].context->row3.z, tONI[i].context->row3.w);
                tData[di].row4.set(tONI[i].context->row4.x, tONI[i].context->row4.y, tONI[i].context->row4.z, tONI[i].context->row4.w);

                matrix.set( tONI[i].context->row1.x, tONI[i].context->row1.y, tONI[i].context->row1.z, tONI[i].context->row1.w,
                            tONI[i].context->row2.x, tONI[i].context->row2.y, tONI[i].context->row2.z, tONI[i].context->row2.w,
                            tONI[i].context->row3.x, tONI[i].context->row3.y, tONI[i].context->row3.z, tONI[i].context->row3.w,
                            tONI[i].context->row4.x, tONI[i].context->row4.y, tONI[i].context->row4.z, tONI[i].context->row4.w);

                for(y=0; y < tData[di].nubeH; y += tONI[i].context->pcDownSample) {
                    for(x=0; x < tData[di].nubeW; x += tONI[i].context->pcDownSample) {
                        d = rawPix[y * tData[di].nubeW + x];
                        if(d != 0) {
                            Point2D.X   = x;
                            Point2D.Y   = y;
                            Point2D.Z   = (float)d;
                            try {

                                Xn_depth->ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                v1.set(Point3D.X, Point3D.Y, Point3D.Z);

                                vt = transformPoint(v1, matrix);
                                if(((int)vt->x == 0) && ((int)vt->x == 0) && ((int)vt->x == 0)) {

                                } else {
                                    tmpX[tData[di].nubeLength] = vt->x;
                                    tmpY[tData[di].nubeLength] = vt->y;
                                    tmpZ[tData[di].nubeLength] = vt->z;
                                    tData[di].nubeLength ++;
                                }
                                delete vt;

                            } catch(...) {
                                ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepción al transformar los puntos.";
                            }
                        }
                    }
                }

                if( (tData[di].nubeLength == 0) || (tData[di].nubeW == 0) || (tData[di].nubeH == 0)) {
                    tData[di].state = 0;
                    if(tONI[i].context->use2D == 1) {
                        tData[di].state    = DEVICE_2D;
                    }
                }
                cout << " tData[di].nubeLength " << tData[di].nubeLength << endl;
                if(tData[di].xpix != NULL) {
                    delete tData[di].xpix;
                    delete tData[di].ypix;
                    delete tData[di].zpix;
                    tData[di].xpix = NULL;
                    tData[di].ypix = NULL;
                    tData[di].zpix = NULL;
                }

                if(tData[di].nubeLength > 0) {
                    tData[di].xpix = new float[tData[di].nubeLength];
                    tData[di].ypix = new float[tData[di].nubeLength];
                    tData[di].zpix = new float[tData[di].nubeLength];

                    memcpy(tData[di].xpix,     tmpX,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].ypix,     tmpY,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].zpix,     tmpZ,     sizeof(float) * tData[di].nubeLength);
                }

                if(tmpX != NULL) {
                    delete tmpX;
                    delete tmpY;
                    delete tmpZ;
                }
            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        di++;
    }

}

ofVec3f * Grabber::transformPoint(ofVec3f point, ofMatrix4x4 transform) {
    ofVec4f v;
    v.set(point.x, point.y, point.z, 1);
    v            = transform * v;
    ofVec3f * v3 = new ofVec3f(v.x, v.y, v.z);
    return v3;
}

void Grabber::exit() {
    //Stop the thread
    cout << "[Grabber::exit]" << endl;
    int i       = 0;
    for(i; i<gdata->total2D; i++) {
        if(t2D[i].isThreadRunning()) {
            t2D[i].exit();
            t2D[i].waitForThread();
        }
    }
    //cout << "[Grabber::exit] a" << endl;
    i = 0;
    for(i; i<gdata->total3D; i++) {
        if(t3D[i].isThreadRunning()) {
            t3D[i].exit();
            t3D[i].waitForThread();
        }
    }
    //cout << "[Grabber::exit] b" << endl;
    i = 0;
    for(i; i<gdata->totalONI; i++) {
        if(tONI[i].isThreadRunning()) {
            tONI[i].exit();
            tONI[i].waitForThread();
        }
    }
    //cout << "[Grabber::exit] c" << endl;
    if(gdata->sys_data->goLive == 1) {
        if(transmitter.isThreadRunning()) {
            transmitter.lock();
            transmitter.exit();
            cout << "ANTES DEL SLEEP " << ofGetElapsedTimeMillis() << endl;
            ofSleepMillis(2000);
            cout << "DESPUES DEL SLEEP " << ofGetElapsedTimeMillis() << endl;
            transmitter.unlock();
            transmitter.stopThread();
            //transmitter.stopThread();
        }
    }
    //cout << "[Grabber::exit] d" << endl;
    //ofSleepMillis(1500);

    if(t2D != NULL) {
        delete t2D;
    }
    //cout << "[Grabber::exit] e" << endl;
    if(t3D != NULL) {
        delete t3D;
    }
    //cout << "[Grabber::exit] f" << endl;
    if(tONI != NULL) {
        delete tONI;
    }
    //cout << "[Grabber::exit] g" << endl;
    if(tData != NULL) {
        i = 0;
        //cout << "[Grabber::exit] " << (gdata->total2D + gdata->total3D + gdata->totalONI) << endl;
        for(i; i<gdata->total2D + gdata->total3D + gdata->totalONI; i++) {
            if((tData[i].nubeLength > 100) && (tData[i].xpix != NULL)) {
                delete tData[i].xpix;
                delete tData[i].ypix;
                delete tData[i].zpix;
            }
        }
        //cout << "[Grabber::exit] por delete tData " << tData << endl;
        //delete tData;
    }
    //cout << "[Grabber::exit] h" << endl;
    if(gdata != NULL) {
        delete gdata;
    }

    //cout << "[Grabber::exit] END" << endl;
}

//--------------------------------------------------------------
void Grabber::keyPressed(int key) {
    exit();
}

//--------------------------------------------------------------
void Grabber::keyReleased(int key) {

}

//--------------------------------------------------------------
void Grabber::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void Grabber::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void Grabber::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void Grabber::dragEvent(ofDragInfo dragInfo) {

}

#include "testApp.h"

testApp::testApp() {
    ofxXmlSettings settings;
    settings.loadFile("detector_settings.xml");

    if(settings.pushTag("settings")) {
        numRGBDevices   = settings.getValue("totalRGBDevices", 0);
    }

    openNIRecorder  = NULL;
    step            = 0;
    currDevices     = 1;
    numDevices      = 0;
    currRGBDevice   = 1;
    deviceInited    = false;
    keyP            = false;
    readyToSave     = false;

    w = 640;
    h = 480;
}

testApp::~testApp() {
    if(openNIRecorder != NULL) {
        delete openNIRecorder;
    }
}

//--------------------------------------------------------------
void testApp::setup() {
    ofSetLogLevel(OF_LOG_VERBOSE);

    openNIRecorder  = new ofxOpenNI(1);
    numDevices      = openNIRecorder->getNumDevices();

    openNIRecorder->stop();

    img.allocate(w, h, OF_IMAGE_COLOR);
    myfont.loadFont("HelveticaNeueLTStd Bd.otf", 12);
}

//--------------------------------------------------------------
void testApp::update() {
    if(step == 0) {
        //cout << "step " << step << ", currRGBDevice " << currRGBDevice  << ", numRGBDevices " << numRGBDevices << endl;
        if(currRGBDevice <= numRGBDevices) {
            if(!deviceInited) {
                vidGrabber.setVerbose(true);
                vidGrabber.initGrabber(w, h);
                vidGrabber.setDeviceID(currRGBDevice);
                deviceInited = true;
                readyToSave  = false;
            } else if(vidGrabber.isInitialized()) {

                vidGrabber.grabFrame();
                if(!readyToSave) {
                    readyToSave = vidGrabber.isFrameNew();
                } else {
                    img.setFromPixels(vidGrabber.getPixels(), w, h, OF_IMAGE_COLOR, true);
                }

                if(keyP) {

                    keyP        = false;

                    if(readyToSave) {
                        string path = ofToDataPath("rgb_devices");
                        ofDirectory::createDirectory(path, true, true);
                        path        = path + "/rgb_device_" + ofToString(currRGBDevice) + ".jpg";
                        img.saveImage(path.c_str());
                    }
                    if(readyToSave) {
                        try {
                            vidGrabber.close();
                        } catch(exception& e) {}
                    }

                    currRGBDevice++;
                    if(currRGBDevice <= numRGBDevices) {
                        deviceInited    = false;
                    }
                }


            } else {
                if(keyP) {
                    keyP    = false;
                    currRGBDevice++;
                    if(currRGBDevice <= numRGBDevices) {
                        deviceInited    = false;
                    }
                }
            }
        } else {
            step = 1;
            deviceInited    = false;
            currDevices     = 1;
            if(vidGrabber.isInitialized()) {
                vidGrabber.close();
            }
        }
    } else {
        //cout << "step " << step << ", currDevices " << currDevices << ", numDevices " << numDevices  << endl;
        if(currDevices <= numDevices) {
            if(!deviceInited) {
                if(openNIRecorder != NULL) {
                    delete openNIRecorder;
                    openNIRecorder = NULL;
                }
                openNIRecorder  = new ofxOpenNI(currDevices);
                openNIRecorder->setup();
                openNIRecorder->addImageGenerator();
                openNIRecorder->addDepthGenerator();
                openNIRecorder->setRegister(true);
                openNIRecorder->setMirror(true);
                openNIRecorder->setUseDepthRawPixels(true);
                openNIRecorder->start();
                deviceInited    = true;
            }
            if(openNIRecorder->isContextReady()) {
                openNIRecorder->update();

                if(openNIRecorder->isNewFrame()) {
                    ofPixels& ipixels = openNIRecorder->getImagePixels();
                    img.setFromPixels(ipixels.getPixels(), w, h, OF_IMAGE_COLOR, true);
                    img.mirror(false, true);
                    spix              = openNIRecorder->getDepthRawPixels();

                    if(keyP) {
                        keyP = false;
                        string path = ofToDataPath("depth_devices");
                        ofDirectory::createDirectory(path, true, true);
                        path        = path + "/depth_device_rgb_" + ofToString(currDevices) + ".jpg";
                        img.saveImage(path.c_str());

                        FILE * pFile;
                        path    = ofToDataPath("depth_devices");
                        path    = path + "/depth_device_pc_" + ofToString(currDevices) + ".xyz";
                        pFile   = fopen( path.c_str(),"w");

                        xn::DepthGenerator& Xn_depth = openNIRecorder->getDepthGenerator();
                        XnDepthPixel*  rawPix        = spix.getPixels();
                        XnPoint3D Point2D, Point3D;
                        int y, x;
                        float d;
                        for(y=0; y < spix.getHeight(); y ++) {
                            for(x=0; x < spix.getWidth(); x ++) {
                                d           = rawPix[y * spix.getWidth() + x];
                                Point2D.X   = x;
                                Point2D.Y   = y;
                                Point2D.Z   = (float)d;
                                ofFloatColor color = ipixels.getColor(x,y);


                                Xn_depth.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                fprintf (pFile, "%f %f %f\n", Point3D.X, Point3D.Y, Point3D.Z);
                            }
                        }

                        fclose (pFile);
                        openNIRecorder->stop();

                        currDevices++;
                        if(currDevices <= numDevices) {

                            deviceInited    = false;
                        }
                    }
                } else {
                    if(keyP) {
                        keyP    = false;
                        currDevices++;
                        openNIRecorder->stop();
                        if(currDevices <= numDevices) {
                            deviceInited    = false;
                        }
                    }
                }
            }
        } else {
            step            = 0;
            deviceInited    = false;
            currRGBDevice   = 1;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    img.draw(0, 0);

    string msj;
    if(step==0) {
        msj = "RGB Camera, device id: " + ofToString(currRGBDevice) + ", total: " + ofToString(numRGBDevices);
    } else {
        msj = "Depth Camera, device id: " + ofToString(currDevices) + ", total: " + ofToString(numDevices);
    }

    ofSetColor(0, 0, 0);
    myfont.drawString(msj, 12, 472);
    ofSetColor(250, 250, 250);
    myfont.drawString(msj, 10, 470);
    ofSetColor(256, 256, 256);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    if(deviceInited) {
        keyP = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

#include "Server.h"
#include "ofxNetwork.h"
#include <sys/time.h>

#include "ofxSimpleGuiToo.h"

#include <stdio.h>
#include <sys/types.h>

#include <string.h>

void Server::setupGui(string ip) {
    ServerGlobalData::debug = false;
    gui.addFPSCounter();
    gui.addTitle("IP: " + ip);
    gui.addTitle("Port: " + ofToString(gdata->sys_data->serverPort));
    gui.addTitle("Desired FPS: " + ofToString(gdata->sys_data->fps));
    gui.addTitle("Max Package Size: " + ofToString(gdata->sys_data->maxPackageSize));
    gui.addTitle("Sync Factor: " + ofToString(gdata->sys_data->syncFactorValue));
    gui.addTitle("Max Threads: " + ofToString(gdata->sys_data->maxThreadedServers));
    gui.addTitle("Max Cli Buffer: " + ofToString(gdata->sys_data->maxReceiveFrameBuffer));

    std::string exstr = "Exit";
    gui.addButton(exstr, b_exit_pressed);

    gui.show();
}

void Server::exit() {

    cout << "[Server::exit]" << endl;
    int i = 0;
    for(i = 0; i < totThreadedServers; i++) {
        tservers[i]->stopThread();
    }
    generator.stopThread();

    TCP.close();

    if(gdata != NULL) {
        delete gdata;
    }
    delete mb;
    pthread_mutex_destroy(&uiMutex);
    pthread_mutex_destroy(&tsrvMutex);
    cout << "[Server::exit] END" << endl;
}

void Server::setup() {

    /*ofFile file("filename.txt",ofFile::WriteOnly);
    file << "saving some data";
    file.close();*/

    drawables   = 0;
    gdata       = new ServerGlobalData();
    gdata->loadCalibData("settings.xml");

    pthread_mutex_init(&uiMutex, NULL);
    pthread_mutex_init(&tsrvMutex, NULL);

    myfont.loadFont("HelveticaNeueLTStd Bd.otf", 8);

    string line;
    string ip;
    ifstream IPFile;

    b_exit          = false;
    b_exit_fired    = false;
    b_exit_pressed  = false;

    int offset;
    char* search0 = "IPv4 Address. . . . . . . . . . . :";      // search pattern

    system("ipconfig > ip.txt");

    IPFile.open ("ip.txt");
    if(IPFile.is_open()) {
       while(!IPFile.eof()) {
           getline(IPFile,line);
           if ((offset = line.find(search0, 0)) != string::npos) {
               line.erase(0,39);
               cout << line << endl;
               ip = line.c_str();
               IPFile.close();
           }
        }
    }

    setupGui(ip);

    switch(gdata->sys_data->logLevel) {
        case 0: ofSetLogLevel(OF_LOG_VERBOSE); break;
        case 1: ofSetLogLevel(OF_LOG_NOTICE); break;
        case 2: ofSetLogLevel(OF_LOG_WARNING); break;
        case 3: ofSetLogLevel(OF_LOG_ERROR); break;
        case 4: ofSetLogLevel(OF_LOG_FATAL_ERROR); break;
        case 5: ofSetLogLevel(OF_LOG_SILENT); break;
        default:ofSetLogLevel(OF_LOG_VERBOSE); break;
    }

    ofLogToFile("server_log.txt", false);

    TCP.setup(gdata->sys_data->serverPort);

	currCliPort         = gdata->sys_data->serverPort + 1;
	totThreadedServers  = 0;
    buffLastIndex       = 0;
    buffCurrIndex       = 0;

    ofSetFrameRate(gdata->sys_data->fps);

    mb              = new MainBufferRT();
    mb->sys_data    = gdata->sys_data;
    mb->startBuffer();

    for(int i = 0; i < gdata->sys_data->maxThreadedServers; i++) {
        tservers[i] = NULL;
	}


    generator.sys_data  = gdata->sys_data;
    generator.buffer    = mb;
    generator.srvinst   = this;
    generator.startThread(false, true);
}

//Dejo abierto el puerto PORT_0
//  Si recibo un cliente conectado,
        //Abro un thread con un puerto libre PUERTO_X
        //Le retorno al cliente el puerto (PUERTO_X) asignado.
//--------------------------------------------------------------
void Server::update() {
    if(b_exit_fired) return;
    if(b_exit) {
        if(!b_exit_fired) {
            b_exit_fired = true;
            ofExit();
        }
        return;
    }

    if(b_exit_pressed) {
        b_exit = true;
    }

    for(int i = 0; i < TCP.getLastID(); i++) { // getLastID is UID of all clients
        if( TCP.isClientConnected(i) ) { // check and see if it's still around
            string str = TCP.receive(i);
            if(str.length() > 0 ) {
                std::string delimiter   = "|";
                std::size_t found       = str.find(delimiter);
                std::string cli         = str.substr(0, found);
                std::string port        = str.substr(found+1, str.size());

                ofLogVerbose() << "[Server::update] NUEVO CLIENTE id:" << str;
                ofLogVerbose() << "[Server::update] cli:"   << cli;
                ofLogVerbose() << "[Server::update] port:"  << port;
                ofLogVerbose() << "[Server::update] ip:"    << TCP.getClientIP(i);

                ostringstream convert;
                convert << currCliPort;

                //NOTA: Además del puerto del thread que lo atiende debería pasarle hora actual del servidor (para que sincronice) y fps.

                ThreadServer * ntsrvr                     = new ThreadServer();
                ntsrvr->sys_data  = gdata->sys_data;
                ntsrvr->cliId     = atoi(cli.c_str());
                ntsrvr->ip        = TCP.getClientIP(i);
                ntsrvr->port      = atoi(port.c_str());
                ntsrvr->server    = this;
                //ntsrvr->tid       = totThreadedServers;
                ntsrvr->startThread(false, true);

                /*tservers[totThreadedServers]            = new ThreadServer();
                tservers[totThreadedServers]->sys_data  = gdata->sys_data;
                tservers[totThreadedServers]->cliId     = atoi(cli.c_str());
                tservers[totThreadedServers]->ip        = TCP.getClientIP(i);
                tservers[totThreadedServers]->port      = atoi(port.c_str());
                tservers[totThreadedServers]->startThread(true, false);

                currCliPort         ++;
                totThreadedServers  ++;*/
                //TCP.close();
            }
        }
    }
}

void Server::threadServerReady(void * ntsrv) {
    pthread_mutex_lock(&tsrvMutex);
    tservers[totThreadedServers] = (ThreadServer *) ntsrv;
    currCliPort         ++;
    totThreadedServers  ++;
    pthread_mutex_unlock(&tsrvMutex);
}

void Server::threadServerClosed(void * ntsrv) {
    pthread_mutex_lock(&tsrvMutex);
    bool encontre = false;
    for(int i = 0; i < totThreadedServers; i++) {
        if(tservers[i] == ntsrv) {
            for(int j = i; j < totThreadedServers-1; j++) {
                tservers[j] == tservers[j+1];
            }
            break;
        }
    }
    totThreadedServers  --;
    pthread_mutex_unlock(&tsrvMutex);
}

ofImage     tmpImg;
int ySep    = 170;
int items   = 1;

void Server::computeFrames() {

    int totActivos      = 0;
    int tmpDrawables    = 0;
    //drawables       = 0;

    for(int i = 0; i < totThreadedServers; i++) {
        if(!tservers[i]->closed) {
            ofLogVerbose() << "[Server::computeFrames] Revisando server " << i;
            totActivos ++;
            int currCam = 1;
            pthread_mutex_lock(&tservers[i]->myMutex);

            //ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : por hacer getHeadFrame " << endl;
            //ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : Largo del buffer " << tservers[i]->fb.length();
            std::pair <int, ThreadData *> head      = tservers[i]->fb.getHeadFrame();
            //ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : Largo del buffer despues de tomar " << tservers[i]->fb.length() << endl;
            pthread_mutex_unlock(&tservers[i]->myMutex);

            if((ThreadData *) head.second) {
                ((ThreadData *) head.second)->deletable   = false;
            }

            for(int c = 0; c < head.first; c++) {
                if(gui.isOn()) {
                    if(head.second[c].img.isAllocated()) {
                        //getCamTag(&drawableTags[tmpDrawables], head.second[c].cliId, head.second[c].camId);
                        pthread_mutex_lock(&uiMutex);
                        drawableImages[tmpDrawables].clone(head.second[c].img);
                        //drawableImages[tmpDrawables].draw(300, 200);
                        //drawableImages[tmpDrawables].saveImage("tmp.jpg");
                        pthread_mutex_unlock(&uiMutex);
                        tmpDrawables++;
                    }
                }

                currCam ++;
                mb->addFrame(&head.second[c], head.second[c].camId, head.second[c].cliId);
            }

            //tservers[i]->unlock();
        } else {
            if(tservers[i]->isThreadRunning()) {
                //cout << "STOP THREAD" << endl;
                tservers[i]->stopThread();
            }
        }
    }
    if(drawables < tmpDrawables) {
        drawables = tmpDrawables;
    }
    //ofLogVerbose() << "[Server::computeFrames] - Total de Threads activos: " << totActivos;
}

void Server::getCamTag(std::string * tagDest, int cliId, int camId) {
    *tagDest   = "CLI ID: " + ofToString(cliId) + ", ID CAM: " + ofToString(camId);
}

void Server::drawTag(std::string msj, int x, int y) {
    y += 15;
    ofSetColor(0, 0, 0);
    myfont.drawString(msj, x+1, y+145+1);
    ofSetColor(250, 250, 250);
    myfont.drawString(msj, x, y+145);
    ofSetColor(256, 256, 256);
}

//--------------------------------------------------------------
void Server::draw() {
    if(gui.isOn()) {
        items           = 2;
        int y           = ySep * items + 15;
        int x           = 15;
        gui.draw();

        for(int i=0; i<drawables; i++) {
            //drawTag(drawableTags[i], x, y);
            pthread_mutex_lock(&uiMutex);
            //drawableImages[i].draw(x, y, 200, 150);
            ofImage clon;
            clon.clone(drawableImages[i]);
            clon.draw(x, y, 200, 150);
            //drawableImages[i].saveImage("tmp.jpg");
            pthread_mutex_unlock(&uiMutex);
            items++;
            if(items >= 3) { x+=216; items = 0; }
            y = ySep * items;
        }
    }
}

//--------------------------------------------------------------
void Server::keyPressed(int key) {
    if(key == 'g') {
        if(gui.isOn()) {
            gui.hide();
        } else {
            gui.show();
        }
    } else if(key == 'd') {
        ServerGlobalData::debug = !ServerGlobalData::debug;
    }
}

//--------------------------------------------------------------
void Server::keyReleased(int key) {

}

//--------------------------------------------------------------
void Server::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void Server::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Server::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Server::dragEvent(ofDragInfo dragInfo){

}

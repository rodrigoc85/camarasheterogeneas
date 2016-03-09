#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include "FreeImage.h"

using namespace std;
int i       = 0;
int proc    = 0;
void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;
    currTProcesor   = 0;
    //idMesh          = 0;
    threads     = new MeshThreadedGenerator[sys_data->totalFreeCores];
    collector   = new MeshCollector();

    ofLogVerbose() << "--[MeshGenerator::threadedFunction] " << endl;
    for(int i=0; i<sys_data->totalFreeCores; i++) {
        threads[i].sys_data  = sys_data;
        threads[i].nMTG      = i;
        threads[i].startThread(false, true);
    }

    collector->sys_data  = sys_data;
    collector->threads   = threads;
    collector->startThread(false, true);
    started         = true;

    unsigned long long minMillis = 1000/(sys_data->fps);
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();
        ofLogVerbose() << "[MeshGenerator::threadedFunction] en el while del MeshGenerator " << endl;
        processFrame();

        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            sleep(minMillis - (currMill - baseMill));
        }
    }
    ofLogVerbose() << "[MeshGenerator::threadedFunction] FIN" << endl;
    //ofAddListener(ofEvents().update, this, &MeshGenerator::processFrame);
}

void MeshGenerator::exit() {
    b_exit          = true;
    collector->exit();
    collector->stopThread();
    int i = 0, proc = 0;
    for(i=0; (i<sys_data->totalFreeCores); i++) {
        proc ++;
        proc = proc % sys_data->totalFreeCores;
        threads[proc].exit();
        threads[proc].stopThread();
    }
}

void MeshGenerator::processFrame(ofEventArgs &e) {
}

void MeshGenerator::processFrame() {

    if(!started) return;

    //int i = 0;
    int j = 0;
    while(!b_exit && (j<sys_data->totalFreeCores)) {
        if(threads[i].getState() == GENERATOR_IDLE) {
            ofLogVerbose() << "[MeshGenerator::processFrame] por hacer computeFrames";
            srvinst->computeFrames();
            std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
            //cout << "Generador " << i << " libre" << endl;
            //ofLogVerbose() << "Generador " << i << " libre";
            if((frame.first != NULL) || (frame.second != NULL)) {
                ofLogVerbose() << "Asignado a Generador " << i;
                threads[i].processMesh(frame, nframe);
                nframe++;
                i = (i + 1) % sys_data->totalFreeCores;
                break;
            } else {
                //ofLogVerbose() << "No había nada para asignar al generador " << i;
                //cout << "No había nada para asignar al generador " << i << endl;
            }
        } else {
            //ofLogVerbose() <<  "Generador " << i << " ocupado" << endl;
        }
        j++;
        //i++;
        i = (i + 1) % sys_data->totalFreeCores;
    }

}

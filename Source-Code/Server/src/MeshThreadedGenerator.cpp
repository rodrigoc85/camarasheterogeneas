#include "MeshThreadedGenerator.h"


//typedef void (*f_compartirMalla)(int idMesh, int numberFaces, FaceStruct* faces);
//typedef void (*f_ShareImage)(int* idImage, unsigned char* pixels, int* wPixels, int* hPixels);

void MeshThreadedGenerator::threadedFunction() {
    state   = GENERATOR_IDLE;
    ofLogVerbose() << "--||[MeshThreadedGenerator::threadedFunction]" << endl;
    meshGenerate = (f_meshGenerate)GetProcAddress(generateMeshLibrary, "meshGenerate");

    /*if(!ofFile::doesFileExist("stats_generacion.csv")) {
        ofFile statsFile("stats_generacion.csv",ofFile::Append);
        statsFile << "Generador" << ";"  << "Hora" << ";" << "Duración" << ";" << "Largo de la nube" << endl ;
        statsFile.close();
    }*/

    /*
    while(isThreadRunning()) {
        //ofSleepMillis(1000/sys_data->fps);
        processFrame();
    }
    */
    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();
        processFrame();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            sleep(minMillis - (currMill - baseMill));
        }
    }
    //ofAddListener(ofEvents().update, this, &MeshThreadedGenerator::processFrame);

}

MeshThreadedGenerator::~MeshThreadedGenerator() {
    //delete result;
}

void MeshThreadedGenerator::exit() {
    //ofRemoveListener(ofEvents().update, this, &MeshThreadedGenerator::processFrame);
}

void MeshThreadedGenerator::processFrame(ofEventArgs &e) {
}

void MeshThreadedGenerator::processFrame() {

    if(state == GENERATOR_LOADED) {
        statsBaseMill = ofGetElapsedTimeMillis();
        ThreadData * iter = (ThreadData *) frame.second;
        /*bool descartado = false;
        while(iter != NULL) {
            descartado = descartado || ((iter->img.getWidth() <= 0) || (iter->img.getHeight() <= 0));
            iter = iter->sig;
        }*/
        int nubeLength = -1;
        int idMesh;
        PointsCloud* nbIN = NULL;

        state           = GENERATOR_BUSY;

        if((frame.first != NULL) && (((ThreadData *) frame.first)->nubeLength >100)) { // En first viene un ThreadData con la nube de puntos.
            //float   wait    = 5000;
            //ofSleepMillis(wait);
            //ofLogVerbose() << "MeshThreadedGenerator :: Waited " << wait << endl;
            nubeLength = ((ThreadData *) frame.first)->nubeLength;
            ThreadData* td = ((ThreadData *) frame.first);
            time_t now = time(0);
            tm *ltm = localtime(&now);

            ///GENERAR MALLA
            //f_generarMalla generarMalla = (f_generarMalla)GetProcAddress(generateMeshLibrary, "generarMalla");

            nbIN             = new PointsCloud;
            nbIN->length      = td->nubeLength;
            nbIN->x          = td->xpix;
            nbIN->y          = td->ypix;
            nbIN->z          = td->zpix;

            faces            = new FaceStruct;
            numberFaces      = new int;

            cout << "[Procesando Nube] Generador: " << nMTG <<", Total puntos: " << td->nubeLength << endl;
            try {
                //if(!descartado) {
                    meshGenerate(nbIN, &faces, numberFaces, nframe);
                //}

                ofLogVerbose() << "--||[MeshThreadedGenerator::processFrame]  Termino de generar " << nframe << endl;
            }  catch (exception& e) {
                cout << "--||[MeshThreadedGenerator::processFrame] FALLO EL GENERAR MALLA" << e.what();
                frame.first = NULL;
            }

            ///FIN GENERAR MALLA
        }

        result              = new GeneratedResult();
        //result->descartado  = descartado;
        result->nframe      = nframe;
        result->idMesh      = 10000 + nframe;

        //if(!descartado) {
        result->hasDepth    = (frame.first != NULL);
        result->hasRGB      = (frame.second != NULL);

        result->textures    = (ThreadData *) frame.second;
        result->numberFaces = numberFaces;
        result->faces       = faces;

        if(frame.first != NULL) {
            delete frame.first;
            frame.first = NULL;
            delete nbIN;
        }

        /*Stats*/
        statsCurrMill = ofGetElapsedTimeMillis();
        timeval curTime;
        gettimeofday(&curTime, NULL);
        int milli = curTime.tv_usec / 1000;
        char buffer [80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
        char currentTime[84] = "";
        sprintf(currentTime, "%s:%d", buffer, milli);

        //ofFile statsFile("stats_generacion.csv",ofFile::Append);
        //statsFile << nMTG << ";" << currentTime << ";" << (statsCurrMill - statsBaseMill) << ";" << nubeLength << endl ;
        //statsFile.close();

        /*Fin: Stats*/
        /**/
        lock();
        state   = GENERATOR_COMPLETE;
        unlock();
    }
}

void MeshThreadedGenerator::setState(int state) {
    lock();
    this->state = state;
    unlock();
}

int MeshThreadedGenerator::getState() {
    return this->state;
}

void MeshThreadedGenerator::processMesh(std::pair <ThreadData *, ThreadData *> frame, int nframe) {
    lock();
    ofLogVerbose() << "--||[MeshThreadedGenerator::processMesh] nframe " << nframe << endl;
    this->frame     = frame;
    this->nframe    = nframe;
    state           = GENERATOR_LOADED;
    unlock();
}

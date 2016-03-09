#include "MainBuffer.h"

MainBuffer::MainBuffer() {
}

void MainBuffer::startBuffer() {
    fm = new FieldManager();
    fm->sys_data = sys_data;
    fm->startBuffer();
}

MainBuffer::~MainBuffer() {
    ofLogVerbose() << "[MainBuffer::~MainBuffer]";
    delete fm;
}

/**
* Agrega un Frame al buffer principal.
* Un frame está compuesto por información de varias cámaras.
* Esta función descompone esa información y agrupa la info de c/cámara según un redondeo
* del tiempo en el que haya sido usado.
* En caso que el buffer esté lleno, inserta igualmente y desplaza los indicadores.
* Nunca deja de almacenar, siempre guarda y descarta lo más viejo.
* Recibe por parámetro el frame y la totalidad de Cámaras en ese momento.
* Asumo que en frame viene una única cámara.
*/
void MainBuffer::addFrame( ThreadData * frame , int cam, int cli) {
    long int camId  = getCamId(cam, cli);
    ofLogVerbose() << "[MainBuffer::addFrame] camId " << camId;

    fm->calcSyncTime(frame);
    ofLogVerbose() << "[MainBuffer::addFrame] time " << frame->key.first << "-" << frame->key.second;

    Field * fi      = fm->getFieldForTime(frame->key);

    if(!fm->hasFrameFromCam(fi, camId)) {
        fm->addFrameFromCam(frame, camId, fi);
        return;
    } else {
        ThreadData * prevFrame  = frame;
        //frame->removeRecursively = false;
        frame->releaseResources();
        //delete frame;
    }

    ofLogVerbose() << endl;
}

/**
* Private
*/
long int MainBuffer::getCamId(int cam, int cli) {
    return ((cli + 1) * 10000 + cam);
}

/**
* Retorna el siguiente frame a ser procesado.
* Retorna un tipo pair conteniendo:
* pair.first: Nube de punto concatenada.
* pair.second: Array de ThreadData conteniendo las texturas de todas las cámaras
* de todos los clientes para ese frame (aún no implementado).
*/
std::pair <ThreadData *, ThreadData *> MainBuffer::getNextFrame() {
    Field * fi = fm->getNextFilledField();
    std::pair <ThreadData *, ThreadData *> ret;
    ret.first   = NULL;
    ret.second  = NULL;

    if(fi == NULL) {
        ofLogVerbose() << "[MainBuffer::getNextFrame] No se encontró un frame sin procesar para devolver. Retorno NULL.";
        return ret;
    }

    for (map< long int, ThreadData * >::iterator it = fi->frame_map.begin(); it != fi->frame_map.end(); ++it) {
        //ofLogVerbose() << "[MainBuffer::getNextFrame] for.";
        if( (((ThreadData *) it->second)->state == 2 ) || (((ThreadData *) it->second)->state == 3 )) {
            ((ThreadData *) it->second)->cameraType = 2;
            if(((ThreadData *) it->second)->nubeLength > 0) {
                if(ret.first == NULL) {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] first==NULL.";
                    ret.first = (ThreadData *) it->second;
                } else {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] Mergeando puntos.";
                    ret.first->mergePointClouds((ThreadData *) it->second);
                }
            }
        }

        if( (((ThreadData *) it->second)->state == 1) || (((ThreadData *) it->second)->state == 3) ) {
            ThreadData * td = new ThreadData();
            if(((ThreadData *) it->second)->state == 3) {
                td->cameraType  = 2;
            } else {
                td->cameraType  = 1;
            }

            ThreadData * re = ((ThreadData *) it->second);

            td->cliId   = re->cliId;
            td->camId   = re->camId;

            td->img.setFromPixels(((ThreadData *) it->second)->img.getPixelsRef());
            td->row1     = re->row1;
            td->row2     = re->row2;
            td->row3     = re->row3;
            td->row4     = re->row4;

            if(ret.second == NULL) {
                ofLogVerbose() << "[MainBuffer::getNextFrame] primera imagen RGB.";
                td->sig     = NULL;
            } else {
                ofLogVerbose() << "[MainBuffer::getNextFrame] agrego otra imagen imagen RGB.";
                td->sig     = ret.second;
            }
            ret.second  = td;
        }
    }
    //fm->removeField(fi);
    ofLogVerbose() << "[MainBuffer::getNextFrame] Luego de mergear";
    return ret;
}

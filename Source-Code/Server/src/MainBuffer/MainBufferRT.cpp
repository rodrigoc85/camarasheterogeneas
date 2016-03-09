#include "MainBufferRT.h"

MainBufferRT::MainBufferRT() {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            tdatabusy[i][j] = false;
        }
    }
    iniData = NULL;
    pthread_mutex_init(&myMutex, NULL);
}

void MainBufferRT::startBuffer() {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            tdata[i][j] = NULL;
        }
    }
}

MainBufferRT::~MainBufferRT() {
    ofLogVerbose() << "[MainBufferRT::~MainBufferRT]";

    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            delete tdata[i][j];
            tdata[i][j] = NULL;
        }
    }

    pthread_mutex_destroy(&myMutex);

}

int MainBufferRT::buffLength() {
    ThreadData * iter = iniData;
    int length = 0;
    while(iter != NULL) {
        length++;
        iter = iter->sig;
    }
    return length;
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
void MainBufferRT::addFrame( ThreadData * frame , int cam, int cli) {
    //ofLogVerbose() << "[MainBufferRT::addFrame] ";
    pthread_mutex_lock(&myMutex);
    frame->used = false;
    if(iniData == NULL) {
        iniData = frame;
        iniData->sig = NULL;
    } else {
        ThreadData * iter = iniData;
        ThreadData * prev = iter;
        while((iter!=NULL) && !((iter->cliId == cli) &&(iter->camId == cam ))) {
            prev = iter;
            iter = iter->sig;
        }

        if(iter == NULL) {
            prev->sig = frame;
        } else {
            if(prev == iter) {
                iniData = frame;
                iniData->sig = iter->sig;
                ThreadData * tmp = iter;
                tmp->releaseResources();
            } else {
                prev->sig = frame;
                frame->sig = iter->sig;
                ThreadData * tmp = iter;
                iter = prev;
                tmp->releaseResources();
            }
        }
    }

    pthread_mutex_unlock(&myMutex);
}

bool MainBufferRT::hasNewData(ThreadData * data) {
    ThreadData * it = data;
    while(it!=NULL) {
        if(!it->used) {
            return true;
        }
        it = it->sig;
    }
    return false;
}

/**
* Retorna el siguiente frame a ser procesado.
* Retorna un tipo pair conteniendo:
* pair.first: Nube de punto concatenada.
* pair.second: Array de ThreadData conteniendo las texturas de todas las cámaras
* de todos los clientes para ese frame (aún no implementado).
*/
std::pair <ThreadData *, ThreadData *> MainBufferRT::getNextFrame() {
    std::pair <ThreadData *, ThreadData *> ret;
    ret.first   = NULL;
    ret.second  = NULL;

    if(!hasNewData(iniData)) {
        //ofLogVerbose() << "[MainBufferRT::getNextFrame()] hasNewData = false";
        return ret;
    }

    ThreadData * it = iniData;
    while(it!=NULL) {
        pthread_mutex_lock(&myMutex);
        it->used = true;
        ThreadData * curr = ThreadData::Clone(it);
        pthread_mutex_unlock(&myMutex);
        if( (curr != NULL) && ((curr->state == 2 ) || (curr->state == 3 ))) {
            if(curr->nubeLength > 0) {
                if(ret.first == NULL) {
                    ret.first = ThreadData::Clone(curr);
                } else {
                    ret.first->mergePointClouds(curr);
                }
            }
        }

        if( (curr != NULL) && ((curr->state == 1 ) || (curr->state == 3 )) ) {
            ThreadData * td = ThreadData::Clone(curr);
            if(ret.second == NULL) {
                //ofLogVerbose() << "[MainBufferRT::getNextFrame] primera imagen RGB.";
                td->sig     = NULL;
            } else {
                //ofLogVerbose() << "[MainBufferRT::getNextFrame] agrego otra imagen imagen RGB. " << td;
                td->sig     = ret.second;
            }
            ret.second  = td;
        }
        delete curr;
        it = it->sig;
    }

    ThreadData * iter = ret.second;
    bool descartado = false;
    while(iter != NULL) {
        descartado = descartado || ((iter->img.getWidth() <= 0) || (iter->img.getHeight() <= 0));
        iter = iter->sig;
    }

    if((ret.first == NULL) || (((ThreadData *) ret.first)->nubeLength <100)) {
        descartado = true;
    }

    if(descartado) {
        ofLogVerbose() << "[MainBufferRT::getNextFrame()] Descartado";
        ret.first   = NULL;
        ret.second  = NULL;
    }

    return ret;
}

/*Field * fi = fm->getNextFilledField();
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
    return ret;*/

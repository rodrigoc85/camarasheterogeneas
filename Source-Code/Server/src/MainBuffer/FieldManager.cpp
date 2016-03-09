#include "FieldManager.h"
#include <sys/time.h>

FieldManager::FieldManager() {
}

void FieldManager::startBuffer() {
    gettimeofday(&curIndex, NULL);
    gettimeofday(&maxIndex, NULL);
    minUnit   = floor((1000/sys_data->syncFactorValue));
    topeUnit  = minUnit * sys_data->syncFactorValue;
    curIndex.tv_usec = 0;
}

FieldManager::~FieldManager() {
    ofLogVerbose() << "[FieldManager::~FieldManager]";
    for (std::map< std::pair <time_t, int>, Field * >::iterator it = field_map.begin(); it != field_map.end(); ++it) {
        delete ((Field *) it->second);
    }
}

/**
* Dado el timestamp del frame, retorna el tiempo correspondiente al field más cercano.
*/
void FieldManager::calcSyncTime(ThreadData * frame) {
    ofLogVerbose() << "[FieldManager::calcSyncTime] " << frame->curTime.tv_usec << " " << frame->curTime.tv_sec;
    float minUnit   = (1000/sys_data->syncFactorValue);
    int milli       = frame->curTime.tv_usec / 1000;
    int step        = (milli / minUnit);

    frame->key.first    = frame->curTime.tv_sec;
    frame->key.second   = step;
    //printf("current time: %d %d \n", frame->key.first, frame->key.second);
}

/**
* Dado un tiempo timestamp, hace la conversión según SYNC_FACTOR_VALUE y retorna
* el field para ese tiempo.
*/
Field * FieldManager::getFieldForTime(std::pair <time_t, int> timestamp) {
    std::pair <time_t, int> search = timestamp;
    ofLogVerbose() << "[FieldManager::getFieldForTime] - Buscando " << timestamp.first << "-" << timestamp.second;
    if(field_map.count(search) == 1) {
        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "ENCONTRO";
        return field_map[search];
    } else {
        Field * fi  = new Field();
        fi->id = field_map.size();


        field_map.insert ( std::pair< std::pair <time_t, int>, Field * > (search, fi) );

        //actualizo el maximo valor guardado
        timeval tmod;
        tmod.tv_sec  = search.first;
        tmod.tv_usec = search.second * minUnit;
        if(timevalMinorEqualThan(maxIndex, tmod )) {
            maxIndex = tmod;
        }

        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "NO ENCONTRO";
        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "NUEVO FIELD " << fi->id;

        return fi;
    }
    return NULL;
}

/**
* Chequea si este Field tiene ya un frame proveniente de esa camara.
* En caso de que sea true, entonces no se debería poner otro frame
* proveniente de esa cámara.
*/
bool FieldManager::hasFrameFromCam(Field * fi, long int camId) {
    //return false;
    return fi->hasCam(camId);
}

/**
* Agrega a Field el frame para esa cámara.
*/
void FieldManager::addFrameFromCam(ThreadData * frame, long int camId, Field * fi) {
    ofLogVerbose() << "[FieldManager::addFrameFromCam] addFrameFromCam " << camId << endl;
    fi->addFrame(frame, camId);
}

/**
* Retorna el Frame completo más antiguo registrado.
*/
Field * FieldManager::getOlderCompleteFrame() {
    return NULL;
}

/**
* Remueve de memoria el Field pasado por parámetro.
*/
void FieldManager::removeField(Field * fi) {

}

/**
* Recorre según el indice de tiempo y retorna el siguiente
* objeto sin procesar.
*/
Field * FieldManager::getNextFilledField() {
    bool encontro = false;
    int step;
    std::pair <time_t, int> search;
    Field * fi = NULL;

    if(sys_data->processMostRecent == 1) {
        curIndex    = maxIndex;
        removeAllMinorThan(curIndex);
    }
    while((!encontro) && timevalMinorEqualThan(curIndex, maxIndex )) {
        step    = (curIndex.tv_usec / minUnit);

        //Buscar
        search.first    = curIndex.tv_sec;
        search.second   = step;

        if(field_map.count(search) == 1) {
            fi          = field_map[search]; //Faltaría después evaluar si borrarlo o no.
            if(fi->enabled) {
                encontro      = true;
                fi->enabled   = false;
                ofLogVerbose() << "[FieldManager::getNextFilledField] - ENCONTRO curIndex.tv_sec: " << curIndex.tv_sec << " - curIndex.tv_usec: " << curIndex.tv_usec;
            }
        }
        if(step >= (topeUnit-1)) {
            curIndex.tv_sec  += 1;
            curIndex.tv_usec  = 0;
        } else {
            curIndex.tv_usec += minUnit;
        }
    }
    if(!encontro) {
        ofLogVerbose() << "[FieldManager::getNextFilledField] - NO ENCONTRO nada para procesar";
    }
    return fi;
}

void FieldManager::removeAllMinorThan(timeval curval) {
    return;
    for (std::map< std::pair <time_t, int>, Field * >::iterator it = field_map.begin(); it != field_map.end(); ++it) {
        std::pair <time_t, int> key = it->first;
        timeval itVal;
        itVal.tv_sec    = key.first;
        itVal.tv_usec   = key.second;
        if(timevalMinorThan(itVal, curval)) {
            ((Field *) it->second)->releaseResources();
            //delete ((Field *) it->second);
        }
    }
}

bool FieldManager::timevalMinorEqualThan(timeval curIndex, timeval maxIndex ) {
    if(curIndex.tv_sec < maxIndex.tv_sec) return true;
    if(curIndex.tv_sec == maxIndex.tv_sec) {
        if(curIndex.tv_usec <= maxIndex.tv_usec) return true;
    }
    return false;
}

bool FieldManager::timevalMinorThan(timeval curIndex, timeval maxIndex ) {
    if(curIndex.tv_sec < maxIndex.tv_sec) return true;
    if(curIndex.tv_sec == maxIndex.tv_sec) {
        if(curIndex.tv_usec < maxIndex.tv_usec) return true;
    }
    return false;
}

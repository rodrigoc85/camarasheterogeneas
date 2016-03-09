#pragma once

#include "ofMain.h"
#include "../Constants.h"
#include "../ThreadData.h"
#include "FieldManager.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <pthread.h>

class MainBufferRT {
	public:
        MainBufferRT();
		~MainBufferRT();
        ThreadData * tdata [50][50];
        ThreadData * iniData;
        bool tdatabusy [50][50];
        bool tdatabusyvar;
        void startBuffer();
        pthread_mutex_t myMutex;
        t_data      * sys_data;

		/**
		* Agrega un Frame al buffer principal.
		* Un frame está compuesto por información de una sola cámara de un cliente dado.
		* Esta función agrupa la info de c/cámara según un redondeo
		* del tiempo en el que haya sido usado.
		* Recibe por parámetro el frame, el id de la camara dentro del cliente y el id del cliente.
		*/
		void addFrame( ThreadData * frame , int cam, int cli);

        /**
		* Retorna el siguiente frame a ser procesado.
		* Retorna un tipo pair conteniendo:
		* pair.first: Nube de punto concatenada.
		* pair.second: Array de ThreadData conteniendo las texturas de todas las cámaras
		* de todos los clientes para ese frame (aún no implementado).
		*/
		std::pair <ThreadData *, ThreadData *> getNextFrame();
        bool hasNewData(ThreadData * data);
		int buffLength();

};

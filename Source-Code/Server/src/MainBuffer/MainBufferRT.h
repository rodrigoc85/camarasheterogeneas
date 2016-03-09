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
		* Un frame est� compuesto por informaci�n de una sola c�mara de un cliente dado.
		* Esta funci�n agrupa la info de c/c�mara seg�n un redondeo
		* del tiempo en el que haya sido usado.
		* Recibe por par�metro el frame, el id de la camara dentro del cliente y el id del cliente.
		*/
		void addFrame( ThreadData * frame , int cam, int cli);

        /**
		* Retorna el siguiente frame a ser procesado.
		* Retorna un tipo pair conteniendo:
		* pair.first: Nube de punto concatenada.
		* pair.second: Array de ThreadData conteniendo las texturas de todas las c�maras
		* de todos los clientes para ese frame (a�n no implementado).
		*/
		std::pair <ThreadData *, ThreadData *> getNextFrame();
        bool hasNewData(ThreadData * data);
		int buffLength();

};

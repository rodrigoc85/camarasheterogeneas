#pragma once

#include "ofMain.h"
#include "../Constants.h"
#include "../ThreadData.h"
#include "Field.h"

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include "../ServerGlobalData.h"

class FieldManager {
	public:
        FieldManager();
		~FieldManager();

        //map< std::pair <time_t, int> , Field * > field_map2;
        map< std::pair <time_t, int> , Field * > field_map;

        timeval curIndex;
        timeval maxIndex;
        float minUnit;
        float topeUnit;

        void startBuffer();
        t_data      * sys_data;

        /**
        * Dado el timestamp del frame, retorna el tiempo correspondiente al field m�s cercano.
        */
        void calcSyncTime(ThreadData *);

        /**
        * Dado un tiempo timestamp, hace la conversi�n seg�n SYNC_FACTOR_VALUE y retorna
        * el field para ese tiempo.
        */
		Field * getFieldForTime(std::pair <time_t, int> timestamp);

		/**
		* Chequea si este Field tiene ya un frame proveniente de esa camara.
		* En caso de que sea true, entonces no se deber�a poner otro frame
        * proveniente de esa c�mara.
		*/
		bool hasFrameFromCam(Field *, long int camId);

		/**
		* Agrega a Field el frame para esa c�mara.
		*/
		void addFrameFromCam(ThreadData *, long int camId, Field *);

		/**
		* Retorna el Frame completo m�s antiguo registrado.
		*/
		Field * getOlderCompleteFrame();

        /**
		* Remueve de memoria el Field pasado por par�metro.
		*/
        void removeField(Field *);

        /**
        * Recorre seg�n el indice de tiempo y retorna el siguiente
        * objeto sin procesar.
        */
        Field * getNextFilledField();

        bool timevalMinorEqualThan(timeval curIndex, timeval maxIndex );

        bool timevalMinorThan(timeval curIndex, timeval maxIndex );

        void removeAllMinorThan(timeval curval);
};

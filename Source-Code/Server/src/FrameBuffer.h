#pragma once

#include "ofMain.h"
#include "ThreadData.h"

#define MAX_BUFF_SIZE 10

class FrameBuffer {
	public:
        ThreadData * buffer[MAX_BUFF_SIZE];
        int          totalCameras[MAX_BUFF_SIZE];

        int tope;
        int base;

		/**
		* Agrega un Frame al tope del buffer.
		* En caso que el buffer est� lleno, inserta igualmente y desplaza los indicadores.
		* Nunca deja de almacenar, siempre guarda y descarta lo m�s viejo.
		*/
		void addFrame( ThreadData * frame , int totalCams);

		/**
		* Retorna y quita el frame m�s viejo.
		* Si no hay m�s retorna null.
		*/
		std::pair <int, ThreadData *> getHeadFrame();

		int length();

		FrameBuffer();
		~FrameBuffer();
};

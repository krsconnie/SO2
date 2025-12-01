#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/*
Struct Marco para la memoria física
	npv: nº página virtual almacenada en el marco, si esta vacía guarda -1.
	refBit: Bit de referencia de acceso.
*/
typedef struct Marco {
	
	int npv;
	int refBit;

} Marco;

/*
Struct Tabla de paginación (TdP) 
	valido: indica si la página esta cargada en memoria.
	nMarco: número de marco donde se encuentra la página,si y solo si valido == true. 
*/
typedef struct TdP {

	bool valido;
	// bool modificado; // puede que no sea necesario mirando los obj de la tarea
	int nMarco;	

} TdP; 


/*
Struct Sim(ulador)
	Contiene los elementos principales para la simulación,
	lo explico línea a línea porque sale más corto
*/

typedef struct Sim {
	
	Marco *memoriaFisica; 		// Memoria ram simulada
	TdP *tablaPag;				// Tabla de páginas simulada

	int nMarcos; 				// cantidad de marcos.
	int tam; 				// tamaño (bytes)
	int b; 						// nº de bits para el offset
	unsigned int mask; 			// mascara de bits 
	int relojAux;				// hace referencia al marco candidato a reemplazar en el "algoritmo del Reloj"
	bool verbose; 				// Flag para imprimir detalles paso a paso. 1 = activado, 0 = desactivado

	// Lo que se imprime,si verbose == false solo sale esto.
	long long referencias;		
	long long fallosPagina;
	double tasaFallo;

} Sim;


// Funciones públicas que se explican mejor en "simulador.c"

void init_simulacion(int nMarcos, int tam, bool verbose);

void process_trace_file(const char *filename); 

void imprimir();

void limpiar();

#endif 
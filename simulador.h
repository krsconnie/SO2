#ifndef SIMULADOR.H
#define SIMULADOR.H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/*
Marco para la memoria física
npv: nº página virtual
refBit: Bit de referencia
*/
typedef struct {
	
	int npv;
	int refBit;

} Marco;


/*
Tabla de paginación (TdP)

*/
typedef struct {

	bool valido;
	bool modificado;
	int nMarco;	

} TdP; 


typedef struct {
	
	Marco *memoriaFisica;
	TdP *tablaPag;
	int nMarcos;
	int tamPag;
	int b;
	unsigned int mask;
	int relojAux;
	int verbose; // el enunciado dice que es opcional, si me funciona lo dejo :D

	long long referencias;
	long long fallosPagina;
	double tasaFallo;

} Sim;

void init_simulacion(int nMarcos, int tamPag, int verbose);
void process_trace_file(const char *filename); 
void print_report();
void cleanup_simulation();

#endif SIMULADOR.H
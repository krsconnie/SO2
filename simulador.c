#include "simulador.h"
#include <math.h>

static Sim sim;

static void traducir_direccion(unsigned int dv);
static int resolver_fallo(int npv);
static int obtener_victima();

/*

*/
void init_simulacion(int nMarcos, int tamPag, bool verbose) {
    sim.nMarcos = nMarcos;
    sim.tamPag = tamPag; // def de la tarea PAGE_SIZE = 2^(b) bytes
    sim.verbose = verbose;
    sim.relojAux = 0;    
    sim.referencias = 0;
    sim.fallosPagina = 0;

    sim.b = (int)log2(tamPag); // usamos log2 para extraer b 
    sim.mask = tamPag - 1; // def de la tarea:  MASK = 2b − 1 


    sim.memoriaFisica = (Marco *)malloc(sizeof(Marco) * nMarcos);
    sim.tablaPag = (TdP *)malloc(sizeof(TdP) * 1048576); 

   	
   	// Inicializa los marcos de memoria física como desocupados
    for (int i = 0; i < nMarcos; i++) {
        sim.memoriaFisica[i].npv = -1;
        sim.memoriaFisica[i].refBit = 0;
    }

    // Inicializa tabla de páginas, todas inválidas
    for (int i = 0; i < 1048576; i++) {
        sim.tablaPag[i].valido = false;
       // sim.tablaPag[i].modificado = false;
        sim.tablaPag[i].nMarco = -1;
    }
}



/*
Procesa el archivo de trazas línea por línea,
convierte las direcciones hexadecimales del archivo.
*/
void process_trace_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error abriendo archivo");
        exit(1);
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), fp)) {
        unsigned int dv = (unsigned int)strtoul(linea, NULL, 0);
        traducir_direccion(dv);
    }
    fclose(fp);
}


/*
Imprime los resultados.
*/
void imprimir() {

	if (sim.referencias > 0){
		
		sim.tasaFallo = (double)sim.fallosPagina / sim.referencias;
	
	} else {

		sim.tasaFallo = 0.0;
	}

    printf("Referencias: %lld\n", sim.referencias);
    printf("Fallos: %lld\n", sim.fallosPagina);
    printf("Tasa de fallos: %.4f\n", sim.tasaFallo);
}

/*

*/
void limpiar() {
    if (sim.memoriaFisica) free(sim.memoriaFisica);
    if (sim.tablaPag) free(sim.tablaPag);
}


/* 

Algoritmo de Reemplazo para el reloj
Busca una víctima usando 'relojAux' y el 'refBit'

*/

static int obtener_victima() {
    while (true) {
        
        Marco *marcoActual = &sim.memoriaFisica[sim.relojAux];

       
        if (marcoActual->refBit == 0) {
            int victima = sim.relojAux;
            sim.relojAux = (sim.relojAux + 1) % sim.nMarcos;
            return victima;
        }
        
        marcoActual->refBit = 0;
        sim.relojAux = (sim.relojAux + 1) % sim.nMarcos;
    }
}

/*

*/
static int resolver_fallo(int npv) {
    sim.fallosPagina++; 
    int marcoDestino = -1;

    for (int i = 0; i < sim.nMarcos; i++) {
        if (sim.memoriaFisica[i].npv == -1) {
            marcoDestino = i;
            break;
        }
    }

    
    if (marcoDestino == -1) {
        marcoDestino = obtener_victima();
        int npv_antiguo = sim.memoriaFisica[marcoDestino].npv;

        if (npv_antiguo != -1) {
            sim.tablaPag[npv_antiguo].valido = false;
            sim.tablaPag[npv_antiguo].nMarco = -1;
        }
    }

   
    sim.tablaPag[npv].valido = true;
    sim.tablaPag[npv].nMarco = marcoDestino;
    // sim.tablaPag[npv].modificado = false; 
    
    sim.memoriaFisica[marcoDestino].npv = npv;
    sim.memoriaFisica[marcoDestino].refBit = 1; 

    return marcoDestino;
}

/* 

*/
static void traducir_direccion(unsigned int dv) {
    
    sim.referencias++; 

   	// Descomposición de la dirección virtual
    unsigned int offset = dv & sim.mask;
    unsigned int npv = dv >> sim.b;

  
    if (npv >= 1048576) return; 

    int marcoUsado = -1;
    const char *estado = "HIT";


    if (sim.tablaPag[npv].valido) {
 
        marcoUsado = sim.tablaPag[npv].nMarco;
        sim.memoriaFisica[marcoUsado].refBit = 1; 
        estado = "HIT";

    } else {
     
        marcoUsado = resolver_fallo(npv); 
        estado = "FALLO";
    }

    // Cálculo de la dirección física 
    unsigned int df = (marcoUsado << sim.b) | offset;

    if (sim.verbose) {
        printf("DV: 0x%X, npv: 0x%X, offset: 0x%X, %s, marco: %d, DF: 0x%X\n",
               dv, npv, offset, estado, marcoUsado, df);
    }
}
#!/bin/bash

# Definir colores para la salida
GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m' # No Color
RESULTADOS_GLOBAL="resultados_generales.csv"

echo -e "${CYAN}Compilando... ${NC}"
gcc main.c simulador.c -o simulador -lm

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: La compilación falló.${NC}"
    exit 1
fi


# Inicializar el archivo CSV global
echo "Archivo,Marcos,TamPag,Referencias,Fallos,TasaFallo" > $RESULTADOS_GLOBAL
echo -e "${GREEN}Archivo resumen '$RESULTADOS_GLOBAL' creado.${NC}\n"


ejecutar_prueba() {
    local MARCOS=$1
    local TAMPAG=$2
    local TRACE_FILE=$3
    
    # --verbose usado
    local DETALLE_CSV="detalle_${TRACE_FILE%.*}_${MARCOS}marcos.csv"
    
    echo -n "  Ejecutando $TRACE_FILE con nº de Marcos = $MARCOS"

    # --verbose 
    ./simulador $MARCOS $TAMPAG --verbose $TRACE_FILE > temp_salida.txt

    # resumen 
    local REF=$(grep "Referencias:" temp_salida.txt | awk '{print $2}')
    local FAIL=$(grep "Fallos:" temp_salida.txt | awk '{print $2}')
    local RATE=$(grep "Tasa de fallos:" temp_salida.txt | awk '{print $4}')

    # archivo del resumen
    echo "$TRACE_FILE,$MARCOS,$TAMPAG,$REF,$FAIL,$RATE" >> $RESULTADOS_GLOBAL

    # archivo del detalle (cuando --verbose)
    echo "DV,NPV,Offset,Estado,Marco,DF" > $DETALLE_CSV
    
    grep "^DV:" temp_salida.txt | \
    sed -E 's/DV: //g; s/ npv: //g; s/ offset: //g; s/ marco: //g; s/ DF: //g; s/, /,/g' \
    >> $DETALLE_CSV

    echo -e " --- ${GREEN}OK${NC} Detalle guardado en: $DETALLE_CSV"
}



echo -e "${CYAN}Verificación de referencias ${NC}"
cat > mini_trace.txt <<EOF
0x00000001
0x00000002
0x00000003
0x00000001
0x00000004
0x00000001
0x00000005
0x00000002
0x00000006
0x00000001
EOF
# Ejecutamos usando la función para que también genere su CSV
ejecutar_prueba 3 1 "mini_trace.txt"


# Trace1
echo -e "\n${CYAN}Ejecutando simulador con trace1 ${NC}"
TRACE_FILE="trace1.txt"
TAM_PAG=8

if [ -f "$TRACE_FILE" ]; then
    for MARCOS in 8 16 32; do
        ejecutar_prueba $MARCOS $TAM_PAG $TRACE_FILE
    done
else
    echo -e "${RED} Error: No se encuentra $TRACE_FILE.${NC}"
fi


# Trace2
echo -e "\n${CYAN} Ejecutando simulador con trace2 ${NC}"
TRACE_FILE="trace2.txt"
TAM_PAG=4096

if [ -f "$TRACE_FILE" ]; then
    for MARCOS in 8 16 32; do
        ejecutar_prueba $MARCOS $TAM_PAG $TRACE_FILE
    done
else
    echo -e "${RED}Error: No se encuentra $TRACE_FILE.${NC}"
fi

# Limpieza
rm  temp_salida.txt
echo -e "\n${GREEN} Listo ${NC}"
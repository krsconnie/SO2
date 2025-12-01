#!/bin/bash

# Definir colores para la salida
GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m' # No Color

RESULTADOS_DIR="resultados"
RESULTADOS_GLOBAL="resultados_generales.csv"

# Compilación
echo -e "${CYAN}Compilando... ${NC}"
make simulador

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: La compilación falló.${NC}"
    exit 1
fi

# Crear directorio de resultados si no existe
mkdir -p "$RESULTADOS_DIR"

# Inicializar el archivo CSV global
echo "Archivo,Marcos,TamPag,Referencias,Fallos,TasaFallo" > "$RESULTADOS_GLOBAL"
echo -e "${GREEN}Archivo resumen '$RESULTADOS_GLOBAL' creado.${NC}\n"

# Función Principal
ejecutar_prueba() {
    local MARCOS=$1
    local TAMPAG=$2
    local TRACE_FILE=$3
    
    # Definir nombre del archivo detalle
    local TRACE_NAME=$(basename "${TRACE_FILE%.*}")
    local DETALLE_CSV="${RESULTADOS_DIR}/detalle_${TRACE_NAME}_${MARCOS}marcos.csv"
    
    echo -n "  Ejecutando $TRACE_FILE (Marcos: $MARCOS, Pág: $TAMPAG)... "

    # Ejecutar simulador y guardar salida temporal
    ./simulador "$MARCOS" "$TAMPAG" --verbose "$TRACE_FILE" > temp_salida.txt

    # Verificar si el simulador retorno bien
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error al ejecutar simulador${NC}"
        return
    fi

    # Extraer datos para el resumen
    local REF=$(grep "Referencias:" temp_salida.txt | awk '{print $2}')
    local FAIL=$(grep "Fallos:" temp_salida.txt | awk '{print $2}')
    local RATE=$(grep "Tasa de fallos:" temp_salida.txt | awk '{print $4}')

    # Guardar en resumen global
    echo "$TRACE_FILE,$MARCOS,$TAMPAG,$REF,$FAIL,$RATE" >> "$RESULTADOS_GLOBAL"

    # Generar CSV de detalle
    echo "DV,NPV,Offset,Estado,Marco,DF" > "$DETALLE_CSV"
    
    # Procesar la salida verbose para convertirla a CSV
    # Se busca líneas que empiecen con DV:
    grep "^DV:" temp_salida.txt | \
    sed -E 's/DV: //g; s/ npv: //g; s/ offset: //g; s/ estado: //g; s/ marco: //g; s/ DF: //g; s/, /,/g' \
    >> "$DETALLE_CSV"

    echo -e "--- ${GREEN}OK${NC} -> $DETALLE_CSV"
}

# --- INICIO DE PRUEBAS ---

echo -e "${CYAN}Verificación de referencias (Mini Trace)${NC}"
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

ejecutar_prueba 3 1 "mini_trace.txt"

# Trace 1
echo -e "\n${CYAN}Ejecutando simulador con trace1${NC}"
TRACE_FILE="traces/trace1.txt"
TAM_PAG=8

if [ -f "$TRACE_FILE" ]; then
    for MARCOS in 8 16 32; do
        ejecutar_prueba $MARCOS $TAM_PAG "$TRACE_FILE"
    done
else
    echo -e "${RED} Error: No se encuentra $TRACE_FILE.${NC}"
fi

# Trace 2
echo -e "\n${CYAN}Ejecutando simulador con trace2${NC}"
TRACE_FILE="traces/trace2.txt"
TAM_PAG=4096

if [ -f "$TRACE_FILE" ]; then
    for MARCOS in 8 16 32; do
        ejecutar_prueba $MARCOS $TAM_PAG "$TRACE_FILE"
    done
else
    echo -e "${RED}Error: No se encuentra $TRACE_FILE.${NC}"
fi

# Limpieza
rm temp_salida.txt
rm mini_trace.txt
echo -e "\n${GREEN}Listo. Resultados en carpeta '$RESULTADOS_DIR'${NC}"

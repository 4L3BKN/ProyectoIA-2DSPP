#!/bin/bash

# ==============================
# CONFIGURACIÓN DEL EXPERIMENTO
# ==============================

# Parámetros a testear
TEMPS=(100 250 500 )
ALPHAS=(0.85 0.90 0.95)
ITERACIONES=(100 1000 2000 5000)
REPETICIONES=3     # cuántas veces correr cada experimento

# Carpeta donde están los input TXT
INSTANCIAS_DIR="./2sp"

# ==============================
#     EJECUCIÓN EXPERIMENTAL
# ==============================
for instancia in "$INSTANCIAS_DIR"/*.TXT; do
    
    BASE=$(basename "$instancia" .TXT)

    if [ "$BASE" = "readme" ]; then
        continue
    fi

    echo "Probando instancia: $BASE"

    for T in "${TEMPS[@]}"; do
        for A in "${ALPHAS[@]}"; do
            for IT in "${ITERACIONES[@]}"; do
        
                for ((i=1; i<=REPETICIONES; i++)); do
                echo "Ejecutando ($i/$REPETICIONES) | T=$T | a=$A | iter=$IT"

                ./2dspp "$BASE" $T $A $IT 0 \
                
                done

            done
        done
    done
done

echo "Experimentos FINALIZADOS."

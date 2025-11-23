import pandas as pd
import matplotlib.pyplot as plt

# ==============================
#   LECTURA DEL CSV
# ==============================
try:
    df = pd.read_csv("resultados_experimentos.csv")
except FileNotFoundError:
    print("Archivo 'resultados_experimentos.csv' no encontrado.")
    exit()

# Normalizar nombres de columnas para evitar problemas con espacios o caracteres especiales
df.columns = (
    df.columns.str.strip()
              .str.lower()
              .str.replace("°", "o")
              .str.replace(" ", "_")
)

print("\n=== Primeras filas ===")
print(df.head())
print("\n=== Columnas ===")
print(df.columns)
print("\n=== Cantidad evaluaciones ===")
print(len(df))
# ==============================
#   MEJORES RESULTADOS POR INSTANCIA
# ==============================
mejores = df.loc[df.groupby("nombreinstancia")["alturafranja"].idxmin()]
mejores = mejores.reset_index(drop=True)

print("\n=== Mejores parámetros por instancia ===")
print(mejores[["nombreinstancia", "toinicial", "alpha", "maxiteraciones", "alturafranja", "areanoutilizada", "tiempoejecucion"]])

# ==============================
#   MEJORES RESULTADOS POR INSTANCIA CON: ALPHA = 0.95, TEMPERATURAINICIAL = 100, MAXITERACIONES = 5000
# ==============================

# Filtrar las mejores soluciones que cumplen con los parámetros deseados
condicion = (
    (mejores["toinicial"] == 100) &
    (mejores["alpha"] == 0.95) &
    (mejores["maxiteraciones"] == 5000)
)

subset = mejores[condicion]

print("\n=== Instancias con parámetros óptimos (T=100, alpha=0.95, iter=5000) ===")
print(subset[["nombreinstancia", "toinicial", "alpha", "maxiteraciones", "alturafranja"]])

print(f"\nTotal de instancias que cumplen: {len(subset)}")

# Lista para almacenar resultados
resultados = []

for _, row in mejores.iterrows():
    instancia = row["nombreinstancia"]
    mejor_altura = row["alturafranja"]

    # Todas las ejecuciones de esa instancia
    df_instancia = df[df["nombreinstancia"] == instancia]

    # Promedio incluyendo la mejor
    promedio_incl = df_instancia["alturafranja"].mean()

    # Promedio excluyendo la mejor
    promedio_excl = df_instancia.loc[df_instancia["alturafranja"] != mejor_altura, "alturafranja"].mean()

    # Diferencias
    diff_incl = promedio_incl - mejor_altura
    diff_excl = promedio_excl - mejor_altura

    # Verificar si cumple la condición
    cumple = (
        (row["toinicial"] == 100) and
        (row["alpha"] == 0.95) and
        (row["maxiteraciones"] == 5000)
    )

    resultados.append({
        "nombreinstancia": instancia,
        "mejor_altura": mejor_altura,
        "promedio_incluyendo": promedio_incl,
        "promedio_excluyendo": promedio_excl,
        "diferencia_incluyendo": diff_incl,
        "diferencia_excluyendo": diff_excl,
        "cumple_condicion": cumple
    })

# Convertir a DataFrame
tabla_resultados = pd.DataFrame(resultados)

print("\n=== Comparación de mejores soluciones vs promedios ===")
print(tabla_resultados)

# Promedios por grupo (cumplen vs no cumplen)
promedios_por_grupo = tabla_resultados.groupby("cumple_condicion")[["diferencia_incluyendo", "diferencia_excluyendo"]].mean()

print("\n=== Promedio de diferencias por grupo ===")
print(promedios_por_grupo)


# Agrupar por configuración completa
resumen = df.groupby(["toinicial", "alpha", "maxiteraciones"])["alturafranja"].agg(
    promedio="mean",
    desviacion="std"
).reset_index()

# Ordenar por calidad (altura promedio más baja)
resumen = resumen.sort_values("promedio")

print("\n=== Configuraciones ordenadas por calidad promedio ===")
print(resumen.head(10))  # ver las mejores 10

# Filtrar las que tienen buena calidad pero baja estabilidad
umbral_altura = resumen["promedio"].quantile(0.45)  # mejor 25% en calidad
umbral_desv = resumen["desviacion"].quantile(0.60)  # peor 25% en estabilidad

candidatas = resumen[(resumen["promedio"] <= umbral_altura) & (resumen["desviacion"] >= umbral_desv)]

print("\n=== Configuraciones con buena calidad pero baja estabilidad ===")
print(candidatas)


# ==============================
#   PROMEDIOS POR INSTANCIA
# ==============================
promedios = (
    df.groupby("nombreinstancia")[["alturafranja", "tiempoejecucion"]]
      .mean()
      .sort_values("alturafranja")
)

print("\n=== Promedios por instancia ===")
print(promedios)

# ==============================
#   ANÁLISIS GLOBAL DE PARÁMETROS
# ==============================
mean_alpha = df.groupby("alpha")["alturafranja"].mean().sort_values()
mean_temp = df.groupby("toinicial")["alturafranja"].mean().sort_values()
mean_iter = df.groupby("maxiteraciones")["alturafranja"].mean().sort_values()

print("\n=== Mejores alphas globales ===")
print(mean_alpha)
print("\n=== Mejores temperaturas ===")
print(mean_temp)
print("\n=== Mejores iteraciones ===")
print(mean_iter)

# ==============================
#   VISUALIZACIÓN (GUARDAR ARCHIVOS)
# ==============================

# toinicial, alpha, maxiteraciones, promedio, desviacion

plt.figure(figsize=(10, 6))
plt.scatter(resumen["promedio"], resumen["desviacion"], alpha=0.7, color="teal")

plt.xlabel("Altura promedio (calidad)")
plt.ylabel("Desviación estándar (estabilidad)")
plt.title("Calidad vs Estabilidad por configuración de parámetros")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()

# Guardar el gráfico
plt.savefig("calidad_vs_estabilidad.png")
plt.close()
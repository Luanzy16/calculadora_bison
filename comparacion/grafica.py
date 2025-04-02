import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Cargar ambos archivos
df_cyk = pd.read_csv("/home/luanzy/Documentos/universidad/lenguajes/taller1-C2/comparacion/bison/datosbison.csv")
df_bison = pd.read_csv("/home/luanzy/Documentos/universidad/lenguajes/taller1-C2/comparacion/codigoC/datoscyk.csv")

df_bison["Longitud"] = df_bison["Cadena"].apply(len)
df_cyk["Longitud"] = df_cyk["Cadena"].apply(len)

df_bison["Algoritmo"] = "Bison"
df_cyk["Algoritmo"] = "CYK"

df = pd.concat([df_bison, df_cyk])

df = df.drop(columns=["Cadena"])

sns.set_theme(style="whitegrid")

plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x="Longitud", y="Tiempo (s)", hue="Algoritmo", marker="o", linewidth=2)

plt.xlabel("Número de Caracteres", fontsize=12)
plt.ylabel("Tiempo de Ejecución (s)", fontsize=12)
plt.title("Comparación de Algoritmos Bison vs. CYK: Tiempo vs. Número de Caracteres", fontsize=14)

plt.savefig("grafica_tiempo.png")  # Guarda la imagen

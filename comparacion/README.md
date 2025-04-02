# Compilación y Ejecución de CYK en C y Bison

## Requisitos

Asegúrate de tener instalados los siguientes paquetes en tu sistema:

- **GCC** (para compilar el programa en C)
- **Bison** (para procesar el archivo de gramática)
- **Flex** (si se utiliza un escáner léxico)

En Ubuntu/Debian puedes instalarlos con:
```bash
sudo apt update
sudo apt install bison flex
```

## Compilación

### 1. Generar el parser con Bison
Si tienes un archivo de Bison:
```bash
bison -d grammar.y  # Genera grammar.tab.c y grammar.tab.h
```

Si también tienes un archivo de Flex:
```bash
flex lex.l  # Genera lex.yy.c
```

Tambien puedes cargar todo con el .sh
```sh
chmod +x grammar.sh #recuerda darle permisos
./grammar.sh
```

### 3. Ejecutar el Intérprete  
```sh
./analizador input.txt datosbison.csv
```

### 3. Compilar el programa en C
Si solo usas Bison:
```bash
gcc cykC.c -o cykC
```

Si usas Bison y Flex:
```bash
./cykC input.txt datoscyk.csv
```


## Ejemplo de Entrada (`input.txt`)
El archivo de entrada debe contener cadenas a evaluar, separadas por saltos de línea. Ejemplo:
```
a
ab
abb
abab
ababa
```

## Ejemplo de Salida (`datoscyk.csv`)
```
Cadena,Tiempo (s),Memoria (bytes),Resultado
a,0.002304,4337664,ACEPTADA
ab,0.000124,4337664,ACEPTADA
abb,0.000109,4337664,ACEPTADA
```


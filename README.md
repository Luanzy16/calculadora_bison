# Intérprete de Expresiones Matemáticas 

Este proyecto es un **intérprete de expresiones matemáticas** basado en **Flex y Bison**, permitiendo definir y evaluar funciones de usuario como `avg(a, b) = (a + b) / 2`.

## integrantes 

- Luis Sanchez
- David Bermudez
- Santiago Ospina

## 🚀 Características  
- Evaluación de expresiones matemáticas (`+, -, *, /, |x|`).  
- Definición de funciones con `let nombre(args) = expresión`.  
- Condiciones y bucles (`if-then-else`, `while-do`).  
- Llamadas a funciones predefinidas y de usuario.  

## 📜 Ejemplo de Uso  
Puedes definir y usar funciones en el intérprete, por ejemplo:  

```txt
> let avg(a, b) = (a + b) / 2;
Defined avg
> avg(4, 10)
= 7.0000
```

También puedes definir una función de raíz cuadrada usando el **método de Newton**:  

```txt
> let sum(a,b) = a + b
Defined sum
sum(4,6)
= 10.000
```

## 🛠️ Instalación y Ejecución  

### 🔹 1. Requisitos  
- **Linux** (Ubuntu recomendado)  
- **Flex y Bison** (`sudo apt install flex bison`)  
- **GCC** (`sudo apt install build-essential`)  

### 🔹 2. Compilar el Intérprete  
Ejecuta los siguientes comandos en la terminal:  

```sh
flex fb3-2.l
bison -d fb3-2.y
gcc -o calc fb3-2.tab.c lex.yy.c fb3-2.c -lm
```

Tambien puedes ejecutar el .sh

```sh
chmod +x calcA.sh #recuerda darle permisos
./calcA.sh
```


### 🔹 3. Ejecutar el Intérprete  
```sh
./calc
```

## 📌 Notas  
- Usa `;` para separar expresiones.  
- Presiona **Enter** después de cada línea para evaluar. 
- expresiones como x = (x + 1) debe tener el parenteisis
- Para salir del intérprete, usa **Ctrl + D**.  
 

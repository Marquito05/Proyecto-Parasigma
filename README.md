# SistOpe - Módulo "Administrador de Usuarios y Perfiles"
**INFO198 - Sistemas Operativos** · Universidad Austral de Chile
Docente: Dr. Luis Veas-Castillo
**Grupo:** Parasigma
**Integrantes:**
- Marco Peralta
- Aaron Mancilla
- Vicente Paredes
- Agustin Cortes
---
## Index
- [1. Proposito de la aplicación](#1-propósito-de-la-aplicación)
- [2. Requisitos](#2-requisitos)
- [3. Cómo Compilar y ejecutar](#3-cómo-compilar-y-ejecutar)
- [4. Descripcion de variables de entorno](#4-descripcion-de-variables-de-entorno)
- [5. Formato de los archivos de datos](#5-formato-de-los-archivos-de-datos)
- [6. Estructuras de datos](#6-estructuras-de-datos)
- [7. Estructura del repositorio](#7-estructura-del-repositorio)
- [8. Índice invertido paralelo (Entrega 3 - Problema 1)](#8-índice-invertido-paralelo-entrega-3---problema-1)
## 1. Propósito de la aplicación
> **Entrega 3:** se agregó la opción **8) Crear índice invertido**, que construye el índice invertido de los libros usando una cola de trabajo, varios threads y semáforos. Ver la [sección 8](#8-índice-invertido-paralelo-entrega-3---problema-1).

Este repositorio contiene el primer módulo del sistema **SistOpe**: el **Administrador de Usuarios y Perfiles**.
Es una aplicación de consola escrita en C++ que permite gestionar las dos entidades base sobre las que se construirán los módulos siguientes del curso:
- **Usuarios** - personas que podrán operar el sistema. Cada usuario queda asociado a un perfil.
- **Perfiles** - conjuntos de permisos identificados por nombre (por ejemplo `ADMIN` o `GENERAL`). Cada perfil guarda una lista de números que representan las opciones de menú que ese perfil podrá manipular en futuras entregas.
Sobre ambas entidades la aplicación permite **ingresar, listar y eliminar** registros.

**Características principales:**
- Los datos se mantienen **en memoria** dentro de arreglos dinámicos de `struct` (`User` y `Perfil`), que duplican su capacidad automáticamente cuando se llenan.
- La persistencia es en **archivos de texto plano** (`USUARIOS.TXT` y `PERFILES.TXT`), cuyas rutas se resuelven mediante **variables de entorno**.
- Eliminar archivos de persistencia (`USUARIOS.txt` y `PERFILES.txt`) durante ejecución no afecta el comportamiento del programa. Se vuelven a crear cuando se ingrese un nuevo valor, **pero los valores antiguos no persisten**.
- Se usan dos tablas auxiliares (`std::map`) para búsqueda y eliminación en tiempo constante: una por ID de usuario y otra por nombre de perfil.
- Toda la entrada del usuario es **validada y saneada**: se rechazan opciones no numéricas, IDs duplicados o negativos, nombres con símbolos, perfiles inexistentes y campos vacíos. Las líneas corruptas de los archivos de datos se ignoran al cargar en lugar de detener el programa.
- Los identificadores de las opciones de menú se tratan internamente como **números enteros**, y la opción **`0`** siempre corresponde a *Salir* / *Regresar*.
- Arreglos dinamicos ajustan automaticamente su espacio reservado si se llega al limite.
---
## 2. Requisitos
| Requisito         | Detalle                                                                                           |
| ----------------- | ------------------------------------------------------------------------------------------------- |
| Compilador        | `g++` 13 o superior (el Makefile usa `-std=c++23`; con g++ 11/12 cambiar a `-std=c++2b`) y `make` |
| Sistema operativo | Linux                                                                                             |

Verificar la versión disponible:
```bash
g++ --version
```
---
## 3. Cómo compilar y ejecutar
### 3.1 Clonar el repositorio
```bash
git clone <URL-DEL-REPOSITORIO>
cd <CARPETA-DEL-REPOSITORIO>
```
### 3.2 Compilar
```bash
make
```
### 3.3 Ejecutar
El sistema recibe **argumentos de ejecución**:

| Argumento | Descripción                                       |
| --------- | ------------------------------------------------- |
| `-u`      | Nombre de usuario                                 |
| `-p`      | Password                                          |
| `-f`      | Ruta del archivo de texto que analiza la opción 6 |

Ejemplo con datos reales:
```bash
./bin/main -u Mesmer -p 21032190 -f "data/1984.txt"
```
Los tres argumentos son obligatorios. Si falta alguno, si un argumento está repetido, vacío o es desconocido, el programa muestra un mensaje de error con el uso correcto y termina sin abrir el menú. Si el usuario o el password no son válidos, muestra un error de autenticación y termina.

> **Importante:** la aplicación busca `.env` en el **directorio desde el cual se ejecuta**, lo mismo va para cualquier `path` que se ingrese, es decir, siempre ingresar asumiendo que estamos en la carpeta raiz. Ejecutar siempre desde la carpeta del proyecto (`make` deja los ejecutables en `bin/`).

### 3.4 Menú principal
La interfaz muestra un título, el **usuario** y su **perfil**, y luego las opciones:
```
1) Administrar usuarios y perfiles  (según permisos del perfil; llama a ./bin/user_admin)

2) Multiplicar matrices NxM         (según permisos del perfil; llama a ./bin/multi)

3) Juego                            (en construcción)

4) Es palindromo?                   (llama a ./bin/palindromo)

5) Calcular f(x) = x*x + 2x + 8     (llama a ./bin/funcion_fx)

6) Conteo sobre texto (archivo -f)  (llama a ./bin/conteo con el archivo de -f)

7) Conteo sobre archivo             (llama a ./bin/conteo, que pide la ruta)

8) Crear indice invertido           (según permisos del perfil; llama a ./bin/indice_invertido, ver sección 8)

0) Salir
```

#### Opciones 6 y 7: conteo sobre texto / archivo
Las dos opciones ejecutan el programa `bin/conteo` con `system()`. Muestran un resumen con **vocales, consonantes, caracteres especiales y palabras**, y luego esperan Enter para **VOLVER** al menú.
- **Opción 6** analiza el archivo indicado con `-f`.
- **Opción 7** pide una ruta de archivo (vacío para volver). Acepta rutas con comillas y rutas de Windows (`C:\Users\...` se convierte a `/mnt/c/Users/...` en WSL).

Criterios de conteo:
- Vocales: `a e i o u` con o sin tilde/diéresis, en mayúscula o minúscula. Consonantes: el resto de las letras (incluye `ñ`).
- Caracteres especiales: todo lo que no es letra, dígito ni espacio (puntuación, símbolos, `¿ ¡`, etc.).
- Palabras: secuencias separadas por espacios que contienen al menos una letra o dígito.
- Lee archivos UTF-8 y, si encuentra bytes que no son UTF-8 válido, los interpreta como Latin-1.

Errores controlados (se muestra el mensaje y se vuelve al menú, sin cerrar el programa): ruta vacía, archivo inexistente, ruta que es una carpeta o no es un archivo regular, sin permisos de lectura, archivo binario (contiene bytes nulos) y errores de lectura. Un archivo vacío se informa con un aviso y conteos en cero.

Cualquier opción inválida en el menú (letras, negativos, fuera de rango) muestra un error y vuelve a pedir la opción. Si se cierra la entrada (Ctrl+D) el programa termina limpiamente.

---
## 4. Descripción de las variables de entorno
La configuración se define en el archivo **`.env`**, ubicado en la raíz del proyecto. El formato es una variable por línea, con la sintaxis `CLAVE=VALOR` (sin comillas ni espacios alrededor del signo `=`).

| Variable             | Tipo               | Descripción                                                                                                                                                                                                                    | Valor por defecto                                                       |
| -------------------- | ------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------------- |
| `USER_FILE`          | Ruta de archivo    | Ruta **absoluta o relativa** al archivo de texto donde se persisten los usuarios del sistema. Cada usuario ocupa una línea. Si el archivo no existe, tira error, y se debe crear manualmente, especificando la ruta en `.env`  | `USUARIOS.txt` en `data/`                                               |
| `PERFIL_FILE`        | Ruta de archivo    | Ruta **absoluta o relativa** al archivo de texto donde se persisten los perfiles y sus permisos. Cada perfil ocupa una línea. Si el archivo no existe, tira error, y se debe crear manualmente especificando la ruta en `.env` | `PERFILES.txt` en `data/`                                               |
| `USER_ADMIN_PROGRAM` | Ruta de ejecutable | Programa que abre la opción 1 del menú principal (administrador de usuarios y perfiles). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error.                                                                | Sin valor por defecto; en el `.env` del repositorio: `./bin/user_admin` |
| `MULTI_PROGRAM`      | Ruta de ejecutable | Programa que abre la opción 2 del menú principal (multiplicador de matrices). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error.                                                                           | Sin valor por defecto; en el `.env` del repositorio: `./bin/multi`      |
| `PALINDROMO_PROGRAM` | Ruta de ejecutable | Programa que abre la opción 4 del menú principal (¿es palíndromo?). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error.                                                                                     | Sin valor por defecto; en el `.env` del repositorio: `./bin/palindromo` |
| `FX_PROGRAM`         | Ruta de ejecutable | Programa que abre la opción 5 del menú principal (cálculo de f(x)). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error.                                                                                     | Sin valor por defecto; en el `.env` del repositorio: `./bin/funcion_fx` |
| `CONTEO_PROGRAM`     | Ruta de ejecutable | Programa que abren las opciones 6 y 7 del menú principal (conteo sobre texto y sobre archivo). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error.                                                          | Sin valor por defecto; en el `.env` del repositorio: `./bin/conteo`     |
| `INDICE_PROGRAM`     | Ruta de ejecutable | Programa que abre la opción 8 del menú principal (crear índice invertido). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error. | Sin valor por defecto; en el `.env` del repositorio: `./bin/indice_invertido` |
| `LIBROS_DIR`         | Ruta de carpeta    | Carpeta con los libros (`.txt`) con los que se construye el índice invertido. Se recorre también por subcarpetas. Debe existir y poder leerse. | Sin valor por defecto; en el `.env` del repositorio: `data/LIBROS` |
| `MAPA_LIBROS_FILE`   | Ruta de archivo    | Archivo donde se guarda la asociación **ID ↔ libro** (`1;nombre.txt`). Su carpeta debe existir. | Sin valor por defecto; en el `.env` del repositorio: `data/mapa_libros.txt` |
| `INDICE_FILE`        | Ruta de archivo    | Archivo donde se guarda el **índice invertido** (`palabra;(id,cantidad);...`). Su carpeta debe existir. | Sin valor por defecto; en el `.env` del repositorio: `data/indice_invertido.idx` |
| `N_THREADS`          | Número entero      | Cantidad de threads trabajadores que procesan los libros. Debe ser un entero entre 1 y 256. | Sin valor por defecto; en el `.env` del repositorio: `4` |

**Ejemplo de `.env` con rutas relativas:**
```
USER_FILE=data/USUARIOS.txt
PROFILE_FILE=data/PERFILES.txt
USER_ADMIN_PROGRAM=./bin/user_admin
MULTI_PROGRAM=./bin/multi
PALINDROMO_PROGRAM=./bin/palindromo
FX_PROGRAM=./bin/funcion_fx
CONTEO_PROGRAM=./bin/conteo
INDICE_PROGRAM=./bin/indice_invertido
LIBROS_DIR=data/LIBROS
MAPA_LIBROS_FILE=data/mapa_libros.txt
INDICE_FILE=data/indice_invertido.idx
N_THREADS=4
```

**Ejemplo con rutas absolutas:**
```
USER_FILE=/home/usuario/sistope/datos/USUARIOS.TXT
PERFIL_FILE=/home/usuario/sistope/datos/PERFILES.TXT
USER_ADMIN_PROGRAM=./home/usuario/sistope/bin/user_admin
MULTI_PROGRAM=./home/usuario/sistope/bin/user_admin/multi
PALINDROMO_PROGRAM=./home/usuario/sistope/bin/user_admin/palindromo
FX_PROGRAM=./home/usuario/sistope/bin/user_admin/funcion_fx
CONTEO_PROGRAM=./home/usuario/sistope/bin/user_admin/conteo
```

  
Consideraciones:
- Si la ruta indicada incluye directorios que no existen, la aplicación no funcionara y dara los errores correspondientes.
- Las líneas del `.env` se leen buscando **el primer** `=`, por lo que los valores pueden contener `:` y `\` sin problema.
- El programa no fue probado en Windows, ya que no compila en Windows por sintaxis de Filesystem y conversiones de `filesystem::path` a `string` que solo funcionan en Linux.
- **`.env` no debería versionarse** si contiene rutas absolutas de una máquina en particular. Se recomienda mantener en el repositorio un archivo de ejemplo (`.env.example`) y agregar `.env` al `.gitignore`. De lo contrario, mantener rutas relativas.

---

## 5. Formato de los archivos de datos
### `USUARIOS.TXT`
Un usuario por línea, campos separados por coma, en el orden del `struct`:
```

id,nombre,username,password,perfil

```

Ejemplo:
```

1,Luis Veas,lvc,1234,ADMIN

2,Maria Alvarez,malvarez,abcd,GENERAL

```

Reglas de validación aplicadas al cargar (las líneas que no las cumplan se ignoran):
- `id` debe contener solo dígitos y ser mayor que 0.
- `nombre` debe contener solo letras y espacios.
- `username` y `password` no pueden estar vacíos.
- `perfil` debe contener solo letras y **existir previamente** en `PERFILES.TXT`.
### `PERFILES.TXT`
Un perfil por línea, con la estructura `[ string, arreglo de enteros ]`:
```

NOMBRE;permiso1,permiso2,permiso3

```

Ejemplo:
```

ADMIN;0,1,2,3,4

GENERAL;0,1,3

```
Los números representan las opciones de menú que ese perfil podrá manipular en futuras entregas del sistema. El nombre del perfil se almacena siempre en mayúsculas y debe contener solo letras; los permisos deben ser enteros no negativos.

---
## 6. Estructuras de datos (`models.h`)
```cpp
struct User {
    int id;
    std::string name;
    std::string username;
    std::string password;
    std::string profile;
};

struct Perfil {
    std::string name;              // Nombre del perfil (siempre en mayúsculas)
    std::vector<int> perm;         // Opciones de menú habilitadas
    std::vector<int> users_id;     // IDs de los usuarios que tienen este perfil

};

```

Ambos conjuntos se mantienen en memoria como **arreglos dinámicos de structs** (`User*` y `Perfil*`) con crecimiento por duplicación de capacidad, acompañados de:
- `std::map<int,int> id_table` - mapea *ID de usuario* → *posición en el arreglo*.
- `std::map<std::string,int> name_table` - mapea *nombre de perfil* → *posición en el arreglo*.
---
## 7. Estructura del repositorio
```
parasigma/
├── src
├── include
├── programs/
│   ├── conteo/
│   │   ├── src
│   │   ├── include
│   │   └── rules.mk
│   ├── function/
│   │   └── ...
│   ├── indice_invertido/      (Entrega 3: índice invertido con threads, ver sección 8)
│   │   ├── src
│   │   ├── include
│   │   ├── PARTE2.md
│   │   └── rules.mk
│   ├── matrix_multiplier/
│   │   └── ...
│   ├── palindromo/
│   │   └── ...
│   └── user_manager/
│       └── ...
├── out/
│   ├── conteo
│   ├── function
│   ├── matrix_multiplier
│   ├── palindromo
│   └── user_manager
├── data/
│   ├── LIBROS/                (libros .txt para el índice invertido, ver LIBROS_DIR)
│   ├── PERFILES.txt
│   ├── USUARIOS.txt
│   └── ...
├── .env
├── Makefile
└── README.md
```

---
## 8. Índice invertido paralelo (Entrega 3 - Problema 1)

### 8.1 Qué hace y cómo se ejecuta
La opción **8) Crear indice invertido** del menú (requiere el permiso `8` en el perfil, por ejemplo `ADMIN`) ejecuta con `system()` el programa `bin/indice_invertido`. También se puede ejecutar directamente desde la carpeta raíz del proyecto:
```bash
make
./bin/indice_invertido
```
El programa:
1. lee y valida las variables del `.env`: `LIBROS_DIR`, `MAPA_LIBROS_FILE`, `INDICE_FILE` y `N_THREADS` (sección 4);
2. revisa que la carpeta de libros exista y se pueda leer, y busca los archivos `.txt` (también en subcarpetas);
3. asigna un ID a cada libro (en orden alfabético) y guarda la asociación en `MAPA_LIBROS_FILE`;
4. pone todos los libros en la cola de trabajo;
5. crea `N_THREADS` threads: cada uno saca libros de la cola y los procesa hasta que la cola queda vacía;
6. espera a que todos los threads terminen (`join`), guarda el índice en `INDICE_FILE` y muestra un resumen.

### 8.2 Flujo: cola → threads → semáforos → resultados → índice invertido
```
 thread principal                              N_THREADS threads trabajadores
 ────────────────                              ──────────────────────────────
 1. valida el .env
 2. busca los libros (.txt) en LIBROS_DIR
 3. asigna IDs y guarda MAPA_LIBROS_FILE
 4. pone todos los libros en la COLA
 5. crea los N threads ──────────────────────► mientras queden libros en la cola:
                                                  sacarLibro()        semáforo semCola
                                                  procesarLibro()     semáforo del índice (PARTE 2)
                                                  muestra el avance   semáforo semPantalla
 6. join() de los N threads ◄───────────────── la cola quedó vacía: el thread termina
 7. escribe el índice en INDICE_FILE
 8. muestra el resumen
```

- **Cola**: `std::queue<Libro>` con todos los libros (cada `Libro` tiene ID, nombre y ruta). Se llena antes de crear los threads. Cada libro se saca dentro de una sección crítica, así que un libro llega a un solo thread: nunca dos threads procesan el mismo libro.
- **Threads**: se crean `N_THREADS` threads con `std::thread`. Cada uno ejecuta `hiloTrabajador()`: saca un libro, lo procesa y vuelve por otro mientras queden.
- **Semáforos**: ver sección 8.3.
- **Resultados → índice invertido** *(PARTE 2: completar)*: cada thread cuenta las palabras de su libro en un mapa local (sin semáforo, porque es memoria propia del thread) y después lo agrega al índice compartido dentro de la sección crítica del semáforo del índice. Al final, el thread principal escribe el índice en `INDICE_FILE`.

### 8.3 Semáforos y secciones críticas
Se usan semáforos POSIX (`sem_t`), creados en `main` con valor 1: `sem_wait` entra a la sección crítica (si otro thread está adentro, espera) y `sem_post` sale de ella.

| Recurso compartido | Semáforo | Sección crítica |
| --- | --- | --- |
| Cola de libros | `semCola` | Sacar un libro de la cola (`sacarLibro`) |
| Contador de libros procesados y pantalla | `semPantalla` | Sumar al contador y escribir el mensaje del avance |
| Índice invertido | `indice.semaforo` | Agregar el conteo de un libro al índice *(PARTE 2)* |

Agregar los libros a la cola no necesita semáforo, porque se hace antes de crear los threads, cuando solo existe el thread principal. Escribir el índice tampoco, porque se hace después del `join`.

### 8.4 Término de los threads
Como todos los libros se ponen en la cola antes de crear los threads, cuando un thread encuentra la cola vacía sabe que ya no queda trabajo y termina. El thread principal espera a todos con `join()` y recién después escribe el índice. El resumen final muestra las cuatro condiciones de la pauta, por ejemplo:
```
===== RESUMEN =====
Libros pendientes en la cola: 0
Libros procesados: 49 de 49
Threads terminados: 4 de 4
Indice invertido guardado en data/indice_invertido.idx
```

### 8.5 Archivos que genera
- **`MAPA_LIBROS_FILE`**: asociación ID ↔ libro, una línea por libro:
  ```
  1;don_quijote.txt
  2;el_hobbit.txt
  ```
- **`INDICE_FILE`**: el índice invertido. Cada palabra aparece una sola vez, con los libros donde aparece (por ID) y cuántas veces:
  ```
  computador;(1,15);(3,8);(7,21)
  ```
  *(PARTE 2: completar con la definición de palabra usada y un extracto real del índice generado.)*

### 8.6 Archivos del programa (`programs/indice_invertido/`)
| Archivo | Contenido | Parte |
| --- | --- | --- |
| `src/main.cpp` | Cola de trabajo, semáforos, threads, `join` y resumen | 1 |
| `src/config.cpp` | Lectura y validación de las variables del `.env` | 1 |
| `src/libros.cpp` | Revisión de la carpeta, IDs y mapa ID ↔ libro | 1 |
| `src/indice.cpp` | Conteo de palabras y escritura del índice | 2 |
| `PARTE2.md` | Guía para la Parte 2 | — |
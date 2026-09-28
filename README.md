# SistOpe - Módulo "Administrador de Usuarios y Perfiles"

**INFO198 - Sistemas Operativos** · Universidad Austral de Chile
Docente: Dr. Luis Veas-Castillo · **Entrega 1**

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


## 1. Propósito de la aplicación

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

| Requisito | Detalle |
|---|---|
| Compilador | `g++` con soporte para **C++17** (por el uso de `<filesystem>`) |
| Sistema operativo | Linux |

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
g++ -std=c++17 -Wall -o main main.cpp utils.cpp env_config.cpp user_repository.cpp profile_repository.cpp
```

### 3.3 Ejecutar

```bash
./main    
```

En el primer arranque, si no existe el archivo `.env`, la aplicación lo crea automáticamente apuntando a `USUARIOS.TXT` y `PERFILES.TXT` dentro del directorio de trabajo actual. También crea ambos archivos de datos si no existen, dejando `PERFILES.TXT` con los dos perfiles por defecto del enunciado:

```
ADMIN;0,1,2,3,4
GENERAL;0,1,3
```

> **Importante:** la aplicación busca `.env` en el **directorio desde el cual se ejecuta**, no en el directorio del binario. Ejecutar siempre desde la carpeta del proyecto.

### 3.4 Navegación

```
Menú principal
  0) Salir
  1) Gestionar usuarios  ->  0) Regresar
                             1) Ingresar usuarios
                             2) Listar usuarios
                             3) Eliminar usuarios
  2) Gestionar perfiles  ->  0) Regresar
                             1) Ingresar perfil
                             2) Listar perfiles
                             3) Eliminar perfil
```

---

## 4. Descripción de las variables de entorno

La configuración se define en el archivo **`.env`**, ubicado en la raíz del proyecto. El formato es una variable por línea, con la sintaxis `CLAVE=VALOR` (sin comillas ni espacios alrededor del signo `=`).

| Variable | Tipo | Descripción | Valor por defecto |
|---|---|---|---|
| `USER_FILE` | Ruta de archivo | Ruta **absoluta o relativa** al archivo de texto donde se persisten los usuarios del sistema. Cada usuario ocupa una línea. Si el archivo no existe, se crea vacío al iniciar. | `USUARIOS.TXT` en el directorio de ejecución |
| `PERFIL_FILE` | Ruta de archivo | Ruta **absoluta o relativa** al archivo de texto donde se persisten los perfiles y sus permisos. Cada perfil ocupa una línea. Si el archivo no existe, se crea con los perfiles `ADMIN` y `GENERAL` por defecto. | `PERFILES.TXT` en el directorio de ejecución |
| `USER_ADMIN_PROGRAM` | Ruta de ejecutable | Programa que abre la opción 1 del menú principal (administrador de usuarios y perfiles). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error. | Sin valor por defecto; en el `.env` del repositorio: `./bin/user_admin` |
| `MULTI_PROGRAM` | Ruta de ejecutable | Programa que abre la opción 2 del menú principal (multiplicador de matrices). El menú lo ejecuta con `system()`. Si falta, el menú muestra un error. | Sin valor por defecto; en el `.env` del repositorio: `./bin/multi` |

**Ejemplo de `.env` con rutas relativas:**

```
USER_FILE=USUARIOS.TXT
PERFIL_FILE=PERFILES.TXT
USER_ADMIN_PROGRAM=./bin/user_admin
MULTI_PROGRAM=./bin/multi
```

**Ejemplo con rutas absolutas:**

```
USER_FILE=/home/usuario/sistope/datos/USUARIOS.TXT
PERFIL_FILE=/home/usuario/sistope/datos/PERFILES.TXT
```

Consideraciones:

- Si la ruta indicada incluye directorios que no existen, la aplicación los crea antes de generar el archivo.
- Las líneas del `.env` se leen buscando **el primer** `=`, por lo que los valores pueden contener `:` y `\` sin problema.
- El programa no fue probado en Windows, ya que no compila en Windows por sintaxis de Filesystem y conversiones de `filesystem::path` a `string` que solo funcionan en Linux.
- **`.env` no debería versionarse** si contiene rutas absolutas de una máquina en particular. Se recomienda mantener en el repositorio un archivo de ejemplo (`.env.example`) y agregar `.env` al `.gitignore`.

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
    std::string name;              // Nombre del perfil (siempre en mayúsculas)
    std::vector<int> perm;         // Opciones de menú habilitadas
    std::vector<int> users_id;     // IDs de los usuarios que tienen este perfil
};
```

Ambos conjuntos se mantienen en memoria como **arreglos dinámicos de structs** (`User*` y `Perfil*`) con crecimiento por duplicación de capacidad, acompañados de:

- `std::map<int,int> id_table` - mapea *ID de usuario* → *posición en el arreglo*.
- `std::map<std::string,int> name_table` - mapea *nombre de perfil* → *posición en el arreglo*.

---
## 7. Estructura del repositorio

```
.
├── main.cpp                  # Punto de entrada y menú principal
├── models.h                  # Definición de los structs User y Perfil
├── env_config.h/.cpp         # Lectura y creación del archivo .env
├── user_repository.h/.cpp    # Carga, ingreso, listado y eliminación de usuarios
├── profile_repository.h/.cpp # Carga, ingreso, listado y eliminación de perfiles
├── utils.h/.cpp              # Utilidades: trim, mayúsculas, saneamiento de cin
├── .env.example              # Plantilla de variables de entorno
├── USUARIOS.TXT              # Persistencia de usuarios (generado en ejecución)
├── PERFILES.TXT              # Persistencia de perfiles (generado en ejecución)
└── README.md
```

# Guía de pruebas — Multiplicador de matrices y conexión con `system()`

Esta guía explica cómo compilar, ejecutar y comprobar las dos funcionalidades de la Entrega 2 que están en esta parte del proyecto:

1. **`multi`**: un programa independiente que multiplica dos matrices leídas desde archivos de texto.
2. **Conexión entre programas con `system()`**: el menú principal ejecuta `user_admin` (opción 1) y `multi` (opción 2) como programas separados y revisa cómo terminó cada uno.

> Todos los comandos se ejecutan **desde la carpeta raíz del proyecto** (donde está el `.env`), porque las rutas `./bin/...` y el `.env` son relativas a esa carpeta.

---

## 1. Archivos nuevos y modificados

| Archivo | Qué contiene |
|---|---|
| `programs/matrix_multiplier/src/main.cpp` | `main` de `multi`: valida argumentos, lee A y B, valida dimensiones, multiplica y muestra el resultado |
| `programs/matrix_multiplier/src/matrix.cpp` y `include/matrix.hpp` | `struct Matriz`, lectura y validación de archivos, multiplicación e impresión |
| `programs/matrix_multiplier/rules.mk` | Reglas para compilar `bin/multi` |
| `src/program_runner.cpp` y `include/program_runner.hpp` | Todo lo relacionado con `system()`: argumentos seguros, revisión del código de salida y las opciones 1 y 2 del menú |
| `src/env_config.cpp` | Nueva función `leerVariableEnv()` para leer cualquier variable del `.env` |
| `src/menu_repository.cpp`, `src/main.cpp` | El menú ahora conoce el perfil del usuario y la opción 2 llama a `multi` |
| `programs/user_manager/src/main.cpp` | `user_admin` valida que reciba sus 2 argumentos |
| `.env` | Nuevas variables `USER_ADMIN_PROGRAM` y `MULTI_PROGRAM` |
| `README.md` | Las dos variables nuevas en la tabla de variables de entorno |
| `Makefile` | Compila también `multi` y recompila `bin/main` cuando cambia `src/main.cpp` |
| `matrices/` | Archivos de ejemplo para las pruebas |

---

## 2. Compilar

```bash
make clean
make
ls bin
```

Salida esperada de `ls bin`:

```
main  multi  user_admin
```

Si aparecen advertencias al compilar, deberían ser solo las de `src/utils.cpp` (`-Wconversion`), que ya existían antes de estos cambios.

---

## 3. Formato de los archivos de matrices

- Una fila de la matriz por línea.
- Los elementos de la fila van separados por **un** carácter: el separador (por ejemplo `#`).
- Se aceptan enteros y decimales con punto: `5`, `-3`, `2.75`.
- Se permiten espacios alrededor de cada número; las líneas vacías se ignoran.

Ejemplo, `matrices/A.TXT` (matriz de 2x3 con separador `#`):

```
1#2#3
4#5#6
```

Archivos de ejemplo incluidos en `matrices/`:

| Archivo | Contenido | Para qué sirve |
|---|---|---|
| `A.TXT` | `1#2#3` / `4#5#6` | Matriz válida de 2x3 (separador `#`) |
| `B.TXT` | `7#8` / `9#10` / `11#12` | Matriz válida de 3x2 (separador `#`) |
| `C.TXT` | `1.5,-2` / `0,3` | Matriz válida de 2x2 con decimales y negativos (separador `,`) |
| `D.TXT` | `2,1` / `-1,0.5` | Matriz válida de 2x2 (separador `,`) |
| `error_letras.TXT` | `1#2#3` / `4#x#6` | Contenido que no es número |
| `error_filas.TXT` | `1#2#3` / `4#5` | Filas con distinta cantidad de columnas |
| `error_separador.TXT` | `1#2#3` / `4##6` | Dos separadores seguidos (elemento vacío) |
| `error_vacio.TXT` | *(vacío)* | Archivo sin matriz |

Las rutas pueden ser completas (`/home/marco/proyecto-parasigma/matrices/A.TXT`) o relativas a la carpeta del proyecto (`matrices/A.TXT`). El símbolo `~` **no** se reemplaza por la carpeta personal, así que hay que escribir la ruta completa.

---

## 4. Probar `multi` directamente (sin el menú)

Se ejecuta con 5 argumentos, como pide el enunciado:

```bash
./bin/multi <rutaA> <rutaB> <separador> <usuario> <perfil>
```

Después de cada ejecución, `echo $?` muestra el **código de salida** del programa. Es el mismo número que después recibe el menú a través de `system()`:

| Código | Significado |
|---|---|
| 0 | Multiplicación correcta |
| 1 | Cantidad de argumentos incorrecta o separador inválido |
| 2 | No se pudo abrir o leer un archivo |
| 3 | Formato o contenido inválido en un archivo |
| 4 | Las dimensiones no permiten multiplicar |

### 4.1 Caso válido: A (2x3) × B (3x2)

```bash
./bin/multi matrices/A.TXT matrices/B.TXT "#" lvc ADMIN
echo $?
```

Salida esperada:

```
==================================================
            MULTIPLICADOR DE MATRICES
  Usuario: lvc  |  Perfil: ADMIN
==================================================

Matriz A (2x3) - archivo: matrices/A.TXT
        1.00        2.00        3.00
        4.00        5.00        6.00

Matriz B (3x2) - archivo: matrices/B.TXT
        7.00        8.00
        9.00       10.00
       11.00       12.00

Resultado A x B (2x2):
       58.00       64.00
      139.00      154.00
0
```

Comprobación a mano de la primera casilla: fila 1 de A por columna 1 de B = 1·7 + 2·9 + 3·11 = 58.

### 4.2 Caso válido con decimales, negativos y otro separador

```bash
./bin/multi matrices/C.TXT matrices/D.TXT "," lvc ADMIN
```

Últimas líneas esperadas (código `0`):

```
Resultado A x B (2x2):
        5.00        0.50
       -3.00        1.50
```

Comprobación: 1.5·2 + (−2)·(−1) = 5 y 1.5·1 + (−2)·0.5 = 0.5.

### 4.3 Casos de error

En todos se muestra primero el título con el usuario y el perfil, y después el mensaje de error.

**E1 · Dimensiones incompatibles** (A de 2x3 por A de 2x3) — código **4**

```bash
./bin/multi matrices/A.TXT matrices/A.TXT "#" lvc ADMIN
```
```
ERROR! No se puede multiplicar A (2x3) por B (2x3).
       Las columnas de A (3) deben ser iguales a las filas de B (2).
```
(Antes del error se muestran A y B tal como se leyeron.)

**E2 · El separador no coincide con el del archivo** — código **3**

```bash
./bin/multi matrices/A.TXT matrices/B.TXT "," lvc ADMIN
```
```
ERROR! Archivo matrices/A.TXT, línea 1: el elemento '1#2#3' no es un número válido.
       Revise que el separador del archivo sea ',' y que los decimales usen punto (ej: 2.5).
```

**E3 · Contenido que no es número** — código **3**

```bash
./bin/multi matrices/error_letras.TXT matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! Archivo matrices/error_letras.TXT, línea 2: el elemento 'x' no es un número válido.
       Revise que el separador del archivo sea '#' y que los decimales usen punto (ej: 2.5).
```

**E4 · Filas de distinto largo** — código **3**

```bash
./bin/multi matrices/error_filas.TXT matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! Archivo matrices/error_filas.TXT, línea 2: tiene 2 elementos, pero la primera fila tiene 3.
       Todas las filas de una matriz deben tener la misma cantidad de columnas.
```

**E5 · Dos separadores seguidos** — código **3**

```bash
./bin/multi matrices/error_separador.TXT matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! Archivo matrices/error_separador.TXT, línea 2: el elemento 2 está vacío.
       Revise que no haya dos separadores seguidos ni un separador al inicio o al final de la línea.
```

**E6 · Archivo vacío** — código **3**

```bash
./bin/multi matrices/error_vacio.TXT matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! El archivo matrices/error_vacio.TXT está vacío, no contiene ninguna matriz.
```

**E7 · El archivo no existe** — código **2**

```bash
./bin/multi matrices/NO_EXISTE.TXT matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! No se pudo abrir el archivo: matrices/NO_EXISTE.TXT
       Revise que la ruta sea correcta y que el archivo exista.
```

**E8 · La ruta es una carpeta** — código **2**

```bash
./bin/multi matrices matrices/B.TXT "#" lvc ADMIN
```
```
ERROR! No se pudo leer el archivo: matrices
       Revise que la ruta sea un archivo de texto y no una carpeta.
```

**E9 · Separador inválido** (más de un carácter, o un dígito, punto, signo o espacio) — código **1**

```bash
./bin/multi matrices/A.TXT matrices/B.TXT "##" lvc ADMIN
```
```
ERROR! Separador inválido: '##'.
       Debe ser un solo carácter y no puede ser un dígito, un punto, un signo (+ -) ni un espacio.
```

**E10 · Faltan argumentos** — código **1**

```bash
./bin/multi matrices/A.TXT matrices/B.TXT "#"
```
```
ERROR! Cantidad de argumentos incorrecta (se recibieron 3 y se esperan 5).
Uso:     ./bin/multi <rutaA> <rutaB> <separador> <usuario> <perfil>
Ejemplo: ./bin/multi "/home/lvc/A.TXT" "/home/lvc/B.TXT" "#" lvc ADMIN
```

---

## 5. Probar la conexión desde el menú principal

Por ahora `./bin/main` recibe usuario, contraseña y archivo en ese orden (cuando se integre `-u -p -f`, solo cambia esta línea):

```bash
./bin/main Mesmer 21032190 archivo.txt
```

`Mesmer` tiene perfil `ADMIN`, que incluye el permiso 2. Para el caso sin permiso se usa el usuario `A` con contraseña `A` (perfil `B`, que solo tiene el permiso 1).

### 5.1 Multiplicar desde el menú

Escribir `2`, luego `matrices/A.TXT`, `matrices/B.TXT`, `#` y, al final, ENTER para volver al menú.

```
Ingresar opcion: 2

=== MULTIPLICAR MATRICES ===
Cada archivo debe tener una fila de la matriz por línea, con los números separados por el separador.
Para cancelar, deje un dato vacío y presione ENTER.

Ruta del archivo de la matriz A (ej: /home/usuario/A.TXT): matrices/A.TXT
Ruta del archivo de la matriz B (ej: /home/usuario/B.TXT): matrices/B.TXT
Separador de los elementos (ej: #): #

[Menú] Ejecutando: './bin/multi' 'matrices/A.TXT' 'matrices/B.TXT' '#' 'Mesmer' 'ADMIN'

==================================================
            MULTIPLICADOR DE MATRICES
  Usuario: Mesmer  |  Perfil: ADMIN
==================================================

Matriz A (2x3) - archivo: matrices/A.TXT
        1.00        2.00        3.00
        4.00        5.00        6.00

Matriz B (3x2) - archivo: matrices/B.TXT
        7.00        8.00
        9.00       10.00
       11.00       12.00

Resultado A x B (2x2):
       58.00       64.00
      139.00      154.00

[Menú] El programa terminó correctamente (código 0).

Presione ENTER para volver al menú principal...
```

Qué comprobar:
- La línea `[Menú] Ejecutando:` muestra el comando exacto que recibe `system()`.
- `multi` muestra el usuario y el perfil que le envió el menú.
- Al terminar, el menú informa el código de salida del programa (0).

### 5.2 El error del programa llega al menú

Igual que 5.1, pero con `matrices/A.TXT` como matriz A **y** como matriz B. Últimas líneas:

```
ERROR! No se puede multiplicar A (2x3) por B (2x3).
       Las columnas de A (3) deben ser iguales a las filas de B (2).

[Menú] El programa terminó con errores (código 4).
```

El `4` es el `return ERROR_DIMENSIONES;` de `multi`, que el menú obtiene con `WEXITSTATUS`.

### 5.3 Usuario sin permiso

```bash
./bin/main A A archivo.txt
```
```
Ingresar opcion: 2
Usuario no tiene permisos para este modulo.
```

No se ejecuta ningún programa.

### 5.4 Rutas con espacios

```bash
mkdir -p "matrices/mis matrices"
cp matrices/A.TXT matrices/B.TXT "matrices/mis matrices/"
```

En el menú: opción `2`, rutas `matrices/mis matrices/A.TXT` y `matrices/mis matrices/B.TXT`, separador `#`. El resultado es el mismo de 5.1:

```
[Menú] Ejecutando: './bin/multi' 'matrices/mis matrices/A.TXT' 'matrices/mis matrices/B.TXT' '#' 'Mesmer' 'ADMIN'
...
[Menú] El programa terminó correctamente (código 0).
```

Cada ruta llega completa como un solo argumento gracias a las comillas simples. (Después se puede borrar la carpeta con `rm -r "matrices/mis matrices"`.)

### 5.5 Intento de inyección de comandos

Opción `2`, y como ruta de A escribir: `matrices/A.TXT; echo HACKEADO` (B: `matrices/B.TXT`, separador `#`).

```
[Menú] Ejecutando: './bin/multi' 'matrices/A.TXT; echo HACKEADO' 'matrices/B.TXT' '#' 'Mesmer' 'ADMIN'
...
ERROR! No se pudo abrir el archivo: matrices/A.TXT; echo HACKEADO
       Revise que la ruta sea correcta y que el archivo exista.

[Menú] El programa terminó con errores (código 2).
```

La palabra `HACKEADO` **no** se imprime sola: todo el texto llegó a `multi` como una ruta. Para ver lo que habría pasado sin las comillas (como estaba antes la opción 1), ejecutar en la terminal:

```bash
sh -c './bin/multi matrices/A.TXT; echo HACKEADO'
```

Ahí la shell ejecuta `echo HACKEADO` como un segundo comando y lo imprime.

### 5.6 Datos con comilla simple

Opción `2`, ruta de A: `matrices/A.TXT'` (con comilla al final), luego B y el separador:

```
ERROR! Los datos no pueden contener comillas simples ('). Operación cancelada.
```

No se ejecuta ningún programa.

### 5.7 Cancelar

Opción `2` y presionar ENTER sin escribir la ruta de A:

```
Operación cancelada.
```

### 5.8 Programa no encontrado (código 127)

```bash
mv bin/multi bin/multi.bak
./bin/main Mesmer 21032190 archivo.txt      # opción 2 con matrices/A.TXT, matrices/B.TXT y #
mv bin/multi.bak bin/multi
```
```
[Menú] Ejecutando: './bin/multi' 'matrices/A.TXT' 'matrices/B.TXT' '#' 'Mesmer' 'ADMIN'
sh: 1: ./bin/multi: not found

[Menú] ERROR! No se encontró el programa (código 127). Compile con 'make' y revise la ruta en el .env.
```

La primera línea la escribe la shell; el código 127 es la forma estándar de la shell de avisar que no encontró el comando.

### 5.9 Ctrl+C dentro de un programa llamado

Opción `1`; cuando aparezca el menú del administrador, presionar **Ctrl+C**:

```
Ingresar opcion: ^C
[Menú] El programa fue interrumpido por la señal 2.
```

Luego vuelve a aparecer el menú principal: Ctrl+C terminó solo el programa hijo (la señal 2 es `SIGINT`).

### 5.10 La opción 1 sigue funcionando

Opción `1` → se abre el administrador de usuarios y perfiles de la Entrega 1 → salir con `0`:

```
[Menú] El programa terminó correctamente (código 0).
```

### 5.11 Falta una variable en el `.env`

Borrar temporalmente la línea `MULTI_PROGRAM=./bin/multi` del `.env` y elegir la opción `2`:

```
ERROR! Falta la variable MULTI_PROGRAM en el archivo .env.
```

(Después volver a agregar la línea.)

---

## 6. Cómo funciona (apoyo para la presentación)

**¿Qué hace `system()`?** `system("comando")` hace tres cosas (internamente equivale a `fork` + `exec` + `waitpid`):

1. Crea un proceso hijo.
2. En ese hijo ejecuta la shell `/bin/sh -c "comando"`, y la shell ejecuta el programa pedido.
3. El proceso padre (el menú) queda esperando hasta que el hijo termine.

Mientras `multi` o `user_admin` están abiertos, el árbol de procesos es:

```
main (menú)  →  sh  →  multi
```

**¿Cómo sabe el menú si el programa funcionó?** `system()` retorna un número (`estado`) que describe cómo terminó el hijo. `ejecutarPrograma()` lo revisa en este orden:

| Revisión | Qué significa |
|---|---|
| `system(NULL) == 0` | No hay una shell disponible (se revisa antes de ejecutar) |
| `estado == -1` | No se pudo crear el proceso hijo |
| `WIFSIGNALED(estado)` | El programa fue detenido por una señal; `WTERMSIG(estado)` dice cuál (2 = Ctrl+C) |
| `WEXITSTATUS(estado)` | El número que retornó el `main` del programa: 0 = bien, 1–4 = errores de `multi`, 126/127 = la shell no pudo ejecutarlo o no lo encontró |

**¿Por qué cada argumento va entre comillas simples?** El comando es un texto que interpreta la shell. Dentro de `'...'` la shell no interpreta ningún carácter especial (`;`, `|`, `&`, `$`, espacios), así que cada dato llega completo como un solo argumento y no puede convertirse en otro comando. Lo único que rompería esa protección es otra comilla simple, por eso se rechazan los datos que la contienen (casos 5.5 y 5.6).

**¿Por qué `std::cout.flush()` antes de `system()`?** Lo que imprime el menú queda primero en un buffer en memoria. Si no se vacía antes de crear el hijo, los mensajes del menú podrían aparecer después de los del programa llamado (se nota, por ejemplo, al redirigir la salida a un archivo).

**¿Por qué Ctrl+C no cierra el menú?** Ctrl+C envía la señal `SIGINT` a todos los procesos que están en primer plano en la terminal (menú, shell y programa hijo). Mientras espera, `system()` hace que el proceso padre ignore esa señal, así que solo termina el hijo y el menú recibe el estado "terminó por la señal 2".

**¿Por qué las rutas de los programas están en el `.env`?** El enunciado pide que toda variable relevante esté en el `.env`. Así, si los ejecutables cambian de carpeta, basta con editar `USER_ADMIN_PROGRAM` o `MULTI_PROGRAM`, sin recompilar.

**¿Cómo se multiplica?** Solo se puede multiplicar si las columnas de A son iguales a las filas de B. El resultado tiene las filas de A y las columnas de B.
En el código son tres ciclos `for` anidados: `i` recorre las filas de A, `j` las columnas de B y `k` recorre las columnas de A sumando los productos:

```
resultado[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + ...   (un término por cada columna de A)
```

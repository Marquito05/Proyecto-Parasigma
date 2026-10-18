# Índice invertido: guía para la Parte 2

## 1. Qué está hecho y qué falta

**Parte 1 (lista).** Está en `src/main.cpp`, `src/config.cpp` y `src/libros.cpp`:

- lee y valida las variables del `.env` (`LIBROS_DIR`, `MAPA_LIBROS_FILE`, `INDICE_FILE`, `N_THREADS`);
- revisa la carpeta de libros, asigna un ID a cada libro y guarda el mapa `ID;nombre`;
- pone los libros en la cola de trabajo, crea los semáforos y los N threads;
- espera a los threads (`join`) y muestra el resumen.

**Parte 2 (la tuya).** Implementar las dos funciones de `src/indice.cpp`:

- `procesarLibro()`: contar las palabras de un libro y agregarlas al índice compartido.
- `escribirIndice()`: guardar el índice en el archivo `INDICE_FILE`.

Hoy las dos retornan `false`. Por eso el programa muestra `ERROR: no se pudo procesar el libro ...`
para cada libro y termina diciendo que el índice no se guardó.

## 2. Lo que ya está definido (`include/indice.hpp`)

```cpp
struct IndiceInvertido {
    std::map<std::string, std::map<int, int>> palabras;  // palabra -> (ID del libro -> cantidad)
    sem_t semaforo;                                      // protege 'palabras'
};

bool procesarLibro(const Libro &libro, IndiceInvertido &indice);
bool escribirIndice(IndiceInvertido &indice, const std::string &archivo);
```

- Cada `Libro` (en `include/libros.hpp`) trae tres datos:
  - `id`: el número que va en el índice;
  - `nombre`: el nombre del archivo;
  - `ruta`: la ruta para abrir el archivo.
- `main` ya crea `indice.semaforo` con valor 1 (`sem_init`) y lo destruye al final. Tú solo usas `sem_wait` y `sem_post`.
- No cambies las firmas de las funciones: `main.cpp` las usa tal como están.

## 3. `procesarLibro()`

Se llama desde **varios threads al mismo tiempo**, cada uno con un libro distinto. `main` garantiza
que un libro nunca llega a dos threads.

1. Abrir el archivo: `std::ifstream archivo(libro.ruta);`. Si no se abre, `return false;`.
2. Separar el texto en palabras y contarlas en un **mapa local**, por ejemplo
   `std::map<std::string, int> conteo;`. Es una variable local del thread, así que **no necesita semáforo**.
   - Decide qué es una palabra y anótalo en el README. Se recomienda:
     - solo letras y números, pasados a minúsculas;
     - todo lo demás separa palabras (espacios, puntuación, `¡ ¿`, comillas, etc.).
   - Ojo: en UTF-8, las letras con tilde y la ñ ocupan 2 bytes, así que `isalpha()` y `tolower()` no
     funcionan bien con ellas. El programa `programs/conteo/src/conteo.cpp` ya decodifica UTF-8 y
     puedes adaptar esa parte.
3. Agregar el conteo al índice compartido **dentro de la sección crítica**:

   ```cpp
   sem_wait(&indice.semaforo);                         // inicio de la sección crítica
   for (const auto &par : conteo) {                    // par.first = palabra, par.second = cantidad
       indice.palabras[par.first][libro.id] = par.second;
   }
   sem_post(&indice.semaforo);                         // fin de la sección crítica
   ```

4. `return true;`

**Por qué así** (te lo pueden preguntar en la revisión):

- `indice.palabras` es compartido por todos los threads. Si dos threads lo modifican a la vez, se corrompe.
  Por eso se modifica entre `sem_wait` y `sem_post`.
- El conteo se hace **antes**, fuera de la sección crítica. Si estuviera adentro, los threads
  trabajarían de a uno y no habría paralelismo.

## 4. `escribirIndice()`

Se llama **una sola vez**, cuando todos los threads ya terminaron. Por eso **no necesita semáforo**:
nadie más está usando el índice.

1. Abrir `archivo` con `std::ofstream`. Si no se abre, mostrar un error y `return false;`.
   El nombre viene de `INDICE_FILE`: no lo escribas en el código.
2. Escribir una línea por palabra, con este formato:

   ```
   computador;(1,15);(3,8);(7,21)
   ```

   Como las palabras y los IDs están guardados en `std::map`, ya salen ordenados. Basta con recorrer
   los dos mapas con dos `for`.
3. `return true;`

## 5. Reglas importantes

- **Nunca hacer `return` entre `sem_wait` y `sem_post`.** Si el semáforo queda tomado, los demás
  threads se bloquean para siempre (deadlock).
- **No usar `std::cout` dentro de `procesarLibro()`.** La pantalla la maneja `main` con su propio
  semáforo. Si algo falla, solo retorna `false`.
- **No usar variables globales o `static` propias en `indice.cpp`.** Serían compartidas entre threads
  y quedarían sin protección.

## 6. Cómo verificar

1. **Ejemplo pequeño.** Crea la carpeta `data/LIBROS_PRUEBA` con tres archivos:

   | Archivo | Contenido |
   |---|---|
   | `a_hola.txt` | `Hola mundo. Hola, HOLA!` |
   | `b_canciones.txt` | `La canción del niño. CANCIÓN, canción... ¡Niño!` |
   | `c_mundo.txt` | `mundo mundo la LA hola` |

   Pon `LIBROS_DIR=data/LIBROS_PRUEBA` en el `.env` y ejecuta `./bin/indice_invertido`. El índice debe quedar así:

   ```
   canción;(2,3)
   del;(2,1)
   hola;(1,3);(3,1)
   la;(2,1);(3,2)
   mundo;(1,1);(3,2)
   niño;(2,2)
   ```

2. **Mismo resultado con distinta cantidad de threads.** Con la carpeta LIBROS completa, ejecuta con
   `N_THREADS=1`, guarda una copia del índice, ejecuta con `N_THREADS=8` y compara las dos versiones
   con `cmp`. Deben ser idénticas. Si no lo son, hay una condición de carrera.
3. **Resumen final.** Debe decir `Libros procesados: N de N` e `Indice invertido guardado en ...`.

## 7. README

Completa en la sección 8 del README las partes marcadas **(PARTE 2: completar)**:

- **8.2**: el paso "resultados → índice invertido".
- **8.5**: la definición de palabra que usaste y un extracto real del índice.

## 8. Checklist de la pauta

- [ ] 1.5 (pasos 2, 3 y 4): cada thread procesa su libro, cuenta las palabras y las agrega al índice.
- [ ] 1.7: el semáforo del índice se usa en la sección crítica.
- [ ] 1.8: cantidad de veces que aparece cada palabra en cada libro.
- [ ] 1.10: un solo índice, con cada palabra una vez y todos sus libros, guardado en `INDICE_FILE`.
- [ ] 4.2: README completo.

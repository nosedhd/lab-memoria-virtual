# Reporte de análisis: simulador de memoria virtual

## 1. Estructuras de datos

### Dirección virtual

Una dirección de 32 bits se divide en tres campos. Con páginas de 4 KB:

```text
┌──────────────┬──────────────┬────────────────┐
│ PT1 (10 bits)│ PT2 (10 bits)│ Offset (12 bits)│
└──────────────┴──────────────┴────────────────┘
```

`VirtualAddress` calcula los tres campos y el VPN (número de página virtual,
que son PT1 y PT2 juntos). Los tamaños de cada campo salen de `MemoryConfig`:
si el tamaño de página cambia, cambian los bits de offset y los bits restantes
se reparten entre los dos niveles,

### Tabla de páginas de dos niveles

- **`DirectoryTable` (nivel 1):** un vector de 1024 entradas. Cada entrada
  indica si existe su tabla de nivel 2, y las tablas se guardan en un vector
  de `std::unique_ptr<PageTable>`. Una tabla de nivel 2 se crea solo cuando se
  reserva memoria en su zona; las zonas que nunca se usan no ocupan espacio.
- **`PageTable` (nivel 2):** un vector de 1024 `PageTableEntry`, creadas todas
  juntas al construir la tabla.
- **`PageTableEntry`:** número de marco físico (PFN) y cuatro bits: `valid`
  (la página está en RAM), `accessed`, `dirty` y `allocated` (la página fue
  reservada con `alloc`).

### Memoria física y marcos

- **`PhysicalMemory`:** un vector de bytes del tamaño de la memoria física
  (256 KB por defecto). La dirección física es `PFN × tamaño de página + offset`.
- **`FrameTable`:** por cada marco guarda si está ocupado y qué página lo
  ocupa (VPN e índices de las tablas), además de una cola con los marcos
  libres. La información de la página dueña permite encontrar la PTE de una
  víctima para invalidarla.

### TLB

16 entradas VPN → marco, en un arreglo de tamaño fijo. Si la TLB está llena,
reemplaza la entrada usada hace más tiempo (LRU). Se invalida la entrada de
una página cuando esa página es expulsada o liberada, para que nunca traduzca
a un marco que ya pertenece a otra página.

### Política de reemplazo

`FifoPolicy` guarda los marcos en una `std::queue`, en el orden en que se
cargaron: el frente es el marco que lleva más tiempo en memoria. Junto a la
cola hay un `std::unordered_set` con los marcos que están en ella, porque
`std::queue` no permite buscar un elemento; así se detecta si alguien intenta
cargar dos veces el mismo marco o liberar uno que no está.

## 2. Política de reemplazo: FIFO

FIFO (primero en entrar, primero en salir) expulsa la página que lleva más
tiempo cargada en memoria, sin importar si se está usando o no.

La implementación reacciona a los eventos de la interfaz `IReplacementPolicy`:

| Evento | Acción de FIFO |
|---|---|
| `onLoad(marco)` | Agrega el marco al final de la lista |
| `onAccess(marco)` | Nada: FIFO no considera el uso |
| `onFree(marco)` | Quita el marco de la lista |
| `selectVictim()` | Saca y devuelve el marco del frente |

Cuando ocurre un fallo de página y no hay marcos libres, el
`PageFaultHandler` pide la víctima a la política, invalida la PTE de la página
que ocupaba ese marco y su entrada en la TLB, limpia el marco y carga la página
nueva.

**Ventajas:** es simple, determinista y barata. Cada operación es O(1)
(excepto `onFree`, que recorre la lista) y no hace ningún trabajo en los
accesos, que son la operación más frecuente.

**Desventaja:** no tiene en cuenta la localidad. Una página muy usada puede
ser expulsada solo porque llegó temprano.

## 3. Resultados

Todas las ejecuciones usan la política FIFO. La configuración por defecto es
de páginas de 4 KB y 256 KB de memoria física (64 marcos).

| Programa | Página | Memoria (marcos) | Accesos | Fallos | Hit rate | Reemplazos | Hits de TLB |
|---|---|---|---|---|---|---|---|
| `test1_basico` | 4 KB | 256 KB (64) | 4 | 2 | 50.00% | 0 | 2 |
| `test2_fallos` | 4 KB | 256 KB (64) | 400 | 4 | 99.00% | 0 | 396 |
| `test3_reemplazo` | 4 KB | 256 KB (64) | 210 | 210 | 0.00% | 146 | 0 |
| `test3_reemplazo` | 4 KB | 512 KB (128) | 210 | 70 | 66.67% | 0 | 0 |
| `test2_fallos` | 8 KB | 256 KB (32) | 400 | 2 | 99.50% | 0 | 398 |
| `test3_reemplazo` | 8 KB | 512 KB (64) | 210 | 35 | 83.33% | 0 | 105 |

Los resultados se reproducen con:

```bash
./simulador tests/test1_basico.txt
./simulador tests/test2_fallos.txt
./simulador tests/test3_reemplazo.txt
./simulador tests/test3_reemplazo.txt --memory 524288
./simulador tests/test2_fallos.txt --page-size 8192
./simulador tests/test3_reemplazo.txt --page-size 8192 --memory 524288
```

### Programa 1: ejemplo del enunciado

`alloc 8192 write 0 42 write 4096 99 read 0 read 4096`

Las dos escrituras tocan por primera vez las páginas 0 y 1, así que cada una
produce un fallo de página obligatorio (la página está reservada pero nunca se
cargó). Hay marcos libres, así que no hay reemplazos. Las dos lecturas
encuentran su traducción en la TLB, que se llenó al resolver los fallos.
Resultado: 4 accesos, 2 fallos y 50% de hit rate, como indica el enunciado.

### Programa 2: localidad

Reserva 4 páginas y ejecuta 50 iteraciones de escritura y lectura sobre esas
mismas 4 páginas (400 accesos). Solo el primer acceso a cada página falla. Como
las 4 páginas caben en la TLB, los 396 accesos restantes se resuelven sin
recorrer las tablas.

Es el comportamiento típico de un programa con buena localidad: su conjunto de
trabajo (4 páginas) es mucho menor que la memoria disponible (64 marcos), así
que los fallos son solo los obligatorios y el hit rate tiende a 100% a medida
que el programa hace más accesos.

### Programa 3: recorrido cíclico

Reserva 70 páginas y las lee en orden, tres veces (0, 1, ..., 69, 0, 1, ...).
Con 64 marcos, las primeras 64 lecturas llenan la memoria; la página 64 expulsa
a la página 0, la 65 a la 1, y así hasta la 69, que expulsa a la 5. Al volver a
empezar, la página 0 ya no está, y al cargarla FIFO expulsa a la página 6 (la
más antigua en memoria). Las páginas 1 a 5 tampoco están y, al cargarse,
expulsan a las páginas 7 a 11; cuando llega el turno de la página 6, ya fue
expulsada. El patrón se repite en cada vuelta: la página más antigua siempre es
una de las próximas que se van a leer, y **todos los accesos son fallos** (0%
de hit rate). Los reemplazos son
210 − 64 = 146: todos los fallos excepto los que llenaron los marcos libres.

La TLB sufre el mismo problema: tiene 16 entradas con LRU y el ciclo es de 70
páginas, así que nunca encuentra una traducción.

## 4. Análisis

### Efecto de la memoria física

En el programa 3, duplicar la memoria (de 64 a 128 marcos) cambia el hit rate
de 0% a 66.67%. Con 128 marcos, las 70 páginas caben completas: solo fallan
los 70 accesos de la primera vuelta y no hay reemplazos. El salto es abrupto
porque el comportamiento depende de si el conjunto de trabajo cabe o no en
memoria: con 64 marcos faltan solo 6 para que quepa, y aun así el resultado es
el peor posible.

Este programa también muestra que **los reemplazos aparecen solo cuando el
conjunto de trabajo supera la memoria**. En los programas 1 y 2, y en el 3 con
128 marcos, hay 0 reemplazos.

### Efecto del tamaño de página

Con páginas de 8 KB, cada página cubre el doble de direcciones:

- En el programa 2, las 4 páginas de 4 KB pasan a ser 2 de 8 KB. Los fallos
  obligatorios bajan de 4 a 2.
- En el programa 3 (con 512 KB), los 70 bloques de 4 KB caben en 35 páginas de
  8 KB. Cada página contiene dos de las direcciones que se leen, así que la
  segunda lectura de cada par encuentra la traducción en la TLB (105 hits) y
  los fallos bajan de 70 a 35.

La contraparte de las páginas grandes es la fragmentación interna: un `alloc`
pequeño ocupa una página completa, y con la misma memoria física hay menos
marcos (32 en lugar de 64 con 256 KB).

### El hit rate como indicador

El hit rate resume en un número cuánto se ajusta el programa a la memoria
disponible, pero depende del patrón de acceso tanto como de la política. Con
el mismo algoritmo (FIFO) y la misma memoria, el programa 2 obtiene 99% y el
programa 3 obtiene 0%. Por eso las políticas se comparan sobre el mismo
patrón de accesos.

## 5. Comparación teórica con LRU

LRU (menos usada recientemente) expulsa la página cuyo último acceso ocurrió
hace más tiempo. A diferencia de FIFO, usa el historial de accesos: una página
muy usada se mantiene en memoria aunque haya llegado temprano.

### En los programas de prueba

- **Programas 1 y 2:** no hay reemplazos, así que LRU daría exactamente los
  mismos resultados. La política solo importa cuando la memoria se llena.
- **Programa 3:** LRU también daría 0% de hit rate. En un recorrido cíclico,
  cada página se usa una vez por vuelta, así que la usada hace más tiempo es
  también la cargada hace más tiempo: LRU elige la misma víctima que FIFO. Es el peor caso
  conocido para las dos políticas. Como referencia, el algoritmo óptimo
  (expulsar la página que se usará más tarde) tendría 82 fallos en este
  programa, es decir, 60.95% de hit rate.

### Un caso donde LRU es mejor

Con 3 marcos y la secuencia de páginas `1 2 3 1 4 1 5 1 6 1`, donde la página
1 se usa constantemente:

| Política | Fallos | Qué pasa |
|---|---|---|
| FIFO | 7 | Al llegar la página 4, expulsa la página 1 por ser la más antigua, aunque se acaba de usar. El acceso siguiente a 1 vuelve a fallar. |
| LRU | 6 | La página 1 siempre es la usada más recientemente, así que nunca es la víctima. |

Este patrón (unas pocas páginas muy usadas mezcladas con otras que se usan una
sola vez) es común en programas reales, y es donde LRU aprovecha la localidad
temporal.

### Costo de implementación

| Aspecto | FIFO | LRU |
|---|---|---|
| Trabajo en cada acceso | Ninguno | Actualizar el orden (`onAccess`) |
| Estructura | Una cola de marcos | Lista ordenada por último uso, o una marca de tiempo por marco |
| Elegir víctima | O(1) | O(1) con lista y mapa; O(n) buscando la marca de tiempo mínima |
| Hit rate | Menor cuando hay localidad | Mayor cuando hay localidad |

El costo de LRU está en que el trabajo ocurre en **cada acceso**, no solo en
los fallos. Por eso el hardware real usa aproximaciones, como el algoritmo del
reloj (segunda oportunidad), que usa el bit `accessed` de la PTE en lugar de
un orden exacto. El simulador ya tiene todo lo necesario para implementar LRU
sin cambiar el resto del código: la interfaz `IReplacementPolicy` incluye el
evento `onAccess`, y el reloj lógico (`Tick`) puede dar la marca de tiempo de
cada acceso.

#ifndef ARBOLES_H
#define ARBOLES_H

// Definimos un struct para los articulos con el formato que decidimos en clase
typedef struct articulo
{
    char nombre[21];
    char apellidos[41];
    char titulo[161];
    char ruta[201];
    int ano;
    char resumen[1025];
} articulo;

// Enum que sirve para determinar cual forma de las que usan letras vamos a hacer el arbol
typedef enum orden_letras
{
    TITULO,
    APELLIDOS,
    RUTA
} orden_letras;

// Arbol heap (como hicimos un struct del articulo, nada más sorteamos por eso lsito)
typedef struct arbol_numeros
{
    articulo **datos;
    int tamano;    // elementos actuales
    int capacidad; // max
} arbol_numeros;

// Arbol heap (como hicimos un struct del articulo, nada más sorteamos por eso lsito)
typedef struct arbol_letras
{
    articulo **datos;
    int tamano;         // elementos actuales
    int capacidad;      // max
    orden_letras orden; // según qué se ordena el arbol
} arbol_letras;

// Como es un heap, podemos usar nada más el indice para sacar padre e hijos
int padre(int i);
int hijo_izq(int i);
int hijo_der(int i);

// Esta funcion es para hacer un swap entre articulos, lo vamos a usar para acomodar luego
// Funciona nada más con un puntero temporal
void swap_articulo(articulo **a, articulo **b);

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
arbol_numeros *arbol_numeros_crear(int capacidad_inicial);

// Funcion para poner numero en el arbol y que se acomode
// nota: como estamos implementando un max heap, los grandes van arriba
void acomodar_numero(arbol_numeros *arbol, int i);

// Funcion para insertar el articulo en el arbol
int arbol_numeros_insertar(arbol_numeros *arbol, articulo *art);

// Con esta funcion nosotros obtenemos el texto que se va a usar como clave
char *obtener_clave_texto(articulo *a, orden_letras tipo);

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
arbol_letras *arbol_letras_crear(int capacidad_inicial, orden_letras tipo);

// Funcion para poner letras en el arbol y que se acomoden
// nota: como estamos implementando un max heap, los grandes van arriba
void acomodar_letras(arbol_letras *arbol, int i);

// Funcion para insertar el articulo en el arbol
int arbol_letras_insertar(arbol_letras *arbol, articulo *art);

// Enum para los criterios de ordenamiento
typedef enum criterio_orden
{
    TITULO_ALFABETICO,
    TAMANO_TITULO,
    NOMBRE_ARCHIVO,
    ANO_PUBLICACION
} criterio_orden;

#endif

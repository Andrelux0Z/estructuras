/*
* Este archivo tiene toda la lógica para el árbol heap. Funciona con todo, porque
* hicimos un struct de articulo
*/

// Definimos un struct para los articulos con el formato que decidimos en clase
struct articulo {
    char nombre[21];
    char apellidos[41];
    char titulo[71];
    char ruta[51];
    int ano;
    char resumen[81];
};

// Arbol heap (como hicimos un struct del articulo, nada más sorteamos por eso lsito)
struct arbol_numeros {
    struct articulo** datos;
    int tamano; //elementos acutales
    int capacidad; //max
};

// Como es un heap, podemos usar nada más el indice para sacar padre e hijos
int padre(int i) {
    return (i - 1) / 2;
}

int hijo_izq(int i) {
    return 2*i + 1;
}

int hijo_der(int i) {
    return 2*i + 2;
}

// Esta funcion es para hacer un swap entre articulos, lo vamos a usar para acomodar lueg
// Funciona nada más con un puntero temporal
void swap_articulo(struct articulo** a, struct articulo** b) {
    struct articulo* temp = *a;
    *a = *b;
    *b = temp;
}

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
struct arbol_numeros* arbol_numeros_crear(int capacidad_inicial) {
    struct arbol_numeros* arbol = calloc(1, sizeof(struct arbol_numeros));

    arbol->datos = calloc(1, sizeof(struct articulo*) * capacidad_inicial);
    arbol->tamano = 0;
    arbol->capacidad = capacidad_inicial;
    return arbol;
}


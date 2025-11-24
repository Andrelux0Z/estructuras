/*
* Este archivo tiene toda la lógica para el árbol heap. En realidad hicimos 2 arboles
* uno para los numeros y otra para letras
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

// Funcion para poner numero en el arbol y que se acomode
// nota: como estamos implementando un max heap, los grandes van arriba
void acomodar_numero(struct arbol_numeros* arbol, int i) {
    while (i > 0) {
        int p = padre(i);

        if (arbol->datos[i]->ano > arbol->datos[p]->ano) {
            swap_articulo(&arbol->datos[i], &arbol->datos[p]);
            i = p;
        } else {
        }
    }
}


// Funcion para insertar el articulo en el arbol
int arbol_numeros_insertar(struct arbol_numeros* arbol, struct articulo *art) {
    if (arbol->tamano == arbol->capacidad) {
        // esto es una verificacion por si ya no hay espacio

        int nuevo = arbol->capacidad * 2;
        struct articulo** temp = realloc(arbol->datos, sizeof(struct articulo*) * nuevo);

        arbol->datos = temp;
        arbol->capacidad = nuevo;
    }

    arbol->datos[arbol->tamano] = art;
    acomodar_numero(arbol, arbol->tamano);
    arbol->tamano++;

    return 1; // todo bien
}

// Enum que sirve para determinar cual forma de las que usan letras vamos a hacer el arbol
enum orden_letras{
    TITULO,
    APELLIDOS,
    RUTA
};

// Arbol heap (como hicimos un struct del articulo, nada más sorteamos por eso lsito)
struct arbol_letras {
    struct articulo** datos;
    int tamano; //elementos acutales
    int capacidad; //max
    enum orden_letras orden; // segun que se ordena el arbol
};

// Con esta funcion nosotros obtenemos el texto que se va a usar como clave
char* obtener_clave_texto(struct articulo* a, enum orden_letras tipo) {
    switch (tipo) {
        case TITULO:    
            return a->titulo;
        case APELLIDOS: 
            return a->apellidos;
        case RUTA:      
            return a->ruta;
        default:              
            return a->titulo;
    }
}


// Esta funcion es para hacer un swap entre articulos, lo vamos a usar para acomodar lueg
// Funciona nada más con un puntero temporal
void swap_articulo(struct articulo** a, struct articulo** b) {
    struct articulo* temp = *a;
    *a = *b;
    *b = temp;
}

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
struct arbol_letras* arbol_letras_crear(int capacidad_inicial, enum orden_letras tipo) {
    struct arbol_letras* arbol = calloc(1, sizeof(struct arbol_letras));

    arbol->datos = calloc(sizeof(struct articulo*) * capacidad_inicial);
    arbol->tamano = 0;
    arbol->capacidad = capacidad_inicial;
    arbol->orden = tipo;
    return arbol;
}

// Funcion para poner letras en el arbol y que se acomoden
// nota: como estamos implementando un max heap, los grandes van arriba
void acomodar_letras(struct arbol_letras* arbol, int i) {
    while (i > 0) {
        int p = padre(i);
        char* hijo = obtener_clave_texto(arbol->datos[i], arbol->orden);
        char* padre = obtener_clave_texto(arbol->datos[p], arbol->orden);

        if (strcmp(hijo, padre) > 0) {
            swap_articulo(&arbol->datos[i], &arbol->datos[p]);
            i = p;
        } else {
            break;
        }
    }
}

// Funcion para insertar el articulo en el arbol
int arbol_letras_insertar(struct arbol_letras* arbol, struct articulo *art) {
    if (arbol->tamano == arbol->capacidad) {
        int nueva = arbol->capacidad * 2;
        struct articulo** temp = realloc(arbol->datos, sizeof(struct articulo*) * nueva);
        
        arbol->datos = temp;
        arbol->capacidad = nueva;
    }

    arbol->datos[arbol->tamano] = art;
    acomodar_letras(arbol, arbol->tamano);
    arbol->tamano++;

    return 1;
}


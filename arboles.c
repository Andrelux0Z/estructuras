/*
* Este archivo tiene toda la lógica para el árbol heap. En realidad hicimos 2 arboles
* uno para los numeros y otra para letras
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "arboles.h"

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
void swap_articulo(articulo** a, articulo** b) {
    articulo* temp = *a;
    *a = *b;
    *b = temp;
}

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
arbol_numeros* arbol_numeros_crear(int capacidad_inicial) {
    arbol_numeros* arbol = calloc(1, sizeof(arbol_numeros));
    if (!arbol) {
        return NULL;
    }

    arbol->datos = calloc(1, sizeof(articulo*) * capacidad_inicial);
    if (!arbol->datos) {
        free(arbol);
        return NULL;
    }
    arbol->tamano = 0;
    arbol->capacidad = capacidad_inicial;
    return arbol;
}

// Funcion para poner numero en el arbol y que se acomode
// nota: usamos min-heap, los mas chicos van arriba
void acomodar_numero(arbol_numeros* arbol, int i) {
    while (i > 0) {
        int p = padre(i);

        if (arbol->datos[i]->ano < arbol->datos[p]->ano) {
            swap_articulo(&arbol->datos[i], &arbol->datos[p]);
            i = p;
        } else {
            break;
        }
    }
}


// Funcion para insertar el articulo en el arbol
int arbol_numeros_insertar(arbol_numeros* arbol, articulo *art) {
    if (arbol->tamano == arbol->capacidad) {
        // esto es una verificacion por si ya no hay espacio

        int nuevo = arbol->capacidad * 2;
        articulo** temp = realloc(arbol->datos, sizeof(articulo*) * nuevo);
        if (!temp) {
            return 0;
        }
        arbol->datos = temp;
        arbol->capacidad = nuevo;
    }

    arbol->datos[arbol->tamano] = art;
    acomodar_numero(arbol, arbol->tamano);
    arbol->tamano++;

    return 1; // todo bien
}

// Con esta funcion nosotros obtenemos el texto que se va a usar como clave
char* obtener_clave_texto(articulo* a, criterio_orden criterio) {
    switch (criterio) {
        case NOMBRE_ARCHIVO:
            return a->ruta;
        case TITULO_ALFABETICO:
        default:
            return a->titulo;
    }
}

// Esta funcion crea los arboles, recibimos capaciddad maxima inicial y retornamos el arbol
arbol_letras* arbol_letras_crear(int capacidad_inicial, criterio_orden criterio) {
    arbol_letras* arbol = calloc(1, sizeof(arbol_letras));
    if (!arbol) {
        return NULL;
    }

    arbol->datos = calloc(capacidad_inicial, sizeof(articulo*));
    if (!arbol->datos) {
        free(arbol);
        return NULL;
    }

    arbol->tamano = 0;
    arbol->capacidad = capacidad_inicial;
    arbol->criterio = criterio;
    return arbol;
}

// Funcion para poner letras en el arbol y que se acomoden
// nota: min-heap, alfabeticamente los primeros van arriba
void acomodar_letras(arbol_letras* arbol, int i) {
    while (i > 0) {
        int p = padre(i);
        char* hijo = obtener_clave_texto(arbol->datos[i], arbol->criterio);
        char* padre = obtener_clave_texto(arbol->datos[p], arbol->criterio);

        if (strcmp(hijo, padre) < 0) {
            swap_articulo(&arbol->datos[i], &arbol->datos[p]);
            i = p;
        } else {
            break;
        }
    }
}

// Funcion para insertar el articulo en el arbol
int arbol_letras_insertar(arbol_letras* arbol, articulo *art) {
    if (arbol->tamano == arbol->capacidad) {
        int nueva = arbol->capacidad * 2;
        articulo** temp = realloc(arbol->datos, sizeof(articulo*) * nueva);
        if (!temp) {
            return 0;
        }
        
        arbol->datos = temp;
        arbol->capacidad = nueva;
    }

    arbol->datos[arbol->tamano] = art;
    acomodar_letras(arbol, arbol->tamano);
    arbol->tamano++;

    return 1;
}

// Reacomoda hacia abajo en el heap numerico para mantener la propiedad
void acomodar_abajo_nums(arbol_numeros* arbol, int i) {
    while (1) {
        int mayor = i;
        int izq = hijo_izq(i);
        int der = hijo_der(i);

        if (izq < arbol->tamano && arbol->datos[izq]->ano < arbol->datos[mayor]->ano) {
            mayor = izq;
        }
        if (der < arbol->tamano && arbol->datos[der]->ano < arbol->datos[mayor]->ano) {
            mayor = der;
        }

        if (mayor == i) {
            break;
        }

        swap_articulo(&arbol->datos[i], &arbol->datos[mayor]);
        i = mayor;
    }
}

// Reacomoda hacia abajo en el heap de texto
void acomodar_abajo_letras(arbol_letras* arbol, int i) {
    while (1) {
        int mayor = i;
        int izq = hijo_izq(i);
        int der = hijo_der(i);

        if (izq < arbol->tamano) {
            char* hijo = obtener_clave_texto(arbol->datos[izq], arbol->criterio);
            char* padre = obtener_clave_texto(arbol->datos[mayor], arbol->criterio);
            if (strcmp(hijo, padre) < 0) {
                mayor = izq;
            }
        }

        if (der < arbol->tamano) {
            char* hijo = obtener_clave_texto(arbol->datos[der], arbol->criterio);
            char* padre = obtener_clave_texto(arbol->datos[mayor], arbol->criterio);
            if (strcmp(hijo, padre) < 0) {
                mayor = der;
            }
        }

        if (mayor == i) {
            break;
        }

        swap_articulo(&arbol->datos[i], &arbol->datos[mayor]);
        i = mayor;
    }
}

// Quita el elemento mas chico del heap numerico
articulo* arbol_numeros_extraer_max(arbol_numeros* arbol) {
    if (!arbol || arbol->tamano == 0) {
        return NULL;
    }

    articulo* max = arbol->datos[0];
    arbol->tamano--;

    if (arbol->tamano > 0) {
        arbol->datos[0] = arbol->datos[arbol->tamano];
        acomodar_abajo_nums(arbol, 0);
    }

    return max;
}

// Quita el elemento mas chico del heap de texto
articulo* arbol_letras_extraer_max(arbol_letras* arbol) {
    if (!arbol || arbol->tamano == 0) {
        return NULL;
    }

    articulo* max = arbol->datos[0];
    arbol->tamano--;

    if (arbol->tamano > 0) {
        arbol->datos[0] = arbol->datos[arbol->tamano];
        acomodar_abajo_letras(arbol, 0);
    }

    return max;
}

// Libera la memoria del heap sin tocar los articulos almacenados
void arbol_numeros_destruir(arbol_numeros* arbol) {
    if (!arbol) {
        return;
    }
    free(arbol->datos);
    free(arbol);
}

// Libera la memoria del heap sin tocar los articulos almacenados
void arbol_letras_destruir(arbol_letras* arbol) {
    if (!arbol) {
        return;
    }
    free(arbol->datos);
    free(arbol);
}

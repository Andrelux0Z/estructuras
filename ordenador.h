#ifndef ORDENADOR_H
#define ORDENADOR_H

#include "arboles.h"

// Extrae el campo n de una linea delimitada por '|'
// Retorna un puntero al campo extraido (debe ser liberado por el caller)
char *obtener_campo(const char *linea, int n);

// Funcion para contar palabras en el titulo
int contar_palabras(const char *texto);

// Funcion de comparacion generica segun criterio
int comparar_articulos(articulo *a, articulo *b, criterio_orden criterio);

// Funcion heapify_down para mantener propiedad del heap
void heapify_down_generico(articulo **datos, int tamano, int i, criterio_orden criterio);

// Funcion para extraer el maximo del heap
articulo *extraer_maximo_heap(articulo **datos, int *tamano, criterio_orden criterio);

// Funcion para cargar todos los articulos del archivo
articulo **cargar_articulos(int *total_articulos);

// Funcion principal de ordenamiento
articulo **ordenar_articulos(criterio_orden criterio, int cantidad, int *total_encontrados);

// Funcion para liberar memoria de articulos
void liberar_articulos(articulo **articulos, int cantidad);

#endif

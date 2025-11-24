#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenador.h"
#include "arboles.h"

articulo **ordenar_con_heaps(criterio_orden criterio, int cantidad, int *total_encontrados);
articulo **extraer_desde_heap_num(arbol_numeros *heap, int cantidad, int *total_encontrados);
articulo **extraer_desde_heap_letras(arbol_letras *heap, int cantidad, int *total_encontrados);

// Muestra el menu principal y retorna la opcion seleccionada
int mostrar_menu()
{
    int opcion;

    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Abrir ordenador\n");
    printf("2. Ver informacion\n");
    printf("3. Cerrar programa\n");
    printf("Seleccione una opcion: ");

    if (scanf("%d", &opcion) == 0)
    {
        while (getchar() != '\n'); //VSCode me mueve esta coma para abajo cada vez que guardo
        return -1;
    }

    while (getchar() != '\n'); //sujetador de punto y coma
    return opcion;
}

// Muestra la informacion del programa
void mostrar_informacion(){
    printf("\n=== INFORMACION DE LA PROGRA ===\n");
    printf("La busqueda de Sisifo\n\n");

    printf("La idea de la progra es hacer una forma de buscar artículos sobre corrupcion en Latino America segun algunos datos.\n");
    printf("Cada articulo tiene un nombre (autor), apellidos (autor), título, ruta, año, resumen).\n");
 
    printf("\nFuncionalidad:\n");
    printf("Ordenar artículos por diferentes criterios (título, tamaño del título, nombre de archivo, año).\n");
    printf("\nAutor: Fabian Sanchez y Andres Padilla\n");
    printf("Estructuras de Datos - II Semestre 2025\n");
    printf("\nPresione Enter para continuar...");
    getchar();
}

// Llena un heap numerico y regresa los elementos ya ordenados
articulo** extraer_desde_heap_num(arbol_numeros* heap, int cantidad, int* total_encontrados) {
    if (!heap) {
        *total_encontrados = 0;
        return NULL;
    }

    if (cantidad > heap->tamano)
    {
        cantidad = heap->tamano;
    }

    articulo **resultado = calloc((size_t)cantidad, sizeof(articulo *));
    if (!resultado)
    {
        *total_encontrados = 0;
        arbol_numeros_destruir(heap);
        return NULL;
    }

    // Extraemos el menor primero para devolverlos en orden ascendente
    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = arbol_numeros_extraer_max(heap);
    }

    // Liberamos los que no se van a usar
    while (heap->tamano > 0)
    {
        articulo *sobrante = arbol_numeros_extraer_max(heap);
        free(sobrante);
    }

    arbol_numeros_destruir(heap);
    *total_encontrados = cantidad;
    return resultado;
}

// Llena un heap de texto y regresa los elementos ya ordenados
articulo **extraer_desde_heap_letras(arbol_letras *heap, int cantidad, int *total_encontrados)
{
    if (!heap)
    {
        *total_encontrados = 0;
        return NULL;
    }

    if (cantidad > heap->tamano)
    {
        cantidad = heap->tamano;
    }

    articulo **resultado = calloc((size_t)cantidad, sizeof(articulo *));
    if (!resultado)
    {
        *total_encontrados = 0;
        arbol_letras_destruir(heap);
        return NULL;
    }

    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = arbol_letras_extraer_max(heap);
    }

    while (heap->tamano > 0)
    {
        articulo *sobrante = arbol_letras_extraer_max(heap);
        free(sobrante);
    }

    arbol_letras_destruir(heap);
    *total_encontrados = cantidad;
    return resultado;
}

// Lee archivo.txt, crea el heap adecuado e inserta todos los articulos
articulo **ordenar_con_heaps(criterio_orden criterio, int cantidad, int *total_encontrados)
{
    int total_archivo = 0;
    articulo **todos = cargar_articulos(&total_archivo);
    if (!todos || total_archivo == 0)
    {
        *total_encontrados = 0;
        return NULL;
    }

    if (cantidad > total_archivo)
    {
        cantidad = total_archivo;
    }

    int fallo = 0;

    if (criterio == ANO_PUBLICACION)
    {
        arbol_numeros *heap = arbol_numeros_crear(total_archivo);
        if (!heap)
        {
            *total_encontrados = 0;
            for (int i = 0; i < total_archivo; i++)
            {
                free(todos[i]);
            }
            free(todos);
            return NULL;
        }

        for (int i = 0; i < total_archivo; i++)
        {
            if (!arbol_numeros_insertar(heap, todos[i]))
            {
                fallo = 1;
                break;
            }
        }

        free(todos); // solo el arreglo, los articulos quedan en el heap

        if (fallo)
        {
            while (heap->tamano > 0)
            {
                articulo *articulo_a_liberar = arbol_numeros_extraer_max(heap);
                free(articulo_a_liberar);
            }
            arbol_numeros_destruir(heap);
            *total_encontrados = 0;
            return NULL;
        }

        return extraer_desde_heap_num(heap, cantidad, total_encontrados);
    }
    else
    {
        criterio_orden tipo = (criterio == NOMBRE_ARCHIVO) ? NOMBRE_ARCHIVO : TITULO_ALFABETICO;
        arbol_letras *heap = arbol_letras_crear(total_archivo, tipo);
        if (!heap)
        {
            *total_encontrados = 0;
            for (int i = 0; i < total_archivo; i++)
            {
                free(todos[i]);
            }
            free(todos);
            return NULL;
        }

        for (int i = 0; i < total_archivo; i++)
        {
            if (!arbol_letras_insertar(heap, todos[i]))
            {
                fallo = 1;
                break;
            }
        }

        free(todos);

        if (fallo)
        {
            while (heap->tamano > 0)
            {
                articulo *articulo_a_liberar = arbol_letras_extraer_max(heap);
                free(articulo_a_liberar);
            }
            arbol_letras_destruir(heap);
            *total_encontrados = 0;
            return NULL;
        }

        return extraer_desde_heap_letras(heap, cantidad, total_encontrados);
    }
}

// Funcion para mostrar articulos ordenados
void mostrar_articulos(articulo **articulos, int cantidad)
{
    if (!articulos || cantidad == 0)
    {
        printf("\nNo hay articulos para mostrar.\n");
        return;
    }

    printf("\n========================================\n");
    printf("ARTICULOS ENCONTRADOS: %d\n", cantidad);
    printf("========================================\n\n");

    for (int i = 0; i < cantidad; i++)
    {
        if (articulos[i])
        {
            printf("--- Articulo %d ---\n", i + 1);
            printf("Titulo: %s\n", articulos[i]->titulo);
            printf("Ano: %d\n", articulos[i]->ano);

            // Quitar /repo/ del inicio de la ruta
            char *ruta_mostrar = articulos[i]->ruta;
            if (strncmp(ruta_mostrar, "/repo/", 6) == 0)
            {
                ruta_mostrar += 6;
            }
            else if (strncmp(ruta_mostrar, "/mnt/data/", 10) == 0)
            {
                ruta_mostrar += 10;
            }
            printf("Archivo: %s\n", ruta_mostrar);

            printf("Resumen: %s\n", articulos[i]->resumen);
            printf("\n");
        }
    }
}

// Funcion para ordenar y mostrar articulos
void ordenar_y_mostrar()
{
    int criterio_num, cantidad;
    criterio_orden criterio;

    printf("\n=== ORDENADOR DE ARTICULOS ===\n\n");

    // Solicitar criterio de ordenamiento
    printf("Seleccione el criterio de ordenamiento:\n");
    printf("1. Alfabeticamente por titulo\n");
    printf("2. Tamano del titulo (cantidad de palabras)\n");
    printf("3. Nombre del archivo\n");
    printf("4. Ano de publicacion\n");
    printf("Opcion: ");

    if (scanf("%d", &criterio_num) == 0)
    {
        while (getchar() != '\n')
            ;
        printf("Opcion invalida.\n");
        getchar();
        return;
    }
    while (getchar() != '\n')
        ;

    // Convertir a enum
    switch (criterio_num)
    {
    case 1:
        criterio = TITULO_ALFABETICO;
        break;
    case 2:
        criterio = TAMANO_TITULO;
        break;
    case 3:
        criterio = NOMBRE_ARCHIVO;
        break;
    case 4:
        criterio = ANO_PUBLICACION;
        break;
    default:
        printf("Opcion invalida.\n");
        getchar();
        return;
    }

    // Solicitar cantidad
    printf("\nCuantos articulos desea ver? ");
    if (scanf("%d", &cantidad) == 0 || cantidad <= 0)
    {
        while (getchar() != '\n')
            ;
        printf("Cantidad invalida.\n");
        getchar();
        return;
    }
    while (getchar() != '\n')
        ;

    // Realizar el ordenamiento
    printf("\nProcesando...\n");
    int total_encontrados;
    articulo **resultados = NULL;
    if (criterio == TAMANO_TITULO)
    {
        resultados = ordenar_articulos(criterio, cantidad, &total_encontrados);
    }
    else
    {
        resultados = ordenar_con_heaps(criterio, cantidad, &total_encontrados);
    }

    // Mostrar resultados
    mostrar_articulos(resultados, total_encontrados);

    // Liberar memoria
    liberar_articulos(resultados, total_encontrados);

    printf("\nPresione Enter para continuar...");
    getchar();
}

int main()
{
    int opcion;

    printf("Bienvenido al sistema de consulta\n");

    while (1)
    {
        opcion = mostrar_menu();

        switch (opcion)
        {
        case 1:
            ordenar_y_mostrar();
            break;
        case 2:
            mostrar_informacion();
            break;
        case 3:
            printf("\nCerrando programa...\n");
            return 0;
        default:
            printf("\nOpcion invalida. Intente nuevamente.\n");
            break;
        }
    }

    return 0;
}

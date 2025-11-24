#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenador.h"
#include "extractor.h"
#include "arboles.h"

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
        while (getchar() != '\n')
            ;
        return -1;
    }

    while (getchar() != '\n')
        ;
    return opcion;
}

// Muestra la informacion del programa
void mostrar_informacion()
{
    printf("\n=== INFORMACION ===\n");
    printf("Hola\n");
    printf("Fabian");
    getchar();
}

// Funcion para consultar datos
void consultar_datos()
{
    int numero_linea = 1;
    int numero_campo = 1;

    printf("\n=== ORDENADOR ===\n");
    printf("Consultando linea %d, campo %d...\n", numero_linea, numero_campo);

    char *linea = obtener_linea(numero_linea);

    char *campo = obtener_campo(linea, numero_campo);
    if (campo)
    {
        printf("\nResultado: %s\n", campo);
        free(campo);
    }

    free(linea);
    printf("\nPresione Enter para continuar...");
    getchar();
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
    articulo **resultados = ordenar_articulos(criterio, cantidad, &total_encontrados);

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

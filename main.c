#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenador.h"
#include "extractor.h"

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

// Funcion principal
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
            consultar_datos();
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

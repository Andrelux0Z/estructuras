#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenador.h"

// Extrae el campo n de una linea delimitada por '|'
// Retorna un puntero al campo extraido (debe ser liberado por el caller)
char *obtener_campo(const char *linea, int n)
{
    char *copia = strdup(linea);

    char *parte = strtok(copia, "|"); // Esto separa la linea en pedazos, reemplaza '|' por '\0'
    int contador = 1;

    while (contador < n)
    {
        parte = strtok(NULL, "|"); // Dentro de strtok hay un puntero a la ultima posición en la que estuvo
                                   // Cuando le paso NULL, sigue desde ahí
                                   // Entonces esto básicamente está recorriendo una lista de textos
        contador++;
    }

    char *resultado = strdup(parte);
    free(copia);

    return resultado;
}

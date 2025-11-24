#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extractor.h"

// Extrae la linea n del archivo archivo.txt
// Retorna un puntero a la linea extraida
// Creo que esto no va a servir después de que hagas el árbol
char *obtener_linea(int n)
{
    FILE *archivo = fopen("archivo.txt", "r");

    char buffer[1450]; // revisar esto
    int contador = 1;

    // En esta linea se extrae la primera linea y mueve un puntero
    // Cómo es un bucle el puntero se va moviendo hasta que llega a la que ocupamos
    while (fgets(buffer, sizeof(buffer), archivo)) {
        if (contador == n)
        {
            fclose(archivo);

            // Eliminar el salto de línea al final si hay salto de linea al final
            int len = strlen(buffer);
            if (buffer[len - 1] == '\n')
            {
                buffer[len - 1] = '\0';
            }

            return strdup(buffer);
        }
        contador++;
    }

    fclose(archivo);
    return NULL;
}

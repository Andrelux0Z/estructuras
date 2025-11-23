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

    char buffer[265]; // Es 264 por cómo Sebas acomodó los archivos
    int contador = 1;

    while (fgets(buffer, sizeof(buffer), archivo)) // En esta linea se extrae la primera linea y mueve un puntero
                                                   // Cómo es un bucle el puntero se va moviendo hasta que llega a la que ocupamos
    {
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

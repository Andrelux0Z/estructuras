#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ordenador.h"
#include "arboles.h"

// Convierte todo el texto a minusculas
void convertir_minusculas(char *texto) {
    if (!texto) {
        return;
    }

    for (char *p = texto; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
}

// Quita acentos y simbolos basicos para no tener caracteres raros al comparar
void normalizar_utf8(char *texto)
{
    if (!texto)
    {
        return;
    }

    size_t lectura = 0;
    size_t escritura = 0;

    while (texto[lectura] != '\0')
    {
        unsigned char c = (unsigned char)texto[lectura];

        if (c < 0x80)
        {
            if (c != '"')
            {
                texto[escritura++] = (char)c;
            }
            lectura++;
            continue;
        }

        if (c == 0xC3 && texto[lectura + 1] != '\0')
        {
            unsigned char siguiente = (unsigned char)texto[lectura + 1];
            char reemplazo = 0;
            switch (siguiente)
            {
            case 0x81: reemplazo = 'A'; break; // Á
            case 0x89: reemplazo = 'E'; break; // É
            case 0x8D: reemplazo = 'I'; break; // Í
            case 0x93: reemplazo = 'O'; break; // Ó
            case 0x9A: reemplazo = 'U'; break; // Ú
            case 0xA1: reemplazo = 'a'; break; // á
            case 0xA9: reemplazo = 'e'; break; // é
            case 0xAD: reemplazo = 'i'; break; // í
            case 0xB3: reemplazo = 'o'; break; // ó
            case 0xBA: reemplazo = 'u'; break; // ú
            case 0x91: reemplazo = 'N'; break; // Ñ
            case 0xB1: reemplazo = 'n'; break; // ñ
            case 0xBC: reemplazo = 'u'; break; // ü
            case 0x82: reemplazo = 'A'; break; // Â
            case 0xA2: reemplazo = 'a'; break; // â
            default: break;
            }

            if (reemplazo)
            {
                texto[escritura++] = reemplazo;
            }

            lectura += 2;
            continue;
        }

        if (c == 0xC2 && texto[lectura + 1] != '\0')
        {
            unsigned char siguiente = (unsigned char)texto[lectura + 1];
            char reemplazo = 0;
            switch (siguiente)
            {
            case 0xA1: reemplazo = '!'; break; // ¡
            case 0xBF: reemplazo = '?'; break; // ¿
            case 0xB0: reemplazo = 'o'; break; // º
            case 0xBA: reemplazo = 'o'; break; // º
            default: break;
            }

            if (reemplazo)
            {
                texto[escritura++] = reemplazo;
            }

            lectura += 2;
            continue;
        }

        if (c == 0xE2 && texto[lectura + 1] != '\0' && texto[lectura + 2] != '\0')
        {
            unsigned char siguiente = (unsigned char)texto[lectura + 1];
            unsigned char tercero = (unsigned char)texto[lectura + 2];

            if (siguiente == 0x80)
            {
                if (tercero == 0x99)
                {
                    texto[escritura++] = '\'';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0x9C || tercero == 0x9D)
                {
                    texto[escritura++] = '"';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0x93 || tercero == 0x94)
                {
                    texto[escritura++] = '-';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0xA6)
                {
                    texto[escritura++] = '.';
                    texto[escritura++] = '.';
                    texto[escritura++] = '.';
                    lectura += 3;
                    continue;
                }
            }

            lectura += 3;
            continue;
        }

        if ((c & 0xE0) == 0xC0 && texto[lectura + 1] != '\0')
        {
            lectura += 2;
            continue;
        }
        if ((c & 0xF0) == 0xE0 && texto[lectura + 2] != '\0')
        {
            lectura += 3;
            continue;
        }
        if ((c & 0xF8) == 0xF0 && texto[lectura + 3] != '\0')
        {
            lectura += 4;
            continue;
        }

        lectura++;
    }

    texto[escritura] = '\0';
    convertir_minusculas(texto);
}

// Quita espacios en blanco al inicio y final
void recortar_blancos(char *texto)
{
    if (!texto)
    {
        return;
    }

    char *inicio = texto;
    while (*inicio && isspace((unsigned char)*inicio))
    {
        inicio++;
    }

    char *fin = inicio + strlen(inicio);
    while (fin > inicio && isspace((unsigned char)fin[-1]))
    {
        fin--;
    }

    size_t nuevo_tamano = (size_t)(fin - inicio);
    if (inicio != texto && nuevo_tamano > 0)
    {
        memmove(texto, inicio, nuevo_tamano);
    }
    else if (inicio != texto && nuevo_tamano == 0)
    {
        texto[0] = '\0';
        return;
    }

    texto[nuevo_tamano] = '\0';
}

// Lee una linea de tamano variable
char *leer_linea(FILE *archivo)
{
    size_t capacidad = 256;
    size_t longitud = 0;
    char *buffer = malloc(capacidad);
    if (!buffer)
    {
        return NULL;
    }

    int c = 0;
    while ((c = fgetc(archivo)) != EOF)
    {
        if (c == '\r')
        {
            continue;
        }
        if (c == '\n')
        {
            break;
        }

        if (longitud + 1 >= capacidad)
        {
            size_t nueva_capacidad = capacidad * 2;
            char *temporal = realloc(buffer, nueva_capacidad);
            if (!temporal)
            {
                free(buffer);
                return NULL;
            }
            buffer = temporal;
            capacidad = nueva_capacidad;
        }

        buffer[longitud++] = (char)c;
    }

    if (c == EOF && longitud == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[longitud] = '\0';
    return buffer;
}

// Extrae el campo n de una linea delimitada por '|'
char *obtener_campo(const char *linea, int n)
{
    char *copia = strdup(linea);
    if (!copia)
    {
        return NULL;
    }

    char *parte = strtok(copia, "|");
    int contador = 1;

    while (contador < n && parte)
    {
        parte = strtok(NULL, "|");
        contador++;
    }

    if (!parte)
    {
        free(copia);
        return NULL;
    }

    char *resultado = strdup(parte);
    free(copia);

    return resultado;
}

// Cuenta palabras separadas por espacios
int contar_palabras(const char *texto)
{
    if (!texto || texto[0] == '\0')
        return 0;

    int contador = 0;
    int en_palabra = 0;

    for (int i = 0; texto[i] != '\0'; i++)
    {
        if (texto[i] != ' ' && texto[i] != '\t' && texto[i] != '\n')
        {
            if (!en_palabra)
            {
                contador++;
                en_palabra = 1;
            }
        }
        else
        {
            en_palabra = 0;
        }
    }

    return contador;
}

// Lee todos los articulos del archivo de texto
articulo **cargar_articulos(int *total_articulos)
{
    if (!total_articulos)
    {
        return NULL;
    }

    *total_articulos = 0;

    FILE *archivo = fopen("archivo.txt", "r");
    if (!archivo)
    {
        return NULL;
    }

    int capacidad = 32;
    articulo **articulos = calloc((size_t)capacidad, sizeof(articulo *));
    if (!articulos)
    {
        fclose(archivo);
        return NULL;
    }

    int cantidad = 0;
    int error_memoria = 0;
    char *linea = NULL;

    while ((linea = leer_linea(archivo)) != NULL)
    {
        normalizar_utf8(linea);
        recortar_blancos(linea);

        if (linea[0] == '\0')
        {
            free(linea);
            continue;
        }

        articulo *art = calloc(1, sizeof(articulo));
        if (!art)
        {
            free(linea);
            error_memoria = 1;
            break;
        }

        int campo = 0;
        char *token = strtok(linea, "|");
        while (token && campo < 6)
        {
            recortar_blancos(token);
            switch (campo)
            {
            case 0:
                strncpy(art->nombre, token, sizeof(art->nombre) - 1);
                break;
            case 1:
                strncpy(art->apellidos, token, sizeof(art->apellidos) - 1);
                break;
            case 2:
                strncpy(art->titulo, token, sizeof(art->titulo) - 1);
                break;
            case 3:
                strncpy(art->ruta, token, sizeof(art->ruta) - 1);
                break;
            case 4:
                art->ano = atoi(token);
                break;
            case 5:
                strncpy(art->resumen, token, sizeof(art->resumen) - 1);
                break;
            }
            campo++;
            token = strtok(NULL, "|");
        }

        free(linea);
        linea = NULL;

        if (campo < 6)
        {
            free(art);
            continue;
        }

        if (cantidad >= capacidad)
        {
            int nueva_capacidad = capacidad * 2;
            articulo **temporal = realloc(articulos, (size_t)nueva_capacidad * sizeof(articulo *));
            if (!temporal)
            {
                free(art);
                error_memoria = 1;
                break;
            }
            articulos = temporal;
            capacidad = nueva_capacidad;
        }

        articulos[cantidad++] = art;
    }

    free(linea);
    fclose(archivo);

    if (error_memoria)
    {
        for (int i = 0; i < cantidad; i++)
        {
            free(articulos[i]);
        }
        free(articulos);
        return NULL;
    }

    if (cantidad == 0)
    {
        free(articulos);
        return NULL;
    }

    articulo **ajustado = realloc(articulos, (size_t)cantidad * sizeof(articulo *));
    if (ajustado)
    {
        articulos = ajustado;
    }

    *total_articulos = cantidad;
    return articulos;
}

// Ordena usando el heap numerico (anos)
articulo **ordenar_por_ano(articulo **todos, int total, int cantidad)
{
    arbol_numeros *arbol = arbol_numeros_crear(total);
    if (!arbol)
    {
        free(todos);
        return NULL;
    }
    for (int i = 0; i < total; i++)
    {
        arbol_numeros_insertar(arbol, todos[i]);
    }
    free(todos);

    articulo **resultado = calloc((size_t)cantidad, sizeof(articulo *));
    if (!resultado)
    {
        while (arbol->tamano > 0)
        {
            articulo *lib = arbol_numeros_extraer_max(arbol);
            free(lib);
        }
        arbol_numeros_destruir(arbol);
        return NULL;
    }

    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = arbol_numeros_extraer_max(arbol);
    }

    while (arbol->tamano > 0)
    {
        articulo *lib = arbol_numeros_extraer_max(arbol);
        free(lib);
    }
    arbol_numeros_destruir(arbol);

    return resultado;
}

// Ordena usando el heap de texto (titulo o ruta)
articulo **ordenar_por_texto(articulo **todos, int total, int cantidad, criterio_orden criterio)
{
    arbol_letras *arbol = arbol_letras_crear(total, criterio);
    if (!arbol)
    {
        free(todos);
        return NULL;
    }
    for (int i = 0; i < total; i++)
    {
        arbol_letras_insertar(arbol, todos[i]);
    }
    free(todos);

    articulo **resultado = calloc((size_t)cantidad, sizeof(articulo *));
    if (!resultado)
    {
        while (arbol->tamano > 0)
        {
            articulo *lib = arbol_letras_extraer_max(arbol);
            free(lib);
        }
        arbol_letras_destruir(arbol);
        return NULL;
    }

    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = arbol_letras_extraer_max(arbol);
    }

    while (arbol->tamano > 0)
    {
        articulo *lib = arbol_letras_extraer_max(arbol);
        free(lib);
    }
    arbol_letras_destruir(arbol);

    return resultado;
}

// Comparador para qsort por tamano de titulo
int comparar_tamano(const void *a, const void *b)
{
    const articulo *art_a = *(const articulo **)a;
    const articulo *art_b = *(const articulo **)b;

    int palabras_a = contar_palabras(art_a->titulo);
    int palabras_b = contar_palabras(art_b->titulo);

    if (palabras_a == palabras_b)
    {
        return 0;
    }
    return (palabras_a < palabras_b) ? -1 : 1;
}

// Ordena por cantidad de palabras en el titulo
articulo **ordenar_por_tamano(articulo **todos, int total, int cantidad)
{
    qsort(todos, (size_t)total, sizeof(articulo *), comparar_tamano);

    articulo **resultado = calloc((size_t)cantidad, sizeof(articulo *));
    if (!resultado)
    {
        for (int i = 0; i < total; i++)
        {
            free(todos[i]);
        }
        free(todos);
        return NULL;
    }

    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = todos[i];
    }

    for (int i = cantidad; i < total; i++)
    {
        free(todos[i]);
    }
    free(todos);

    return resultado;
}

// Funcion principal que el resto del programa usa
articulo **ordenar_articulos(criterio_orden criterio, int cantidad, int *total_encontrados)
{
    int total = 0;
    articulo **todos = cargar_articulos(&total);

    if (!todos || total == 0)
    {
        *total_encontrados = 0;
        return NULL;
    }

    if (cantidad > total)
    {
        cantidad = total;
    }

    articulo **resultado = NULL;

    switch (criterio)
    {
    case TITULO_ALFABETICO:
    case NOMBRE_ARCHIVO:
        resultado = ordenar_por_texto(todos, total, cantidad, criterio);
        break;
    case ANO_PUBLICACION:
        resultado = ordenar_por_ano(todos, total, cantidad);
        break;
    case TAMANO_TITULO:
        resultado = ordenar_por_tamano(todos, total, cantidad);
        break;
    }

    *total_encontrados = resultado ? cantidad : 0;
    return resultado;
}

// Libera la memoria de los articulos recibidos
void liberar_articulos(articulo **articulos, int cantidad)
{
    if (!articulos)
        return;

    for (int i = 0; i < cantidad; i++)
    {
        if (articulos[i])
        {
            free(articulos[i]);
        }
    }
    free(articulos);
}

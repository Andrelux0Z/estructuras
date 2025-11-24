#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ordenador.h"
#include "arboles.h"

static void normalizar_utf8(char *texto);
static void convertir_minusculas(char *texto);

static char *leer_linea(FILE *archivo)
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

static void recortar_blancos(char *texto)
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

static void convertir_minusculas(char *texto)
{
    if (!texto)
    {
        return;
    }

    for (char *p = texto; *p; p++)
    {
        *p = (char)tolower((unsigned char)*p);
    }
}

static void normalizar_utf8(char *texto)
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
            case 0x81:
                reemplazo = 'A';
                break; // Á
            case 0x89:
                reemplazo = 'E';
                break; // É
            case 0x8D:
                reemplazo = 'I';
                break; // Í
            case 0x93:
                reemplazo = 'O';
                break; // Ó
            case 0x9A:
                reemplazo = 'U';
                break; // Ú
            case 0xA1:
                reemplazo = 'a';
                break; // á
            case 0xA9:
                reemplazo = 'e';
                break; // é
            case 0xAD:
                reemplazo = 'i';
                break; // í
            case 0xB3:
                reemplazo = 'o';
                break; // ó
            case 0xBA:
                reemplazo = 'u';
                break; // ú
            case 0x91:
                reemplazo = 'N';
                break; // Ñ
            case 0xB1:
                reemplazo = 'n';
                break; // ñ
            case 0xBC:
                reemplazo = 'u';
                break; // ü
            case 0x82:
                reemplazo = 'A';
                break; // Â
            case 0xA2:
                reemplazo = 'a';
                break; // â
            default:
                break;
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
            case 0xA1:
                reemplazo = '!';
                break; // ¡
            case 0xBF:
                reemplazo = '?';
                break; // ¿
            case 0xB0:
                reemplazo = 'o';
                break; // º
            case 0xBA:
                reemplazo = 'o';
                break; // º
            default:
                break;
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
                { // '
                    texto[escritura++] = '\'';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0x9C || tercero == 0x9D)
                { // "
                    texto[escritura++] = '"';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0x93 || tercero == 0x94)
                { // -
                    texto[escritura++] = '-';
                    lectura += 3;
                    continue;
                }
                if (tercero == 0xA6)
                { // ...
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

// Funcion para contar palabras en un texto (separadas por espacios)
int contar_palabras(const char *texto)
{
    if (!texto || texto[0] == '\0')
        return 0;

    int count = 0;
    int en_palabra = 0;

    for (int i = 0; texto[i] != '\0'; i++)
    {
        if (texto[i] != ' ' && texto[i] != '\t' && texto[i] != '\n')
        {
            if (!en_palabra)
            {
                count++;
                en_palabra = 1;
            }
        }
        else
        {
            en_palabra = 0;
        }
    }

    return count;
}

// Funcion de comparacion generica segun criterio
// Retorna: >0 si a>b, <0 si a<b, 0 si iguales
int comparar_articulos(articulo *a, articulo *b, criterio_orden criterio)
{
    int resultado = 0;

    switch (criterio)
    {
    case TITULO_ALFABETICO:
        resultado = strcmp(a->titulo, b->titulo);
        break;

    case TAMANO_TITULO:
        resultado = contar_palabras(a->titulo) - contar_palabras(b->titulo);
        break;

    case NOMBRE_ARCHIVO:
        resultado = strcmp(a->ruta, b->ruta);
        break;

    case ANO_PUBLICACION:
        resultado = a->ano - b->ano;
        break;
    }

    return resultado;
}

// Funcion heapify_down para mantener propiedad max-heap
void heapify_down_generico(articulo **datos, int tamano, int i, criterio_orden criterio)
{
    int mayor = i;
    int izq = hijo_izq(i);
    int der = hijo_der(i);

    // Comparar con hijo izquierdo
    if (izq < tamano && comparar_articulos(datos[izq], datos[mayor], criterio) > 0)
    {
        mayor = izq;
    }

    // Comparar con hijo derecho
    if (der < tamano && comparar_articulos(datos[der], datos[mayor], criterio) > 0)
    {
        mayor = der;
    }

    // Si el mayor no es la raiz, hacer swap y recursion
    if (mayor != i)
    {
        swap_articulo(&datos[i], &datos[mayor]);
        heapify_down_generico(datos, tamano, mayor, criterio);
    }
}

// Funcion para extraer el maximo del heap
articulo *extraer_maximo_heap(articulo **datos, int *tamano, criterio_orden criterio)
{
    if (*tamano <= 0)
        return NULL;

    // El maximo esta en la raiz
    articulo *max = datos[0];

    // Mover el ultimo elemento a la raiz
    datos[0] = datos[*tamano - 1];
    (*tamano)--;

    // Restaurar propiedad del heap
    if (*tamano > 0)
    {
        heapify_down_generico(datos, *tamano, 0, criterio);
    }

    return max;
}

// Funcion para cargar todos los articulos del archivo
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
            linea = NULL;
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
                art->nombre[sizeof(art->nombre) - 1] = '\0';
                break;
            case 1:
                strncpy(art->apellidos, token, sizeof(art->apellidos) - 1);
                art->apellidos[sizeof(art->apellidos) - 1] = '\0';
                break;
            case 2:
                strncpy(art->titulo, token, sizeof(art->titulo) - 1);
                art->titulo[sizeof(art->titulo) - 1] = '\0';
                break;
            case 3:
                strncpy(art->ruta, token, sizeof(art->ruta) - 1);
                art->ruta[sizeof(art->ruta) - 1] = '\0';
                break;
            case 4:
                art->ano = atoi(token);
                break;
            case 5:
                strncpy(art->resumen, token, sizeof(art->resumen) - 1);
                art->resumen[sizeof(art->resumen) - 1] = '\0';
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
                linea = NULL;
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
        *total_articulos = 0;
        return NULL;
    }

    if (cantidad == 0)
    {
        free(articulos);
        *total_articulos = 0;
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

// Funcion para construir un heap desde un arreglo (heapify)
void construir_heap(articulo **datos, int tamano, criterio_orden criterio)
{
    // Empezar desde el ultimo nodo no-hoja y hacer heapify hacia arriba
    for (int i = (tamano / 2) - 1; i >= 0; i--)
    {
        heapify_down_generico(datos, tamano, i, criterio);
    }
}

// Funcion principal de ordenamiento usando heap
articulo **ordenar_articulos(criterio_orden criterio, int cantidad, int *total_encontrados)
{
    // Cargar todos los articulos
    int total;
    articulo **todos = cargar_articulos(&total);

    if (!todos || total == 0)
    {
        *total_encontrados = 0;
        return NULL;
    }

    // Ajustar cantidad si es mayor al total
    if (cantidad > total)
    {
        cantidad = total;
    }

    // Construir heap con todos los articulos
    construir_heap(todos, total, criterio);

    // Heapsort: ordenar arreglo completo de menor a mayor
    for (int tamano_actual = total; tamano_actual > 1; tamano_actual--)
    {
        swap_articulo(&todos[0], &todos[tamano_actual - 1]);
        heapify_down_generico(todos, tamano_actual - 1, 0, criterio);
    }

    // Reservar espacio para los resultados
    articulo **resultado = calloc(cantidad, sizeof(articulo *));
    if (!resultado)
    {
        for (int i = 0; i < total; i++)
        {
            free(todos[i]);
        }
        free(todos);
        *total_encontrados = 0;
        return NULL;
    }

    // Copiar los primeros 'cantidad' articulos ya ordenados
    for (int i = 0; i < cantidad; i++)
    {
        resultado[i] = todos[i];
        todos[i] = NULL;
    }

    // Liberar articulos restantes no utilizados
    for (int i = cantidad; i < total; i++)
    {
        if (todos[i])
        {
            free(todos[i]);
        }
    }
    free(todos);

    *total_encontrados = cantidad;
    return resultado;
}

// Funcion para liberar memoria de articulos
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

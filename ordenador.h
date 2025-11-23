#ifndef ORDENADOR_H
#define ORDENADOR_H

// Extrae el campo n de una linea delimitada por '|'
// Retorna un puntero al campo extraido (debe ser liberado por el caller)
char *obtener_campo(const char *linea, int n);

#endif

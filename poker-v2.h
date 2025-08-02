/**
 * poker.h - Archivo de Cabecera
 *
 * Contiene las declaraciones de funciones, las inclusiones de librerías
 * estándar y las definiciones de estructuras y macros para el proyecto.
 */

#ifndef POKER_V2_H
#define POKER_V2_H

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

// --- Definiciones de Colores y Palos (Sin cambios) ---
#define RED "\x1b[31m"
#define BLACK "\e[0;30m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[0m"

#define ACE "A"
#define HEART "♥"
#define SPADE "♠"
#define DIAMOND "♦"
#define CLUB "♣"
#define JOKER "☺"

// --- Estructuras de Datos ---
// CAMBIO 1: Se añade una estructura para representar una carta.
// Esto es necesario para poder agruparlas en una "mano".
typedef struct
{
    const char *suit;
    char value;
    const char *color;
} Card;

// --- Declaraciones de Funciones ---

// CAMBIO 2: Se declara la nueva función que dibujará la mano completa.
void print_hand(Card hand[], int num_cards);

// Prototipos para tus funciones existentes, para que el compilador las conozca.
void printCard(const char *suit, char value, const char *color);


#endif // POKER_V2_H

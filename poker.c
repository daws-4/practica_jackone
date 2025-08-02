#include <stdio.h>
#include <stdlib.h>
#include "poker-v2.h"
#include <time.h>

// Esta estructura representa una carta individual
typedef struct
{
    char suit[4];         // El palo: "♥", "♠", "♦", "♣"
    char displayValue[3]; // El valor a mostrar: "A", "K", "10", etc.
    int numericValue;     // El valor para sumar en el juego (As=1, K=10, etc.)
    const char *color;    // El color para la función printCard (RED o BLACK)
} Card;

typedef struct
{
    char username[11]; // 10 caracteres + 1 para el terminador nulo '\0'
    int score;         // El puntaje del jugador
    long uniqueID;     // Un identificador único
    time_t timestamp;  // La fecha y hora de registro
    // Aquí añadiremos la mano de cartas más adelante
} Player[3];



int main(int argc, char *argv[]) {
	printf("Cartas de poker en ASCII Art:\n\n");
	printCard(ACE, 'A', RED);
    printf("mi libro, luna de puton\n");

    system("pause");
	return 0;
}

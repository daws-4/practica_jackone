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
// Esta función llena un arreglo (mazo) con las 52 cartas estándar
void createDeck(Card deck[])
{
    const char *suits[] = {"♥", "♦", "♠", "♣"};
    const char *colors[] = {RED, RED, WHITE, WHITE}; // Corazones y Diamantes rojos, el resto negros
    const char *displayValues[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int numericValues[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10}; // Valores para el juego

    int cardIndex = 0;
    for (int s = 0; s < 4; s++)
    { // Bucle para los 4 palos
        for (int v = 0; v < 13; v++)
        { // Bucle para los 13 valores
            strcpy(deck[cardIndex].suit, suits[s]);
            strcpy(deck[cardIndex].displayValue, displayValues[v]);
            deck[cardIndex].numericValue = numericValues[v];
            deck[cardIndex].color = colors[s];
            cardIndex++;
        }
    }
}

// Esta estructura representa a un jugador
typedef struct
{
    char username[11]; // 10 caracteres + 1 para el terminador nulo '\0'
    int score;         // El puntaje del jugador
    long uniqueID;     // Un identificador único
    time_t timestamp;  // La fecha y hora de registro
    // Aquí añadiremos la mano de cartas más adelante
} Player;

Player createNewPlayer()
{
    Player newPlayer;

    // 1. Pedir el nombre de usuario
    printf("Introduce tu nombre de usuario (max 10 caracteres): ");
    scanf("%10s", newPlayer.username);

    // 2. Inicializar el puntaje
    newPlayer.score = 0;

    // 3. Generar el timestamp actual
    newPlayer.timestamp = time(NULL);

    // 4. Crear un ID único combinando el timestamp y un número aleatorio
    newPlayer.uniqueID = newPlayer.timestamp + (rand() % 1000);
as
    // Imprimir los datos del jugador para verificar
    printf("\n--- ¡Jugador Creado! ---\n");
    printf("Usuario: %s\n", newPlayer.username);
    printf("ID: %ld\n", newPlayer.uniqueID);
    // ctime() convierte el timestamp a un formato de texto legible
    printf("Registrado el: %s", ctime(&newPlayer.timestamp));
    printf("------------------------\n");

    return newPlayer;
}


int main(int argc, char *argv[]) {
	printf("Cartas de poker en ASCII Art:\n\n");
	printCard(ACE, 'A', RED);
    printf("mi libro, luna de puton\n");

    system("pause");
	return 0;
}

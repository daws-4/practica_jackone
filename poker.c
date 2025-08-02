#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "poker-v2.h"
#include <conio.h>

#define ANSI_COLOR_RESET "\x1b[0m"
FILE *archivo;

// --- Tus Estructuras de Datos (Sin cambios) ---
typedef struct
{
    char suit[4];
    char displayValue[3];
    int numericValue;
    const char *color;
} GameCard; // Renombrada para evitar conflicto con la de poker.h

typedef struct
{
    char username[11];
    int score;
    long uniqueID;
    time_t timestamp;
} Player;

void startGame();
void showRules();
void showMainMenu();
// --- Implementación de tus funciones de dibujo (Sin cambios en su lógica interna) ---

void print_bigger_card(const char* suit, const char value, const char* color) {
    const char *card_template[7][7] = {
        {"┌", "─", "─", "─", "─", "─", "┐"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"└", "─", "─", "─", "─", "─", "┘"}
    };
    char dynamic_card[7][7][5];
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            strcpy(dynamic_card[i][j], card_template[i][j]);
        }
    }
    strcpy(dynamic_card[3][3], suit);
    if (value == '0') {
        strcpy(dynamic_card[1][1], "1"); strcpy(dynamic_card[1][2], "0");
        strcpy(dynamic_card[5][4], "1"); strcpy(dynamic_card[5][5], "0");
    } else {
        static char value_str[2];
        value_str[0] = value; value_str[1] = '\0';
        strcpy(dynamic_card[1][2], value_str);
        strcpy(dynamic_card[5][4], value_str);
    }
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            printf("%s%s", color, dynamic_card[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printCard(const char* suit, char value, const char* color) {
    value = toupper(value);
    if (value == 'J') {
        print_bigger_card(JOKER, 'J', color);
        return;
    }
    if (value == '0') {
        print_bigger_card(suit, '0', color);
        return;
    }
    print_bigger_card(suit, value, color);
}

// --- CAMBIO 3: Implementación de la nueva lógica para impresión horizontal ---

void generate_card_matrix(Card card, char card_buffer[7][7][5]) {
    const char *card_template[7][7] = {
        {"┌", "─", "─", "─", "─", "─", "┐"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"},
        {"└", "─", "─", "─", "─", "─", "┘"}
    };
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            strcpy(card_buffer[i][j], card_template[i][j]);
        }
    }
    strcpy(card_buffer[3][3], card.suit);
    char value = toupper(card.value);
    if (value == 'J') { strcpy(card_buffer[3][3], JOKER); }
    if (value == '0') {
        strcpy(card_buffer[1][1], "10");
        strcpy(card_buffer[5][4], "10");
    } else {
        char value_str[2] = {value, '\0'};
        strcpy(card_buffer[1][2], value_str);
        strcpy(card_buffer[5][4], value_str);
    }
}

void print_hand(Card hand[], int num_cards) {
    if (num_cards == 0) return;
    char all_cards_buffer[num_cards][7][7][5];
    for (int i = 0; i < num_cards; i++) {
        generate_card_matrix(hand[i], all_cards_buffer[i]);
    }
    for (int row = 0; row < 7; row++) {
        for (int card_idx = 0; card_idx < num_cards; card_idx++) {
            printf("%s", hand[card_idx].color);
            for(int col=0; col < 7; col++){
                printf("%s", all_cards_buffer[card_idx][row][col]);
            }
            printf("   ");
        }
        printf("\n");
    }
    printf("%s", WHITE);
}

// --- Tu Lógica de Menús y Jugadores (Sin cambios) ---

int selectNumberOfPlayers() {
    int selection = 0, key = 0;
    while (1) {
        system("cls");
        printf("--- Selecciona el numero de jugadores ---\n\n");
        printf(selection == 0 ? " -> 3 Jugadores\n" : "    3 Jugadores\n");
        printf(selection == 1 ? " -> 4 Jugadores\n" : "    4 Jugadores\n");
        key = _getch();
        if (key == 13) return selection + 3;
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) selection--;
            if (key == 80 && selection < 1) selection++;
        }
    }
}

void startGame() {
    archivo = fopen("record.txt", "a");
    system("cls");
    printf("--- Iniciando el Juego ---\n\n");
    int numPlayers = selectNumberOfPlayers();
    printf("Numero de jugadores seleccionados: %d\n", numPlayers);
    Player players[numPlayers];
    for (int i = 0; i < numPlayers; i++) {
        printf("Ingresa el nombre del jugador %d: ", i + 1);
        scanf(" %[^\n]s", players[i].username);
        players[i].score = 0;
        players[i].timestamp = time(NULL);
        players[i].uniqueID = players[i].timestamp + rand() % 1000;
        printf("¡Jugador '%s' registrado!\n\n", players[i].username);
    }
    for (int i = 0; i < numPlayers; i++) {
        fprintf(archivo, "%ld %s %d %ld\n", players[i].uniqueID, players[i].username, players[i].score, (long)players[i].timestamp);
    }
    printf("¡El juego ha terminado!\n");
    system("pause");
    showMainMenu();
}

void showRules() {
    system("cls");
    printf("--- Reglas de JackOne ---\n\n");
    printf("1. El objetivo es sumar 21 puntos.\n");
    printf("2. Si te pasas de 21, pierdes.\n");
    printf("3. ...etc.\n\n");
    system("pause");
}

void showMainMenu() {
    int select = 0;
    while (select != 4) {
        system("cls");
        printf("=== BIENVENIDO A JACKONE ===\n\n");

        // CAMBIO 4: Se reemplazan las llamadas a printCard por una sola llamada a print_hand.
        Card menu_cards[6] = {
            {ACE, 'A', RED}, {HEART, 'K', GREEN}, {SPADE, 'Q', BLUE},
            {CLUB, 'J', MAGENTA}, {JOKER, 'J', CYAN}, {DIAMOND, '9', YELLOW}
        };
        print_hand(menu_cards, 6);

        printf(ANSI_COLOR_RESET "\n\n1. Iniciar Juego\n");
        printf("2. Ver Reglas\n");
        printf("3. Ver Marcadores\n");
        printf("4. Salir\n\n");
        printf("Elige una opcion: ");
        scanf("%d", &select);
        switch (select) {
            case 1: startGame(); break;
            case 2: showRules(); break;
            case 3:
                printf("\nMarcadores:\n");
                printf("Jugador 1: 100 puntos\n");
                system("pause");
                break;
            case 4: printf("\nGracias por jugar. ¡Adios!\n"); break;
            default: printf("\nOpcion no valida.\n"); system("pause"); break;
        }
    }
}

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    srand(time(NULL));
    showMainMenu();
    return 0;
}

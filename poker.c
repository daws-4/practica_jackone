#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "poker-v2.h"
#include <conio.h>

#define ANSI_COLOR_RESET "\x1b[0m"
#define MAX_PLAYERS 4

FILE *SaveFile;

// CAMBIO 2: Se eliminan las definiciones duplicadas de GameCard y Player.
// Ahora se usan las que están en poker-v2.h, solucionando el error de redefinición.

// Prototipos para funciones internas de este archivo
void print_bigger_card(const char *suit, const char value, const char *color);
void generate_card_matrix(GameCard card, char card_buffer[7][7][5]);
void SaveRecord(Player players[], int numPlayers);
int selectNumberOfPlayers();
void showRecords();


//Función para seleccionar jugadores
int selectNumberOfPlayers()
{
    int selection = 0, key = 0;
    while (1)
    {
        system("cls");
        printf("--- Selecciona el numero de jugadores ---\n\n");
        printf(selection == 0 ? " -> 3 Jugadores\n" : "    3 Jugadores\n");
        printf(selection == 1 ? " -> 4 Jugadores\n" : "    4 Jugadores\n");
        key = _getch();
        if (key == 13)
            return selection + 3;
        if (key == 224)
        {
            key = _getch();
            if (key == 72 && selection > 0)
                selection--;
            if (key == 80 && selection < 1)
                selection++;
        }
    }
}

void CreateDeck(GameCard deck[])
{
    const char *suits[] = {HEART, SPADE, DIAMOND, CLUB};
    const char *colors[] = {RED, WHITE, RED, WHITE}; // Corregido para que coincida con los palos
    const char *displayValues[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int numericValues[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
    int index = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            strcpy(deck[index].suit, suits[i]);
            strcpy(deck[index].displayValue, displayValues[j]);
            deck[index].numericValue = numericValues[j];
            deck[index].color = colors[i];
            switch (j)
            {
            case 10:
                deck[index].isSpecial = TRUE;
                deck[index].specialValue = 1;
                break;
            case 11:
                deck[index].isSpecial = TRUE;
                deck[index].specialValue = 2;
                break;
            case 12:
                deck[index].isSpecial = TRUE;
                deck[index].specialValue = 3;
                break;
            default:
                deck[index].isSpecial = FALSE;
                deck[index].specialValue = 0;
            }
            index++;
        }
    }
    for (int i = index - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        GameCard temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}
//funcion para guardar los records de los jugadores y el histórico de jugadores
void SaveRecord(Player players[], int numPlayers)
{
    FILE *record = fopen("record.txt", "a");
    for (int i = 0; i < numPlayers; i++)
    {
        fprintf(record, "%ld %s %d %ld\n", players[i].uniqueID, players[i].username, players[i].score, (long)players[i].timestamp);
    }
    fclose(record);
}
//guarda los estados actuales de todos los jugadores
void SaveGame(Player players[], int numPlayers, GameCard gameDeck[], int cardDealIndex, int countRounds, int playerStatuses[], int currentPlayerIndex, int turnDirection)
{
    FILE *game = fopen("game.txt", "w");
    if (!game)
    {
        // Si no se puede abrir el archivo, no hacemos nada.
        return;
    }

    // 1. Guardar el estado general del juego (número de jugadores, etc.)
    fprintf(game, "%d %d %d %d %d\n",
            numPlayers,
            cardDealIndex,
            countRounds,
            currentPlayerIndex, 
            turnDirection);    

    // 2. Guardar el estado completo del mazo de cartas
    // Esto asegura que el orden barajado se preserve.
    for (int i = 0; i < 52; i++)
    {
        fprintf(game, "%s %s %d %d %d %d %s\n",
                gameDeck[i].suit,
                gameDeck[i].displayValue,
                gameDeck[i].numericValue,
                gameDeck[i].isSpecial,
                gameDeck[i].specialValue,
                i, // Índice de la carta para referencia
                gameDeck[i].color);
    }

    // 3. Guardar los datos de cada jugador (LA PARTE CLAVE)
    for (int i = 0; i < numPlayers; i++)
    {
        // Primero, escribimos toda la información del jugador en una línea,
        // SIN incluir un salto de línea (\n) al final.
        fprintf(game, "%ld %s %d %ld %d %d",
                players[i].uniqueID,
                players[i].username,
                players[i].score,
                (long)players[i].timestamp,
                players[i].cardCount,
                playerStatuses[i]);

        // A continuación, en la MISMA línea, añadimos los índices de las cartas
        // que tiene el jugador, separados por un espacio.
        for (int j = 0; j < players[i].cardCount; j++)
        {
            fprintf(game, " %d", players[i].CardOwn[j]);
        }

        // Finalmente, AHORA SÍ, añadimos el salto de línea para que el siguiente
        // jugador se escriba en una nueva línea.
        fprintf(game, "\n");
    }

    fclose(game);
}
// función para cargar los datos de una partida guardada
int loadGame(Player players[], int *numPlayers, GameCard gameDeck[], int *cardDealIndex, int *countRounds, int playerStatuses[],int *currentPlayerIndex, int *turnDirection)
{
    FILE *game = fopen("game.txt", "r");
    if (!game)
    {
        printf("No se pudo cargar el juego. Archivo no encontrado.\n");
        return 0;
    }

    int nJug, cDeal, cRounds, currPlayer, turnDir;
    if (fscanf(game, "%d %d %d %d %d\n", &nJug, &cDeal, &cRounds, &currPlayer, &turnDir) != 5)
    {
        fclose(game);
        printf("Archivo de juego corrupto o incompleto.\n");
        return 0;
    }
    *numPlayers = nJug;
    *cardDealIndex = cDeal;
    *countRounds = cRounds;
    *currentPlayerIndex = currPlayer;
    *turnDirection = turnDir;

    // --- Cargar el mazo con la corrección de color ---
    for (int i = 0; i < 52; i++)
    {
        char suit[4], displayValue[3], colorStr[10];
        int numericValue, isSpecial, specialValue, idx;

        // Leemos todos los campos de la línea de la carta
        fscanf(game, "%3s %2s %d %d %d %d %9s\n",
               suit, displayValue, &numericValue, &isSpecial, &specialValue, &idx, colorStr);

        strcpy(gameDeck[i].suit, suit);
        strcpy(gameDeck[i].displayValue, displayValue);
        gameDeck[i].numericValue = numericValue;
        gameDeck[i].isSpecial = isSpecial;
        gameDeck[i].specialValue = specialValue;

        // --- SOLUCIÓN PARA EL COLOR ---
        // Comparamos la cadena leída y asignamos el puntero a la constante correcta.
        if (strcmp(colorStr, RED) == 0)
        {
            gameDeck[i].color = RED;
        }
        else if (strcmp(colorStr, WHITE) == 0)
        {
            gameDeck[i].color = WHITE;
        }
        else if (strcmp(colorStr, GREEN) == 0)
        {
            gameDeck[i].color = GREEN;
        }
        else if (strcmp(colorStr, BLUE) == 0)
        {
            gameDeck[i].color = BLUE;
        }
        else if (strcmp(colorStr, MAGENTA) == 0)
        {
            gameDeck[i].color = MAGENTA;
        }
        else if (strcmp(colorStr, CYAN) == 0)
        {
            gameDeck[i].color = CYAN;
        }
        else if (strcmp(colorStr, YELLOW) == 0)
        {
            gameDeck[i].color = YELLOW;
        }
        else
        {
            gameDeck[i].color = WHITE; // Un color por defecto
        }
    }

    // --- Cargar los jugadores (esta lógica es correcta si el game.txt está bien) ---
    for (int i = 0; i < nJug; i++)
    {
        fscanf(game, "%ld %10s %d %ld %d %d",
               &players[i].uniqueID,
               players[i].username,
               &players[i].score,
               (long *)&players[i].timestamp,
               &players[i].cardCount,
               &playerStatuses[i]);

        // Este bucle leerá los índices de las cartas, asumiendo que están todos en la misma línea
        for (int j = 0; j < players[i].cardCount; j++)
        {
            fscanf(game, " %d", &players[i].CardOwn[j]);
        }
    }

    fclose(game);
    printf("Partida cargada exitosamente desde game.txt\n");
    return 1;
}
// Calcula la suma de los valores numéricos de las cartas en la mano de un jugador
int calculateHandValue(Player player, GameCard gameDeck[])
{
    int total = 0;
    for (int i = 0; i < player.cardCount; i++)
    {
        total += gameDeck[player.CardOwn[i]].numericValue;
    }
    return total;
}

// Obtiene el valor de la carta más alta para el desempate (K=13, Q=12, J=11, etc.)
int getHighestCardValue(Player player, GameCard gameDeck[])
{
    int maxValue = 0;
    for (int i = 0; i < player.cardCount; i++)
    {
        int cardValue = gameDeck[player.CardOwn[i]].numericValue;
        // Asignar valores más altos a K, Q, J para el desempate
        if (strcmp(gameDeck[player.CardOwn[i]].displayValue, "K") == 0)
            cardValue = 13;
        if (strcmp(gameDeck[player.CardOwn[i]].displayValue, "Q") == 0)
            cardValue = 12;
        if (strcmp(gameDeck[player.CardOwn[i]].displayValue, "J") == 0)
            cardValue = 11;

        if (cardValue > maxValue)
        {
            maxValue = cardValue;
        }
    }
    return maxValue;
}

void startGame(int loadSavedGame)
{
    system("cls");
    printf("--- Iniciando el Juego ---\n\n");
    
    int numPlayers;
    Player players[MAX_PLAYERS];

    GameCard gameDeck[52];
    int cardDealIndex = 0;
    int playerStatuses[MAX_PLAYERS];
    int countRounds = 1;
    int alredyLoaded = 0;
    int currentPlayerIndex = 0;
    int turnDirection = 1;

    int partidaCargada = 0;
    if (loadSavedGame == 1)
    {
        partidaCargada = loadGame(players, &numPlayers, gameDeck, &cardDealIndex, &countRounds, playerStatuses, &currentPlayerIndex, &turnDirection);
        if (partidaCargada)
        {
            printf("Partida cargada exitosamente.\n");
            alredyLoaded = 0;
        }
        else
        {
            printf("No se pudo cargar la partida. Iniciando una nueva partida.\n");
        }
        Sleep(1500);
    }
    if(!partidaCargada){
        numPlayers = selectNumberOfPlayers();
            printf("Numero de jugadores seleccionados: %d\n", numPlayers);
        for (int i = 0; i < numPlayers; i++)
        {
            printf("Ingresa el nombre del jugador %d: ", i + 1);
            scanf("%10s", players[i].username);
            players[i].score = 0;
            players[i].timestamp = time(NULL);
            players[i].uniqueID = players[i].timestamp + rand() % 1000;
            players[i].cardCount = 0;
            printf("¡Jugador '%s' registrado!\n\n", players[i].username);
        }
        CreateDeck(gameDeck);
        cardDealIndex = 0;
        countRounds = 1;
    }
    boolean gameOver = FALSE;
    while (!gameOver)
    {
        if(partidaCargada && alredyLoaded != 0 || !partidaCargada){

            for (int i = 0; i < numPlayers; i++)
            {
                players[i].cardCount = 0;
            }
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < numPlayers; j++)
                {
                    players[j].CardOwn[players[j].cardCount] = cardDealIndex;
                    players[j].cardCount++;
                    cardDealIndex++;
                }
            }
        }
            
        boolean roundOver = FALSE;
        int usedK = 0;
        for (int i = 0; i < numPlayers; i++)
            playerStatuses[i] = 0; // 0: Jugando, 1: Plantado

        while (!roundOver)
        {
            system("cls");
            printf("--- RONDA %d ---\n\n", countRounds/numPlayers+1);
            for (int i = 0; i < numPlayers; i++)
            {
                int handValue = calculateHandValue(players[i], gameDeck);
                const char *statusStr = "JUGANDO";
                if (playerStatuses[i] == 1)
                    statusStr = "PLANTADO";
                if (playerStatuses[i] == 2)
                    statusStr = "ELIMINADO";

                printf("Jugador %s (%s) - Puntos: %d\n", players[i].username, statusStr, handValue);
                GameCard hand[players[i].cardCount];
                for (int j = 0; j < players[i].cardCount; j++)
                {
                    int cardIndex = players[i].CardOwn[j];
                    hand[j] = gameDeck[cardIndex];
                }
                print_hand(hand, players[i].cardCount);
                printf("\n");
            }
            printf("----------------------------------\n");

            // Verificar si solo queda un jugador activo
            int activePlayers = 0;
            for (int i = 0; i < numPlayers; i++)
            {
                if (playerStatuses[i] == 0)
                    activePlayers++;
            }
            if (activePlayers <= 1)
            {
                roundOver = TRUE;
                continue;
            }
            printf("Turno de: %s jugador: %d\n", players[currentPlayerIndex].username, currentPlayerIndex+1);

            if (playerStatuses[currentPlayerIndex] !=0)
            {
                if(playerStatuses[currentPlayerIndex]==2){
                    printf("%s ha sido eliminado. Pasando al siguiente jugador...\n", players[currentPlayerIndex].username);
                }else{
                    printf("%s ya se ha plantado. Pasando al siguiente jugador...\n", players[currentPlayerIndex].username);
                }
                Sleep(1500);
                currentPlayerIndex = (currentPlayerIndex + turnDirection + numPlayers) % numPlayers;
                continue;
            }

            // --- Lógica para listar cartas especiales y manejar el turno ---
            int specialCardHandIndices[3] = {-1, -1, -1}; // Índices en la mano del jugador para K, Q, J
            boolean hasK = FALSE, hasQ = FALSE, hasJ = FALSE;

            for (int j = 0; j < players[currentPlayerIndex].cardCount; j++)
            {
                int cardIndex = players[currentPlayerIndex].CardOwn[j];
                if (gameDeck[cardIndex].isSpecial)
                {
                    if (gameDeck[cardIndex].specialValue == 3 && !hasK)
                    {
                        specialCardHandIndices[2] = j;
                        hasK = TRUE;
                    }
                    if (gameDeck[cardIndex].specialValue == 2 && !hasQ)
                    {
                        specialCardHandIndices[1] = j;
                        hasQ = TRUE;
                    }
                    if (gameDeck[cardIndex].specialValue == 1 && !hasJ)
                    {
                        specialCardHandIndices[0] = j;
                        hasJ = TRUE;
                    }
                }
            }

            printf("Elige tu jugada:\n");
            printf("0. Guardar Partida\n");
            printf("1. Pedir una carta (DRAW)\n");
            printf("2. Plantarse (STAND)\n");
            if (hasK || hasQ || hasJ)
            {
                printf("3. Usar una carta especial (HIT)\n");
            }
            int select = 10;
            scanf("%d", &select);

            switch (select)
            {
            case 0: 
                SaveGame(players, numPlayers, gameDeck, cardDealIndex, countRounds, playerStatuses, currentPlayerIndex, turnDirection);
                printf("\nPartida guardada exitosamente.\n");
                Sleep(1500);
                roundOver = TRUE;
                gameOver = TRUE;
                continue;
            case 1:
                if (cardDealIndex < 52)
                {
                    players[currentPlayerIndex].CardOwn[players[currentPlayerIndex].cardCount] = cardDealIndex++;
                    players[currentPlayerIndex].cardCount++;
                    printf("\nHas pedido una carta.\n");
                }
                else
                {
                    printf("\nNo quedan cartas en el mazo.\n");
                }
                break;
            case 2:
                playerStatuses[currentPlayerIndex] = 1;
                printf("\nTe has plantado.\n");
                break;
            case 3:
                if (hasK || hasQ || hasJ)
                {
                    int specialSelect = 0;
                    int cardToRemoveIdx = -1;
                    int nextPlayerIndex = (currentPlayerIndex + turnDirection + numPlayers) % numPlayers;

                    system("cls");
                    printf("Selecciona la carta especial a usar:\n");
                    if (hasJ)
                    if(players[nextPlayerIndex].cardCount % 2 == 0){
                        printf("1. Usar J +2 cartas\n");
                    }else{
                        printf("1. Usar J +4 cartas\n");
                    }
                    if (hasQ)
                        printf("2. Usar Q (Reversa)\n");
                    if (hasK)
                        printf("3. Usar K (Saltar turno)\n");
                    scanf("%d", &specialSelect);


                    switch (specialSelect)
                    {
                    case 1: // JOKER
                        if (hasJ)
                        {
                            int penalty = (players[nextPlayerIndex].cardCount % 2 == 0) ? 2 : 4;
                            printf("\nSe aplicara una penalizacion de %d cartas a %s.\n", penalty, players[nextPlayerIndex].username);
                            for (int i = 0; i < penalty && cardDealIndex < 52; i++)
                            {
                                players[nextPlayerIndex].CardOwn[players[nextPlayerIndex].cardCount] = cardDealIndex++;
                                players[nextPlayerIndex].cardCount++;
                            }
                            cardToRemoveIdx = specialCardHandIndices[0];
                        }
                        break;
                    case 2: // QUEEN
                        if (hasQ)
                        {
                            printf("\n¡Reversa! El orden de los turnos ha cambiado.\n");
                            turnDirection *= -1;
                            cardToRemoveIdx = specialCardHandIndices[1];
                        }
                        break;
                    case 3: // KING
                        if (hasK)
                        {
                            printf("\n¡Salto! %s pierde su turno.\n", players[nextPlayerIndex].username);
                            usedK = 1;
                            cardToRemoveIdx = specialCardHandIndices[2];
                        }
                        break;
                    default:
                        printf("Seleccion invalida.\n");
                        break;
                    }

                    if (cardToRemoveIdx != -1)
                    {
                        // Eliminar la carta usada de la mano del jugador
                        for (int i = cardToRemoveIdx; i < players[currentPlayerIndex].cardCount - 1; i++)
                        {
                            players[currentPlayerIndex].CardOwn[i] = players[currentPlayerIndex].CardOwn[i + 1];
                        }
                        players[currentPlayerIndex].cardCount--;
                    }
                    if(usedK){
                        currentPlayerIndex = (currentPlayerIndex + turnDirection + numPlayers) % numPlayers; // Saltar un jugador
                    }
                }
                else
                {
                    printf("\nOpcion invalida.\n");
                }
                break;
            default:
                printf("\nOpcion invalida.\n");
                break;
            }

            int currentHandValue = calculateHandValue(players[currentPlayerIndex], gameDeck);
            if (currentHandValue > 21)
            {
                printf("\n¡Te has pasado de 21! Quedas eliminado de la ronda.\n");
                playerStatuses[currentPlayerIndex] = 2; // 2: Eliminado
            }
            if (currentHandValue == 21)
            {
                printf("\n¡JACKONE! Has conseguido 21.\n");
                roundOver = TRUE;
            }

            int playersStoodOrBusted = 0;
            for (int i = 0; i < numPlayers; i++)
            {
                if (playerStatuses[i] != 0)
                {
                    playersStoodOrBusted++;
                }
            }
            if (playersStoodOrBusted == numPlayers)
            {
                roundOver = TRUE;
            }

            Sleep(2000);
            currentPlayerIndex = (currentPlayerIndex + turnDirection + numPlayers) % numPlayers;
        }

        // --- ASIGNACIÓN DE PUNTOS AL FINAL DE LA RONDA ---
        if(!gameOver){

            printf("--- FIN DE LA RONDA %d ---\n\n", countRounds/numPlayers+1);
            
            int winnerIndex = -1;
            int winners[numPlayers];
        int winnerCount = 0;
        int maxScore = 0;

        // Buscar ganadores con 21
        for (int i = 0; i < numPlayers; i++)
        {
            if (playerStatuses[i] != 2 && calculateHandValue(players[i], gameDeck) == 21)
            {
                winners[winnerCount++] = i;
            }
        }

        // Si no hay ganadores con 21, buscar el más alto sin pasarse
        if (winnerCount == 0)
        {
            for (int i = 0; i < numPlayers; i++)
            {
                if (playerStatuses[i] != 2)
                {
                    int handValue = calculateHandValue(players[i], gameDeck);
                    if (handValue > maxScore)
                        maxScore = handValue;
                }
            }
            for (int i = 0; i < numPlayers; i++)
            {
                if (playerStatuses[i] != 2 && calculateHandValue(players[i], gameDeck) == maxScore)
                {
                    winners[winnerCount++] = i;
                }
            }
        }

        if (winnerCount == 1)
        {
            winnerIndex = winners[0];
            players[winnerIndex].score += (calculateHandValue(players[winnerIndex], gameDeck) == 21) ? 100 : 50;
            printf("El ganador es %s con %d puntos!\n", players[winnerIndex].username, players[winnerIndex].score);
        }
        else if (winnerCount > 1)
        {
            printf("Hay un empate. Desempatando por carta más alta...\n");
            int bestCard = 0;
            winnerIndex = winners[0]; // Ganador por defecto
            for (int i = 0; i < winnerCount; i++)
            {
                int playerIdx = winners[i];
                int highestCard = getHighestCardValue(players[playerIdx], gameDeck);
                if (highestCard > bestCard)
                {
                    bestCard = highestCard;
                    winnerIndex = playerIdx;
                }
            }
            players[winnerIndex].score += 50;
            printf("El ganador por desempate es %s!\n", players[winnerIndex].username);
        }
        else
        {
            printf("No hay ganadores en esta ronda.\n");
        }
        printf("\nPuntuaciones de la ronda:\n");
        for (int i = 0; i < numPlayers; i++)
        {
            if (i != winnerIndex && playerStatuses[i] != 2) // Solo si no está eliminado
            {
                players[i].score += calculateHandValue(players[i], gameDeck);
            }
            printf("- %s: %d\n", players[i].username, players[i].score);
        }
        SaveRecord(players, numPlayers);
        printf("\n¡La ronda ha terminado!\n");
        printf("¿Jugar otra partida? (s/n): ");
        char playAgain;
        scanf(" %c", &playAgain);
        if (playAgain != 's' && playAgain != 'S')
        {
            CreateDeck(gameDeck);
            cardDealIndex = 0;
            countRounds = 1;
            alredyLoaded = 0;
            currentPlayerIndex = 0;
            turnDirection = 1;
            for (int i = 0; i < numPlayers; i++){
                playerStatuses[i] = 0;
            }
            alredyLoaded++;
            gameOver = TRUE;
        }
    }
    }

    printf("¡El juego ha terminado!\n");
    system("pause");
    showMainMenu();
}

void showRules()
{
    system("cls");
    printf("\t\t\t\t\t --- Reglas de JackOne ---\n\n");
    printf("||\t1. El objetivo es sumar 21 puntos. si te pasas de 21 puntos pierdes\n");
    printf("||\t   y quedas eliminado de la ronda.\n||\n");
    printf("||\t2. Cada jugador recibe 2 cartas al inicio de la ronda.\n||\n");
    printf("||\t3. En tu turno, puedes:\n");
    printf("||\t   - Pedir una carta (DRAW): Añade una carta a tu mano.\n");
    printf("||\t   - Plantarte (STAND): Terminas tu turno y no pides más cartas.\n");
    printf("||\t   - Usar una carta especial (HIT): Si tienes J, Q o K, puedes usarlas\n");
    printf("||\t    para afectar a otros jugadores.\n||\n");
    printf("||\t4. Las cartas especiales son:\n");
    printf("||\t   - J (JOKER): Hace que el siguiente jugador robe 2 o 4 cartas\n");
    printf("||\t    dependiendo de si tiene un número par o impar de cartas.\n");
    printf("||\t   - Q (QUEEN): Invierte el orden de los turnos.\n");
    printf("||\t   - K (KING): Salta el turno del siguiente jugador.\n||\n");
    printf("||\t5. Si consigues 21 puntos, ganas automáticamente la ronda.\n||\n");
    printf("||\t6. Si todos los jugadores se plantan o quedan eliminados, la ronda termina.\n||\n");
    printf("||\t7. Al final de cada ronda, se asignan puntos:\n");
    printf("||\t   - Ganar con 21 puntos: 100 puntos.\n");
    printf("||\t   - Ganar sin 21 puntos: 50 puntos.\n");
    printf("||\t   - Otros jugadores suman sus puntos a su puntuación total.\n||\n");
    printf("||\t8. El juego continúa hasta que los jugadores decidan terminar\n");
    printf("||\t   O algún jugador llegue a 21 puntos\n");
    system("pause");
    showMainMenu();
}

void showRecords()
{
    system("cls");
    printf("--- Marcadores ---\n\n");

    FILE *record = fopen("record.txt", "r");
    if (record == NULL)
    {
        printf("No se encontro el archivo de records o esta vacio.\n");
        system("pause");
        return;
    }

    // Contar registros correctamente
    int count = 0;
    Player tempPlayer;
    while (fscanf(record, "%ld %10s %d %ld", &tempPlayer.uniqueID, tempPlayer.username, &tempPlayer.score, &tempPlayer.timestamp) == 4)
    {
        count++;
    }

    if (count == 0)
    {
        printf("El archivo de historial esta vacio.\n");
        fclose(record);
        system("pause");
        return;
    }

    // Reservar memoria y leer los datos
    Player *historicPlayers = (Player *)malloc(count * sizeof(Player));
    if (historicPlayers == NULL)
    {
        printf("Error de memoria.\n");
        fclose(record);
        system("pause");
        return;
    }

    rewind(record);
    int i = 0;
    while (i < count && fscanf(record, "%ld %10s %d %ld", &historicPlayers[i].uniqueID, historicPlayers[i].username, &historicPlayers[i].score, &historicPlayers[i].timestamp) == 4)
    {
        i++;
    }
    fclose(record);

    // Ordenar por puntaje descendente
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (historicPlayers[j].score < historicPlayers[j + 1].score)
            {
                Player temp = historicPlayers[j];
                historicPlayers[j] = historicPlayers[j + 1];
                historicPlayers[j + 1] = temp;
            }
        }
    }

    printf("%-15s %-10s %-10s %-20s\n", "JUGADOR", "PUNTAJE", "ID", "FECHA Y HORA");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        char fecha[32];
        time_t t = (time_t)historicPlayers[i].timestamp;
        struct tm *tm_info = localtime(&t);
        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("%-15s %-10d %-10ld %-20s\n",
               historicPlayers[i].username,
               historicPlayers[i].score,
               historicPlayers[i].uniqueID,
               fecha);
    }
    printf("\n");
    free(historicPlayers);
    system("pause");
}

void showName()
{
    system("cls");
    printf("%s", CYAN);

    printf("\n\n\n\n");
printf("\t\t  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄    ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄  \n");
printf("\t\t ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌  ▐░▌▐░░░░░░░░░░░▌▐░░▌      ▐░▌▐░░░░░░░░░░░▌ \n");
printf("\t\t  ▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌ ▐░▌ ▐░█▀▀▀▀▀▀▀█░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀  \n");
printf("\t\t       ▐░▌    ▐░▌       ▐░▌▐░▌          ▐░▌▐░▌  ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌           \n");
printf("\t\t       ▐░▌    ▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░▌░▌   ▐░▌       ▐░▌▐░▌ ▐░▌   ▐░▌▐░█▄▄▄▄▄▄▄▄▄  \n");
printf("\t\t       ▐░▌    ▐░░░░░░░░░░░▌▐░▌          ▐░░▌    ▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌ \n");
printf("\t\t       ▐░▌    ▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░▌░▌   ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌▐░█▀▀▀▀▀▀▀▀▀  \n");
printf("\t\t       ▐░▌    ▐░▌       ▐░▌▐░▌          ▐░▌▐░▌  ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌▐░▌           \n");
printf("\t\t  ▄▄▄▄▄█░▌    ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░▌ ▐░▌ ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌▐░█▄▄▄▄▄▄▄▄▄  \n");
printf("\t\t ▐░░░░░░░▌    ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░▌  ▐░▌▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌ \n");
printf("\t\t  ▀▀▀▀▀▀▀      ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀    ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀ \n");

printf("\n\n\n\n");

printf("%s", WHITE);
printf("          Presiona cualquier tecla para continuar...");

_getch();
}

void showMainMenu()
{
    int select = 0;
    while (select != 5)
    {
        system("cls");
        printf("=== BIENVENIDO A JACKONE ===\n\n");

        // CAMBIO 3: Se corrige la inicialización de menu_cards para que coincida
        // con la estructura GameCard que tú definiste.
        GameCard menu_cards[6] = {
            {ACE, "A", 0, FALSE, 0, RED},
            {HEART, "K", 10, TRUE, 3, GREEN},
            {SPADE, "Q", 10, TRUE, 2, BLUE},
            {CLUB, "10", 10, TRUE, 1, MAGENTA},
            {JOKER, "J", 10, TRUE, 1, CYAN},
            {DIAMOND, "9", 9, FALSE, 0, YELLOW}};
        print_hand(menu_cards, 6);
        
        printf(ANSI_COLOR_RESET "\n\n1. Iniciar Juego\n");
        printf("2. Cargar Partida\n");
        printf("3. Ver Reglas\n");
        printf("4. Ver Marcadores\n");
        printf("5. Salir\n\n");
        printf("Elige una opcion: ");
        scanf("%d", &select);
        int loadSavedGame;
        switch (select)
        {
        case 1:
        loadSavedGame = 0;
            startGame(loadSavedGame);
            break;
        case 2:
        loadSavedGame = 1;
            startGame(loadSavedGame);
            break;
        case 3:
            showRules();
            break;
        case 4:
            showRecords();
            break;
        case 5:
            printf("\nGracias por jugar. ¡Adios!\n");
            break;
        default:
            printf("\nOpcion no valida.\n");
            system("pause");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    srand(time(NULL));
    showName();
    showMainMenu();
    return 0;
}

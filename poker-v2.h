

#ifndef POKER_V2_H
#define POKER_V2_H

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

#define RED     "\x1b[31m"
#define BLACK   "\e[0;30m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[0m"

#define ACE     "A"
#define HEART   "♥"
#define SPADE   "♠"
#define DIAMOND "♦"
#define CLUB    "♣"
#define JOKER   "☺"

#define TRUE 1
#define FALSE 0

typedef struct
{
    char suit[4];
    char displayValue[3];
    int numericValue;
    boolean isSpecial;
    int specialValue;
    const char *color;
} GameCard;

typedef struct
{
    char username[11];
    int score;
    int CardOwn[17];
    int cardCount;
    long uniqueID;
    time_t timestamp;
} Player;

void print_hand(GameCard hand[], int num_cards);
void printCard(const char *suit, char value, const char *color);
void startGame(int loadSavedGame);
void showRules();
void showMainMenu();
void CreateDeck(GameCard deck[]);

void print_bigger_card(const char *suit, const char value, const char *color)
{
    const char *card_template[7][7] = {
        {"┌", "─", "─", "─", "─", "─", "┐"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"└", "─", "─", "─", "─", "─", "┘"}};
    char dynamic_card[7][7][5];
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            strcpy(dynamic_card[i][j], card_template[i][j]);
        }
    }
    strcpy(dynamic_card[3][3], suit);
    if (value == '0')
    {
        strcpy(dynamic_card[1][1], "1");
        strcpy(dynamic_card[1][2], "0");
        strcpy(dynamic_card[5][4], "1");
        strcpy(dynamic_card[5][5], "0");
    }
    else
    {
        static char value_str[2];
        value_str[0] = value;
        value_str[1] = '\0';
        strcpy(dynamic_card[1][2], value_str);
        strcpy(dynamic_card[5][4], value_str);
    }
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            printf("%s%s", color, dynamic_card[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printCard(const char *suit, char value, const char *color)
{
    value = toupper(value);
    if (value == 'J')
    {
        print_bigger_card(JOKER, 'J', color);
        return;
    }
    if (value == '0')
    {
        print_bigger_card(suit, '0', color);
        return;
    }
    print_bigger_card(suit, value, color);
}

void generate_card_matrix(GameCard card, char card_buffer[7][7][5])
{
    const char *card_template[7][7] = {
        {"┌", "─", "─", "─", "─", "─", "┐"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"│", " ", " ", " ", " ", " ", "│"}, {"└", "─", "─", "─", "─", "─", "┘"}};
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            strcpy(card_buffer[i][j], card_template[i][j]);
        }
    }
    strcpy(card_buffer[3][3], card.suit);
    char value = toupper(card.displayValue[0]);
    if (value == 'J')
    {
        strcpy(card_buffer[3][3], JOKER);
    }
    if (strcmp(card.displayValue, "10") == 0)
    {
        strcpy(card_buffer[1][1], "1");
        strcpy(card_buffer[1][2], "0");
        strcpy(card_buffer[5][4], "1");
        strcpy(card_buffer[5][5], "0");
    }
    else
    {
        char value_str[2] = {value, '\0'};
        strcpy(card_buffer[1][2], value_str);
        strcpy(card_buffer[5][4], value_str);
    }
}

void print_hand(GameCard hand[], int num_cards)
{
    if (num_cards == 0)
        return;
    char all_cards_buffer[num_cards][7][7][5];
    for (int i = 0; i < num_cards; i++)
    {
        generate_card_matrix(hand[i], all_cards_buffer[i]);
    }
    for (int row = 0; row < 7; row++)
    {
        for (int card_idx = 0; card_idx < num_cards; card_idx++)
        {
            printf("%s", hand[card_idx].color);
            for (int col = 0; col < 7; col++)
            {
                printf("%s", all_cards_buffer[card_idx][row][col]);
            }
            printf("   ");
        }
        printf("\n");
    }
    printf("%s", WHITE);
}


#endif // POKER_V2_H

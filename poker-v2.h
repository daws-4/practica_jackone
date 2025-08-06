

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
void startGame();
void showRules();
void showMainMenu();
void CreateDeck(GameCard deck[]);

#endif // POKER_V2_H

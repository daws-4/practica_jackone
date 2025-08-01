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

#define ACE 65
#define HEART 147
#define SPADE 148
#define CLUB 190
#define JOKER 177

void print_bigger_card(const int suit, const int value, const char* color) {
    int card[7][7] = {
        {201, 61, 61, 61, 61, 61, 187},
        {186, 32, 32, 32, 32, 32, 186},
        {186, 32, 32, 32, 32, 32, 186},
        {186, 32, 32, 32, 32, 32, 186},
        {186, 32, 32, 32, 32, 32, 186},
        {186, 32, 32, 32, 32, 32, 186},
        {200, 61, 61, 61, 61, 61, 188}
    };

    card[3][3] = (int) suit;

    if(value == '0') {
        card[1][1] = '1';
        card[1][2] = '0';

        card[5][4] = '1';
        card[5][5] = '0';
    }

    if(value != '0') {
        card[1][2] = (int) value;
        card[5][4] = (int) value;
    }

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            printf("%s%c", color, card[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printCard(int suit, char value, const char* color) {
    value = toupper(value);

    if(value == 'J') {
       print_bigger_card(JOKER, 'J', color);
       return;
    }

    if (value == '0') {
        print_bigger_card(suit, '0', color);
        return;
    }

    print_bigger_card(suit, value, color);
}

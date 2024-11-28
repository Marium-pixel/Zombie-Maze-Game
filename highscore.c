#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SINGLE_PLAYER_FILE "single_player_scores.txt"
#define PLAYER1_FILE "player1_scores.txt"
#define PLAYER2_FILE "player2_scores.txt"

// Function to save the highscore to the appropriate file
void saveHighscore(const char *playerName, int score, char mode) {
    FILE *file;

    // Determine the file to write based on the mode and player name
    if (mode == '1') {
        file = fopen(SINGLE_PLAYER_FILE, "a");
    } else if (strcmp(playerName, "Player X") == 0) {
        file = fopen(PLAYER1_FILE, "a");
    } else if (strcmp(playerName, "Player Y") == 0) {
        file = fopen(PLAYER2_FILE, "a");
    } else {
        printf("Error: Unknown player or mode.\n");
        return;
    }

    if (file == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }

    fprintf(file, "%s: %d\n", playerName, score);
    fclose(file);
    printf("Score saved for %s.\n", playerName);
}

// Function to display highscores from a file
void displayHighscoresFromFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    char line[256];

    if (file == NULL) {
        printf("No highscores available for this mode/player.\n");
        return;
    }

    printf("Highscores from %s:\n", fileName);
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

// Function to display all highscores
void displayHighscores(FILE* SINGLE_PLAYER_FILE, FILE* PLAYER1_FILE, FILE* PLAYER2_FILE) {
    printf("\n--- Highscores ---\n");
    printf("Single Player:\n");
    displayHighscoresFromFile(SINGLE_PLAYER_FILE);
    printf("\nPlayer 1 (X):\n");
    displayHighscoresFromFile(PLAYER1_FILE);
    printf("\nPlayer 2 (Y):\n");
    displayHighscoresFromFile(PLAYER2_FILE);
    printf("-------------------\n");
}

// Function to calculate score based on eliminations, time left, and difficulty
int calculateScore(int eliminations, int timeLeft, char difficulty) {
    int baseScore = eliminations * 100 + timeLeft * 10;
    if (difficulty == 'H' || difficulty == 'h') {
        baseScore *= 1.5; // Hard mode bonus
    }
    return baseScore;
}

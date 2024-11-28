#include "highscore.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SCORES 5

typedef struct {
    int score;
    char difficulty;
} HighScore;

void saveHighScore(int score, char difficulty) {
    HighScore scores[MAX_SCORES];
    FILE *file = fopen("highscores.dat", "rb");
    int numScores = 0;

    if (file != NULL) {
        numScores = fread(scores, sizeof(HighScore), MAX_SCORES, file);
        fclose(file);
    }

    HighScore newScore = {score, difficulty};
    int inserted = 0;

    for (int i = 0; i < numScores; i++) {
        if (newScore.score > scores[i].score) {
            for (int j = numScores - 1; j > i; j--) {
                scores[j] = scores[j-1];
            }
            scores[i] = newScore;
            inserted = 1;
            break;
        }
    }

    if (!inserted && numScores < MAX_SCORES) {
        scores[numScores] = newScore;
        numScores++;
    }

    file = fopen("highscores.dat", "wb");
    if (file != NULL) {
        fwrite(scores, sizeof(HighScore), numScores, file);
        fclose(file);
    }
}

int getHighScore() {
    HighScore scores[MAX_SCORES];
    FILE *file = fopen("highscores.dat", "rb");
    int highScore = 0;

    if (file != NULL) {
        int numScores = fread(scores, sizeof(HighScore), MAX_SCORES, file);
        fclose(file);

        if (numScores > 0) {
            highScore = scores[0].score;
        }
    }

    return highScore;
}

void printHighScores() {
    HighScore scores[MAX_SCORES];
    FILE *file = fopen("highscores.dat", "rb");

    if (file != NULL) {
        int numScores = fread(scores, sizeof(HighScore), MAX_SCORES, file);
        fclose(file);

        printf("High Scores:\n");
        for (int i = 0; i < numScores; i++) {
            printf("%d. %d (Difficulty: %c)\n", i+1, scores[i].score, scores[i].difficulty);
        }
    } else {
        printf("No high scores yet.\n");
    }
}

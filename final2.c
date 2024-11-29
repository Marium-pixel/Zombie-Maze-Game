 #include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<windows.h>
#include<conio.h>
#include "highscore.h"  // Include the highscore functions

#define MAZE_SIZE 20
#define BASE_NUM_ZOMBIES 5
#define TIME_LIMIT_EASY 120 // 2 minutes
#define TIME_LIMIT_HARD 60 // 1 minute

//sound functions
void startsound();
void endsound();
void winningsound();
void loosingsound();
void zombieapproach ();

#define RED "\e[0;31m" // Bright red color
#define BHBLU "\e[1;94m"
#define HYEL 14 // High Intensity Yellow for caution message (Windows color code)

#define BHRED "\e[1;91m" //bold high intensity red for game over
#define HGRN "\033[38;5;10m"  // High Intensity Green
#define RESET "\033[0m"
// Console colors for Windows
void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

 // Function to print the maze with color
void printMaze(char maze[MAZE_SIZE][MAZE_SIZE]) {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == '#') {
                setConsoleColor(12); // Red
                printf("# ");
            } else if (maze[i][j] == 'E') {
                setConsoleColor(11); // Bright Blue
                printf("E ");
            } else if (maze[i][j] == 'Z') {
                setConsoleColor(10); // Green
                printf("Z ");
            } else if (maze[i][j] == 'X' || maze[i][j] == 'Y') {
                setConsoleColor(14); // Yellow
                printf("%c ", maze[i][j]);
            } else {
                setConsoleColor(7); // Default color
                printf("%c ", maze[i][j]);
            }
        }
        printf("\n");
    }
    setConsoleColor(7); // Reset to default
}


// Function to move the zombies
void moveZombies(char maze[MAZE_SIZE][MAZE_SIZE], int zombieX[], int zombieY[], int numZombies) {
    int i;
    for (i = 0; i < numZombies; i++) {
        int newX, newY;
        do {
            int dx = rand() % 3 - 1;
            int dy = rand() % 3 - 1;
            newX = zombieX[i] + dx;
            newY = zombieY[i] + dy;
        } while (newX < 0 || newX >= MAZE_SIZE || newY < 0 || newY >= MAZE_SIZE || maze[newY][newX] != ' ');

        // Clear the previous zombie position
        maze[zombieY[i]][zombieX[i]] = ' ';

        // Set the new zombie position
        maze[newY][newX] = 'Z';
        zombieX[i] = newX;
        zombieY[i] = newY;
    }
}

// Function to check if the exit is in front of the player
int checkForExit(char direction, int playerX, int playerY, char maze[MAZE_SIZE][MAZE_SIZE]) {
    if (direction == 'W' || direction == 'w') {
        return maze[playerY - 1][playerX] == 'E';
    } else if (direction == 'A' || direction == 'a') {
        return maze[playerY][playerX - 1] == 'E';
    } else if (direction == 'S' || direction == 's') {
        return maze[playerY + 1][playerX] == 'E';
    } else if (direction == 'D' || direction == 'd') {
        return maze[playerY][playerX + 1] == 'E';
    }
    return 0; // Not moving towards exit
}

// Function to check for zombies in front and handle elimination
  int checkForZombiesAndEliminate(char direction, int* playerX, int* playerY, char maze[MAZE_SIZE][MAZE_SIZE], char playerChar, int* playerEliminations) {
    int zombieInFront = 0;

    // Determine if there's a zombie in front based on the direction
    if (direction == 'W' || direction == 'w') {
        if (maze[*playerY - 1][*playerX] == 'Z') zombieInFront = 1;
    } else if (direction == 'A' || direction == 'a') {
        if (maze[*playerY][*playerX - 1] == 'Z') zombieInFront = 1;
    } else if (direction == 'S' || direction == 's') {
        if (maze[*playerY + 1][*playerX] == 'Z') zombieInFront = 1;
    } else if (direction == 'D' || direction == 'd') {
        if (maze[*playerY][*playerX + 1] == 'Z') zombieInFront = 1;
    }

    if (zombieInFront) {
        // Provide a warning about the zombie
        printf("Warning: Zombie in front! Press 'O' twice to eliminate it.\n");
        zombieapproach(); // Calling zombie approaching sound

        // Wait for two 'O' key presses without blocking
        int count = 0;
        while (count < 2) {
            if (_kbhit()) {
                char input = _getch(); // Get input without blocking
                if (input == 'O' || input == 'o') {
                    count++;
                }
            }
        }

        // Clear the zombie's position in the maze before moving the player
        if (direction == 'W' || direction == 'w') {
            maze[*playerY - 1][*playerX] = ' '; // Clear the zombie
            maze[*playerY][*playerX] = ' '; // Clear the player's old position
            (*playerY)--; // Move player up
        } else if (direction == 'A' || direction == 'a') {
            maze[*playerY][*playerX - 1] = ' '; // Clear the zombie
            maze[*playerY][*playerX] = ' '; // Clear the player's old position
            (*playerX)--; // Move player left
        } else if (direction == 'S' || direction == 's') {
            maze[*playerY + 1][*playerX] = ' '; // Clear the zombie
            maze[*playerY][*playerX] = ' '; // Clear the player's old position
            (*playerY)++; // Move player down
        } else if (direction == 'D' || direction == 'd') {
            maze[*playerY][*playerX + 1] = ' '; // Clear the zombie
            maze[*playerY][*playerX] = ' '; // Clear the player's old position
            (*playerX)++; // Move player right
        }
        (*playerEliminations)++; // Increment the player's eliminations count
        printf("Zombie eliminated\n");
        return 1; // Indicate a zombie was eliminated
    }

    return 0; // No zombie in front
}

int main() {
   
    char maze[MAZE_SIZE][MAZE_SIZE] = {
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', '#', '#', ' ', '#', '#'},
        {'#', ' ', '#', '#', ' ', '#', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#', '#', ' ', ' ', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', ' ', '#', ' ', ' ', '#'},
        {'#', ' ', '#', '#', ' ', '#', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#', '#', '#', ' ', ' ', '#'},
        {'#', ' ', '#', '#', ' ', '#', '#', '#', ' ', ' ', ' ', ' ', '#', ' ', '#', '#', ' ', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', ' ', '#', '#', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', ' ', ' ', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'E', '#'}, // Exit marked with 'E'
    };

 int playerX[2] = {1, 4}; // Starting positions for Player 1 (X) and Player 2 (Y)
    int playerY[2] = {1, 1}; // Corrected starting positions
    int zombieX[BASE_NUM_ZOMBIES], zombieY[BASE_NUM_ZOMBIES];
    time_t startTime, currentTime;
    int eliminations[2] = {0, 0}; // Track eliminations for both players
    char playerChar[2] = {'X', 'Y'}; // Player representations
    int numZombies = BASE_NUM_ZOMBIES; // Number of zombies
    int timeLimit; // Time limit for the current mode
    int score = 0;
    int highScore = getHighScore();
    char mode, playerMode;

    startsound(); // calling start sound function
    printf("WELCOME TO THE ZOMBIE MAZE GAME!\n Here's a chance for you and your friend to compete to see who would reach the end first in a maze filled with zombies!\n Use W(up), S(down), A(left) and D(right) to move and O to kill the zombies.\n REMEMBER: if a zombie kills you, or time runs out, GAME OVER \n");
    printf("Select game mode (1: Single Player, 2: Two Player): ");
    scanf(" %c", &playerMode);
    printf("Select game difficulty (E: Easy, H: Hard): ");
    scanf(" %c", &mode);

    // Set time limit based on mode
    switch (mode) {
        case 'E': case 'e':
            timeLimit = TIME_LIMIT_EASY;
            break;
        case 'H': case 'h':
            timeLimit = TIME_LIMIT_HARD;
            numZombies = BASE_NUM_ZOMBIES * 2; // Increase number of zombies for hard mode
            break;
        default:
            printf("Invalid mode selected. Defaulting to Easy mode.\n");
            timeLimit = TIME_LIMIT_EASY;
            break;
    }

    // Initialize zombie positions
    srand(time(NULL));
    for (int i = 0; i < numZombies; i++) {
        do {
            zombieX[i] = rand() % (MAZE_SIZE - 2) + 1;
            zombieY[i] = rand() % (MAZE_SIZE - 2) + 1;
        } while (maze[zombieY[i]][zombieX[i]] != ' ');
        maze[zombieY[i]][zombieX[i]] = 'Z'; // Place zombie in the maze
    }

    // Mark starting positions in the maze
    maze[playerY[0]][playerX[0]] = 'X'; // Player 1 representation
    if (playerMode == '2') {
        maze[playerY[1]][playerX[1]] = 'Y'; // Player 2 representation
    } else {
        maze[playerY[1]][playerX[1]] = ' '; // Hide Player 2 in single-player mode
    }

    printf("Welcome to the maze! Use WASD to move, O to eliminate zombies, and Q to quit.\n");
    printMaze(maze);
    startTime = time(NULL);

    while (1) {
        currentTime = time(NULL);

        // Check if the time limit has been reached
        if (currentTime - startTime >= timeLimit) {
            printf("Time's up! Game over.\n");
            endsound(); // calling end sound function
            break;
        }

        for (int player = 0; player < (playerMode == '2' ? 2 : 1); player++) {
            if (maze[playerY[player]][playerX[player]] == ' ') {
                printf("Player %d (%c) is dead and cannot play anymore.\n", player + 1, playerChar[player]);
                loosingsound(); // Calling losing sound function

                if (playerMode == '2' && (maze[playerY[0]][playerX[0]] == ' ' && maze[playerY[1]][playerX[1]] == ' ')) {
                    printf("Both players are dead! Game over.\n");
                    endsound(); // Calling end sound function
                    return 0; // End the game immediately
                }
                continue; // Skip this player's turn
            }

            // Non-blocking input check
            if (_kbhit()) {
                char direction = _getch(); // Get input without blocking
                if (direction == 'Q' || direction == 'q') {
                    printf("Exiting game...\n");
                    return 0;
                }

                // Check for exit condition
                if (checkForExit(direction, playerX[player], playerY[player], maze)) {
                    printf("Congratulations, Player %d wins!!! :D\n", player + 1);
                    winningsound(); // Calling winning sound function

                    // Score calculation for single-player mode
                    if (playerMode == '1') {
                        printf("Zombies eliminated by Player X: %d\n", eliminations[0]);
                        score = eliminations[0] * 100;
                        if (mode == 'E' || mode == 'e') {
                            score *= 1.5;
                        } else if (mode == 'H' || mode == 'h') {
                            score *= 2.5;
                        }
                        printf("Your score: %d\n", score);
                        printf("Current high score: %d\n", highScore);

                        if (score > highScore) {
                            printf("Congratulations! You've set a new high score!\n");
                            saveHighScore(score, mode);
                        }

                        printHighScores();
                    } else if (playerMode == '2') {
                        printf("Zombies eliminated by Player 1: %d, Player 2: %d\n", eliminations[0], eliminations[1]);
                    }
                    return 0; // End the game immediately
                }

                // Handle zombie elimination logic
                int result = checkForZombiesAndEliminate(direction, &playerX[player], &playerY[player], maze, playerChar[player], &eliminations[player]);
                if (result == -1) {
                    maze[playerY[player]][playerX[player]] = ' '; // Remove player character from the maze
                    printf("Player %c has died! Game over.\n", playerChar[player]);
                    endsound();
                    return 0; // End the game immediately
                }

                // Move the player based on the direction
                int newPlayerX = playerX[player], newPlayerY = playerY[player];

                switch (direction) {
                    case 'W': case 'w': newPlayerY--; break;
                    case 'A': case 'a': newPlayerX--; break;
                    case 'S': case 's': newPlayerY++; break;
                    case 'D': case 'd': newPlayerX++; break;
                    default:
                        printf("Invalid direction. Try again.\n");
                        printMaze(maze);
                        continue;
                }

                // Check if the new position is valid
                if (maze[newPlayerY][newPlayerX] == ' ' || maze[newPlayerY][newPlayerX] == 'E') {
                    maze[playerY[player]][playerX[player]] = ' '; // Clear the previous player position
                    playerX[player] = newPlayerX;
                    playerY[player] = newPlayerY;
                    maze[playerY[player]][playerX[player]] = playerChar[player]; // Set the new player position
                } else {
                    printf("You can't move there!\n");
                    continue; // Skip the rest of the loop
                }

                // Move the zombies
                moveZombies(maze, zombieX, zombieY, numZombies);

                // Clear the console and print the updated maze
                system("cls"); // Clear screen on Windows; use "clear" on Unix
                printMaze(maze);
            }
        }
    }

    return 0;
}
// sound functions implementation
     
void startsound() {

mciSendString("open \"startsound2.mp3\" type mpegvideo alias myMP3", NULL, 0, NULL);

// Play the mp3 file (non-blocking)
mciSendString("play myMP3", NULL, 0, NULL);

// Wait for user input to stop playback
while (1) {
Sleep (1000); // Pause for 1 second prints
if (kbhit()) {
getchar ();
break;
}
}
// Stop and close the mp3
mciSendString("stop myMP3", NULL, 0, NULL);
mciSendString("close myMP3", NULL, 0,NULL);
}

void endsound() //calling end sound 
{
mciSendString("open \"gameOver.mp3\" type mpegvideo alias myMP3", NULL, 0, NULL);

// Play the mp3 file (non-blocking)
mciSendString("play myMP3", NULL, 0, NULL);

// Wait for user input to stop playback
while (1) {
Sleep (1000); // Pause for 1 second prints
if (kbhit()) {
getchar ();
break;
}
}
// Stop and close the mp3
mciSendString("stop myMP3", NULL, 0, NULL);
mciSendString("close myMP3", NULL, 0,NULL);
}

void winningsound() // winning sound 
{
mciSendString("open \"winning.mp3\" type mpegvideo alias myMP3", NULL, 0, NULL);

// Play the mp3 file (non-blocking)
mciSendString("play myMP3", NULL, 0, NULL);

// Wait for user input to stop playback
while (1) {
Sleep (1000); // Pause for 1 second prints
if (kbhit()) {
getchar ();
break;
}
}
// Stop and close the mp3
mciSendString("stop myMP3", NULL, 0, NULL);
mciSendString("close myMP3", NULL, 0,NULL);
}

void loosingsound() // loosing sound 
{
mciSendString("open \"loosingsound2.mp3\" type mpegvideo alias myMP3", NULL, 0, NULL);

// Play the mp3 file (non-blocking)
mciSendString("play myMP3", NULL, 0, NULL);

// Wait for user input to stop playback
while (1) {
Sleep (1000); // Pause for 1 second prints
if (kbhit()) {
getchar ();
break;
}
}
// Stop and close the mp3
mciSendString("stop myMP3", NULL, 0, NULL);
mciSendString("close myMP3", NULL, 0,NULL);
}
void zombieapproach ()
{
mciSendString("open \"caution.mp3\" type mpegvideo alias myMP3", NULL, 0, NULL);

// Play the mp3 file (non-blocking)
mciSendString("play myMP3", NULL, 0, NULL);

// Wait for user input to stop playback
while (1) {
Sleep (1000); // Pause for 1 second prints
if (kbhit()) {
getchar ();
break;
}
}
// Stop and close the mp3
mciSendString("stop myMP3", NULL, 0, NULL);
mciSendString("close myMP3", NULL, 0,NULL);
}

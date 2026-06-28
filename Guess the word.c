#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define MAX_WORDS 100
#define MAX_WORD_LEN 50

// Structure to store word list
struct WordList {
    char words[MAX_WORDS][MAX_WORD_LEN];
    int count;
};

// Load words from file into the structure
int loadWords(const char *filename, struct WordList *list) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0;
    }

    list->count = 0;
    while (fgets(list->words[list->count], MAX_WORD_LEN, fp) != NULL && list->count < MAX_WORDS) {
        list->words[list->count][strcspn(list->words[list->count], "\n")] = '\0';
        list->count++;
    }

    fclose(fp);
    return 1;
}

// Clear screen
void clearScreen() {
    system(CLEAR);
}

// ASCII art on losing
void displayYouDied(const char *word) {
    printf("\033[1;31m");
    printf("   __     ______  _    _   _      ____   _____ ______ \n");
    printf("   \\ \\   / / __ \\| |  | | | |    / __ \\ / ____|  ____|\n");
    printf("    \\ \\_/ / |  | | |  | | | |   | |  | | (___ | |__   \n");
    printf("     \\   /| |  | | |  | | | |   | |  | |\\___ \\|  __|  \n");
    printf("      | | | |__| | |__| | | |___| |__| |____) | |____ \n");
    printf("      |_|  \\____/ \\____/  |______\\____/|_____/|______|\n");
    printf("\033[0m");
    printf("\nThe word was: '%s'\n", word);
    printf("\n1) Play Again\n2) Exit\n");
}

// ASCII art on winning
void displayYouWon() {
    printf("\033[1;32m");
    printf(" __     ______  _    _   _    _  ____  _   _ \n");
    printf(" \\ \\   / / __ \\| |  | | | |  | |/ __ \\| \\ | |\n");
    printf("  \\ \\_/ / |  | | |  | | | |  | | |  | |  \\| |\n");
    printf("   \\   /| |  | | |  | | | |  | | |  | | . ` |\n");
    printf("    | | | |__| | |__| | | |__| | |__| | |\\  |\n");
    printf("    |_|  \\____/ \\____/   \\____/ \\____/|_| \\_|\n");
    printf("\033[0m");
    printf("\n1) Play Again\n2) Exit\n");
}

// Generate clue with a number of hints
void generateClue(const char *word, char *clue, int hints) {
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        clue[i] = '-';
    }

    int revealed = 0;
    while (revealed < hints) {
        int index = rand() % len;
        if (clue[index] == '-') {
            clue[index] = word[index];
            revealed++;
        }
    }

    clue[len] = '\0';
}

// Update clue with guessed letter
int updateClue(const char *word, char *clue, char guess) {
    int found = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (tolower(word[i]) == guess && clue[i] == '-') {
            clue[i] = word[i];
            found = 1;
        }
    }
    return found;
}

// Check if word is fully guessed
int isComplete(const char *clue) {
    return strchr(clue, '-') == NULL;
}

// Ask if player wants to play again
int playAgain() {
    int choice;
    printf("\nChoose one option: ");
    scanf("%d", &choice);
    return choice == 1;
}

// Ask player for difficulty, set lives and load word list
int chooseDifficulty(int *lives, struct WordList *list, char *difficultyFile) {
    int choice;
    printf("Welcome to Guess A Word\n\nGame Difficulty:\n1) Easy\n2) Medium\n3) Hard\n\nChoose a game difficulty: ");
    scanf("%d", &choice);

    if (choice == 1) {
        *lives = 3;
        strcpy(difficultyFile, "easy.txt");
    } else if (choice == 2) {
        *lives = 6;
        strcpy(difficultyFile, "medium.txt");
    } else if (choice == 3) {
        *lives = 9;
        strcpy(difficultyFile, "hard.txt");
    } else {
        return 0;
    }

    return loadWords(difficultyFile, list);
}

// Main game logic
void playGame(struct WordList *list, int lives, int hintCount) {
    const char *word = list->words[rand() % list->count];
    char clue[MAX_WORD_LEN];
    char input[100];

    generateClue(word, clue, hintCount);

    while (lives > 0) {
        printf("Word: %s\n", clue);
        printf("Lives left: %d\n", lives);
        printf("Guess a letter: ");
        scanf("%s", input);

        char guess = tolower(input[0]);

        if (!isalpha(guess)) {
            clearScreen();
            printf("Please enter a valid letter.\n\n");
            continue;
        }

        if (updateClue(word, clue, guess)) {
            clearScreen();
            if (isComplete(clue)) {
                displayYouWon();
                return;
            }
            printf("Good guess!\n\n");
        } else {
            lives--;
            clearScreen();
            printf("Wrong guess!\n\n");
        }
    }

    clearScreen();
    displayYouDied(word);
}

int main() {
    srand(time(NULL));
    while (1) {
        clearScreen();
        int lives, hintCount;
        struct WordList list;
        char difficultyFile[20];

        if (!chooseDifficulty(&lives, &list, difficultyFile)) {
            printf("Invalid choice or failed to load word file.\n");
            return 1;
        }

        // Set number of hints based on difficulty
        if (strcmp(difficultyFile, "easy.txt") == 0) {
            hintCount = 2;
        } else if (strcmp(difficultyFile, "medium.txt") == 0) {
            hintCount = 3 + rand() % 2; // 3 or 4
        } else if (strcmp(difficultyFile, "hard.txt") == 0) {
            hintCount = 5 + rand() % 2; // 5 or 6
        } else {
            hintCount = 2; // fallback
        }

        clearScreen();
        playGame(&list, lives, hintCount);

        if (!playAgain()) {
            printf("Thanks for playing!\n");
            break;
        }
    }

    return 0;
}

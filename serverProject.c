#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 12165

typedef struct {
    int socket;
    int puzzle[4][4];
} PuzzleArgs;

typedef struct {
    int socket;
    const char* message;
} MessageArgs;

bool checkRow(int puzzle[4][4], int row, int value) {
    printf("\ninCheckRow: ");
    for (int i = 0; i < 4; i++) {
        if (puzzle[row][i] == value) {
            printf("false\n");
            return false;
        }
    }
    printf("true\n");
    return true;
}

bool checkColumn(int puzzle[4][4], int column, int value) {
    printf("inCheckColumn: ");
    for (int i = 0; i < 4; i++) {
        if (puzzle[i][column] == value) {
            printf("false\n");
            return false;
        }
    }
    printf("true\n");
    return true;
}

bool checkSquare(int puzzle[4][4], int startRow, int startColumn, int value) {
    printf("inCheckSquare: \n");
    for (int i = startRow; i < startRow + 2; i++) {
        for (int j = startColumn; j < startColumn + 2; j++) {
            printf("puzzle Row: %d Column: %d ", i, j);
            if (puzzle[i][j] == value) {
                printf("checkSquare: false\n");
                return false;
            }
            printf("true\n");
        }
    }
    printf("checkSquare: true\n");
    return true;
}

bool validMove(int puzzle[4][4], int row, int column, int value) {
    printf("inValidMove: ");
    if (checkRow(puzzle, row, value) && checkColumn(puzzle, column, value) &&
        checkSquare(puzzle, row - row % 2, column - column % 2, value)) {
        printf("validMove: true\n");
        return true;
    }
    printf("validMove: false\n");
    return false;
}

bool puzzleSolved(int puzzle[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (puzzle[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

void printPuzzle(int puzzle[4][4]) {
    printf("\nCurrent puzzle state:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", puzzle[i][j]);
        }
        printf("\n");
    }
}

void* sendPuzzle(void* args) {
    //รับอาร์กิวเมนต์ args ที่ถูกส่งมาตอนสร้าง thread และแปลงเป็นประเภท PuzzleArgs*.
    PuzzleArgs* puzzleArgs = (PuzzleArgs*)args;
    // ส่งข้อมูลแต่ละแถว
    for (int i = 0; i < 4; i++) {
        send(puzzleArgs->socket, puzzleArgs->puzzle[i], sizeof(int) * 4, 0);
    }
    pthread_exit(NULL);
}

void* sendMessage(void* args) {
    MessageArgs* messageArgs = (MessageArgs*)args;
    send(messageArgs->socket, messageArgs->message, strlen(messageArgs->message), 0);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int puzzle[4][4] = { 
        {0, 3, 4, 0}, 
        {4, 0, 0, 2}, 
        {1, 0, 0, 3}, 
        {0, 2, 1, 0} 
    };

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    //ประกาศตัวแปรสร้าง thread ใหม่
    pthread_t puzzleThread;
    PuzzleArgs puzzleArgs = { .socket = new_socket };
    //คัดลอกข้อมูลจากตัวแปร puzzle
    memcpy(puzzleArgs.puzzle, puzzle, sizeof(puzzle));
    //สร้าง thread ใหม่่เรียกใช้ sendPuzzle
    pthread_create(&puzzleThread, NULL, sendPuzzle, (void*)&puzzleArgs);
    //รอให้ thread ทำงานเสร็จ
    pthread_join(puzzleThread, NULL);

    while (1) {
        int row, column, value;
        if (read(new_socket, &row, sizeof(int)) < 0 || read(new_socket, &column, sizeof(int)) < 0 || read(new_socket, &value, sizeof(int)) < 0) {
            perror("read");
            close(new_socket);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (puzzle[row - 1][column - 1] != 0) {
            send(new_socket, "Position already filled", strlen("Position already filled"), 0);
            continue;
        }

        if (validMove(puzzle, row - 1, column - 1, value)) { 
            puzzle[row - 1][column - 1] = value;
            if (puzzleSolved(puzzle)) {
                pthread_t congratsThread;
                MessageArgs congratsArgs = { .socket = new_socket, .message = "Congratulations!" };
                pthread_create(&congratsThread, NULL, sendMessage, (void*)&congratsArgs);
                pthread_join(congratsThread, NULL);
                break; 
            } else {
                send(new_socket, "Valid move", strlen("Valid move"), 0);
            }
            printf("Move is valid: (%d, %d) = %d\n\n", row, column, value);
            printPuzzle(puzzle);
        } else {
            send(new_socket, "Invalid move", strlen("Invalid move"), 0);
            printf("Move is invalid: (%d, %d) = %d\n\n", row, column, value);
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}

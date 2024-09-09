#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12165

void printPuzzle(int puzzle[4][4]) {
    int i, j, a;
    printf("\n");
    printf(" 0 | 1  2 | 3  4 | X\n");
    printf(" -----------------\n");
    for (i = 0, a = 1; i < 4; i++, a++) {
        for (j = 0; j < 4; j++) {
            if (j == 0)
                printf(" %d |", a);
            else if ((j) % 2 == 0)
                printf("|");
            printf(" %d ", puzzle[i][j]);
            if (j == 3)
                printf("|");
        }
        printf("\n");
        if ((i + 1) % 2 == 0)
            printf(" -----------------\n");
    }
    printf(" Y\n");
}

void *handle_server(void *sock_desc) {
    int sock = *(int*)sock_desc;
    int puzzle[4][4] = {0};

    for (int i = 0; i < 4; i++) {
        if (read(sock, puzzle[i], sizeof(int) * 4) < 0) {
            perror("Read error");
            close(sock);
            pthread_exit(NULL);
        }
    }

    printPuzzle(puzzle);

    while (1) {
        char input[100];
        printf("Enter row, column, value (or 'q' to exit): ");
        fgets(input, sizeof(input), stdin);

        if (input[0] == 'q' || input[0] == 'Q') {
            break; 
        }

        int row, column, value;
        if (sscanf(input, "%d %d %d", &row, &column, &value) != 3) {
            printf("Invalid input. Please enter: row column value\n");
            continue;
        }

        if (row < 1 || row > 4 || column < 1 || column > 4 || value < 1 || value > 4) {
            printf("Values between 1 and 4. Please try again\n");
            continue;
        }

        if (send(sock, &row, sizeof(int), 0) < 0 || send(sock, &column, sizeof(int), 0) < 0 || send(sock, &value, sizeof(int), 0) < 0) {
            perror("Send error");
            close(sock);
            pthread_exit(NULL);
        }
        
        char buffer[1024] = {0};
        if (read(sock, buffer, 1024) < 0) {
            perror("Read error");
            close(sock);
            pthread_exit(NULL);
        }
        printf("Server response: %s\n", buffer);

        if (strcmp(buffer, "Valid move") == 0) {
            puzzle[row - 1][column - 1] = value; 
            printPuzzle(puzzle);
        } else if (strcmp(buffer, "Congratulations!") == 0) {
            puzzle[row - 1][column - 1] = value; 
            printPuzzle(puzzle);
            printf("Congratulations! You have solved the puzzle!\n");
            break; 
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    pthread_t server_thread;
    int *new_sock = malloc(sizeof(int));
    *new_sock = sock;
    if (pthread_create(&server_thread, NULL, handle_server, (void*) new_sock) < 0) {
        perror("pthread_create");
        free(new_sock);
        return -1;
    }

    pthread_join(server_thread, NULL);
    free(new_sock);

    return 0;
}
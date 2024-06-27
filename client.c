
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8091
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char request[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server\n");

    double initial_velocity, launch_angle;

    do {
        printf("Enter initial velocity or -1 to exit: ");
        scanf("%lf", &initial_velocity);
        if (initial_velocity == -1) {
            break;
        } else if (initial_velocity < 0 && initial_velocity > -1 || initial_velocity < -1) {
            printf("The initial velocity is a negative number, change input\n\n");
            continue;
        }
        printf("Enter launch angle: ");
        scanf("%lf", &launch_angle);
        if (launch_angle < 0 || launch_angle > 180) {
            printf("The launch angle does not match the conditions, change input\n\n");
            continue;
        }

        snprintf(request, sizeof(request), "GET /calculate?velocity=%lf&angle=%lf HTTP/1.1\nHost: 127.0.0.1\n\n", initial_velocity, launch_angle);
        send(sock, request, strlen(request), 0);

        read(sock, buffer, BUFFER_SIZE);
        printf("Server response:\n%s\n", buffer);

        printf("If you want to post the result of the recorded simulation data insert 1, otherwise 0: ");
        int post;
        scanf("%d", &post);
        if (post == 1) {
            snprintf(request, sizeof(request), "POST /store HTTP/1.1\nHost: 127.0.0.1\n\n%s", buffer);
            send(sock, request, strlen(request), 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("Server response:\n%s\n", buffer);
        } else if (post == 0) {
            printf("The response has not been posted\n");
        } else {
            printf("Posting operation failed\n");
        }
    } while (1);

    close(sock);
    return 0;
}

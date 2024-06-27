#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8091
#define BUFFER_SIZE 1024
#define PI 3.14159265
#define GRAVITY 9.81

typedef struct {
    double initial_velocity;
    double launch_angle;
} ProjectileParams;

typedef struct {
    double max_height;
    double time_of_flight;
    double range;
} ProjectileResults;

typedef struct {
    ProjectileParams params;
    ProjectileResults results;
} SimulationData;

SimulationData stored_data[100];
int stored_data_count = 0;

ProjectileResults calculate_trajectory(ProjectileParams params) {
    ProjectileResults results;
    double v0 = params.initial_velocity;
    double angle_rad = params.launch_angle * PI / 180.0;

    results.max_height = pow(v0 * sin(angle_rad), 2) / (2 * GRAVITY);
    results.time_of_flight = (2 * v0 * sin(angle_rad)) / GRAVITY;
    results.range = (pow(v0, 2) * sin(2 * angle_rad)) / GRAVITY;

    return results;
}

void handle_get_request(int new_socket, char *buffer) {
    ProjectileParams params;
    sscanf(buffer, "GET /calculate?velocity=%lf&angle=%lf", &params.initial_velocity, &params.launch_angle);

    ProjectileResults results = calculate_trajectory(params);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nMax Height: %lf\nTime of Flight: %lf\nRange: %lf\n",
             results.max_height, results.time_of_flight, results.range);

    send(new_socket, response, strlen(response), 0);
}

void handle_post_request(int new_socket, char *buffer) {
    ProjectileParams params;
    ProjectileResults results;

    sscanf(buffer, "POST /store HTTP/1.1\n\nInitial Velocity: %lf\nLaunch Angle: %lf\nMax Height: %lf\nTime of Flight: %lf\nRange: %lf\n",
           &params.initial_velocity, &params.launch_angle, &results.max_height, &results.time_of_flight, &results.range);

    if (stored_data_count < 100) {
        stored_data[stored_data_count].params = params;
        stored_data[stored_data_count].results = results;
        stored_data_count++;
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nData stored successfully.\n");
        send(new_socket, response, strlen(response), 0);
    } else {
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\n\nStorage limit reached.\n");
        send(new_socket, response, strlen(response), 0);
    }
}

void handle_retrieve_request(int new_socket) {
    char response[BUFFER_SIZE];
    char *response_ptr = response;
    int offset = snprintf(response_ptr, BUFFER_SIZE, "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n");

    for (int i = 0; i < stored_data_count; ++i) {
        offset += snprintf(response_ptr + offset, BUFFER_SIZE - offset,
                           "Initial Velocity: %lf, Launch Angle: %lf, Max Height: %lf, Time of Flight: %lf, Range: %lf\n",
                           stored_data[i].params.initial_velocity,
                           stored_data[i].params.launch_angle,
                           stored_data[i].results.max_height,
                           stored_data[i].results.time_of_flight,
                           stored_data[i].results.range);
    }

    send(new_socket, response, strlen(response), 0);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

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

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, BUFFER_SIZE);
        if (strncmp(buffer, "GET /calculate", 14) == 0) {
            handle_get_request(new_socket, buffer);
        } else if (strncmp(buffer, "POST /store", 11) == 0) {
            handle_post_request(new_socket, buffer);
        } else if (strncmp(buffer, "GET /retrieve", 13) == 0) {
            handle_retrieve_request(new_socket);
        }
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

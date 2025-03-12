#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int host_html() {
    FILE *html_data = fopen("index.html", "r");
    if (!html_data) {
        perror("Failed to open index.html");
        return 1;
    }

    char response_data[4096] = {'\0'};
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), html_data)) {
        strcat(response_data, buffer);
    }
    fclose(html_data);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        return 1;
    }

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        send(client_socket, response_data, strlen(response_data), 0);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

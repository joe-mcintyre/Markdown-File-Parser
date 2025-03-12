#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "md_conversion.h"
#include "md_parsing.h"


char* convert_to_html_exten(const char* md_filename);
bool valid_arguments(int argc, char** argv);
bool only_contains_periods(const char *str);
int host_html(char* html_filename);

int main(int argc, char** argv) {

    if (!valid_arguments(argc, argv)) return -1;

    char* filename = argv[1];
    char* html_filename = convert_to_html_exten(filename);
    FILE* html_file;


    FILE* md_file = fopen(filename, "r");
    if (!md_file) {
        perror("Error opening file");
        return -1;
    }

    int md_file_line_count = count_file_lines(md_file);


    char **lines = convert_file_to_array(md_file);
    fclose(md_file);


    char** html_arr = convert_to_html(lines, md_file_line_count);

    FILE* file = fopen(html_filename, "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    // Write basic HTML structure
    // Write basic HTML structure with enhanced inline CSS
    fprintf(file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Markdown Output</title>\n");
    fprintf(file, "<style>\n"
                  "body { background-color: #343541; color: white; font-family: Arial, sans-serif; }\n"
                  "h1 { color: #b8bb26; } /* Gruvbox green */\n"
                  "h2 { color: #fabd2f; } /* Gruvbox yellow */\n"
                  "h3 { color: #fe8019; } /* Gruvbox orange */\n"
                  "h4 { color: #d3869b; } /* Gruvbox pink */\n"
                  "h5 { color: #83a598; } /* Gruvbox blue */\n"
                  "h6 { color: #8ec07c; } /* Gruvbox aqua */\n"
                  "pre, code { background-color: black; color: #b8bb26; padding: 5px; border-radius: 4px; }\n"
                  "pre { padding: 10px; border: 1px solid #b8bb26; overflow-x: auto; }\n"
                  "</style>\n</head>\n<body>\n");

    // Write parsed HTML content
    for (int i = 0; i < md_file_line_count; i++) {
        if (html_arr[i]) { // Ensure it's not NULL
            fprintf(file, "%s\n", html_arr[i]);
        }
    }
    // Close body and HTML tags
    fprintf(file, "</body>\n</html>\n");
    // Close the file
    fclose(file);


    printf("Markdown parsing successful. Html output now displaying on localhost:8080\n");
    int o = host_html(html_filename);

    return 0;
}

bool valid_arguments(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument count\n");
        return false;
    }
    char* filename = argv[1];
    if (only_contains_periods(filename)) {
        fprintf(stderr, "Invalid filename\n");
        return false;
    }
    if (access(filename, F_OK) != 0) {
        fprintf(stderr, "File cannot be found\n");
        return false;
    }
    return true;
}

bool only_contains_periods(const char *str) {
    if (str == NULL || *str == '\0') return true;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] != '.') {
            return false;
        }
    }
    return true;
}

char* convert_to_html_exten(const char* md_filename) {
    char* md_copy = strdup(md_filename);
    if (!md_copy) {
        perror("mem allocation failed");
        exit(EXIT_FAILURE);
    }
    char* filename = strtok(md_copy, ".");
    if (!filename) {
        free(md_copy);
        perror("file name does not exist");
        exit(EXIT_FAILURE);
    }

    size_t filename_size = strlen(filename) + strlen(".html") + 1;
    char* html_filename = malloc(filename_size);

    if (!html_filename) {
        perror("mem allocation failed");
        free(md_copy);
        exit(EXIT_FAILURE);
    }

    snprintf(html_filename, filename_size, "%s.html", filename);
    free(md_copy);
    return html_filename;
}

int host_html(char* html_filename) {
    FILE *html_data = fopen(html_filename, "r");
    if (!html_data) {
        perror("Failed to open output.html");
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

        // Construct the HTTP response with proper headers
        char http_response[5000];
        snprintf(http_response, sizeof(http_response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=UTF-8\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 strlen(response_data), response_data);

        send(client_socket, http_response, strlen(http_response), 0);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

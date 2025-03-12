#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include "md_parsing.h"

int host_html();

int main() {
    //char* md_arr[] = { "this should be a heading", "===", "# heading", "## heading", "### heading3", "----", "* ham", "* cheese", "##gogo", "* swiss",
     //"1. tsauce", "this is _bold_ text", "this is __em__ text", "this is ___em___ text", "2. bsauce", "* dsad", "3. lsauce", "> bq1", "> bq2", ">>>>> bq333", "egghead"};
    //char* md_arr[] = { "this is _em_ text", "this is __strong__ text ", "this is ___emstrong___ text", "dsad", "salad", "# heading"};
 //   char* md_arr[] = {"3 underscore ___11234___", " 2 underscore __ballball__", "1 underscore _bal_", "_as_", "_ass_ _a", "egg ham _ ds", "eas", "__ds__ ___ds___", "___a___", "eggo _sam_", "___sf___ _em_ __strong__ ___emstrong___", "# heading", "_________ _ _", "**ds** ***ds***", "***a***", "eggo *sam*", "***sf*** *em* __strong__ ***emstrong***", "``` code", "int x = 3;", "printf(\"_%d_\", x);", "# heading", "```", "code is over", "new code ```char** string_in_c``` code over", "this is a <validurl.com>", " this is not <valid url.com>", "this is valid <url<<.com>", "this is valid <url_inc_adopt.com>", "this is valid <url``.com>", "this is valid <url<url.com>inside.com>"};
 char * md_arr[] = {"# Heading 1", "# Heading2 ", "this is some text & its _emphasied_", "this next one is __strong text__", "while this one is ___strong & bold___", "heres a lil code sample", "```printf(some text);", "int x = 100;", "```", "THis is my website <www.fuckoff.com>"};
int arr_size = sizeof(md_arr)/sizeof(md_arr[0]);

    char** html_arr = convert_to_html(md_arr, arr_size);

    printf("HTML_ARR CREATED & CONVERTED\n");

    int i;
    for (i=0; i < arr_size; i++) {
        printf("LINE: %s\n", html_arr[i]);
    }

    // Open an HTML file for writing
    FILE* file = fopen("output.html", "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    // Write basic HTML structure
    fprintf(file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Markdown Output</title>\n</head>\n<body>\n");

    // Write parsed HTML content
    for (int i = 0; i < arr_size; i++) {
        if (html_arr[i]) { // Ensure it's not NULL
            fprintf(file, "%s\n", html_arr[i]);
        }
    }
    // Close body and HTML tags
    fprintf(file, "</body>\n</html>\n");
    // Close the file
    fclose(file);

    printf("HTML written to output.html\n");

    int o = host_html();

    return 0;
}
int host_html() {
    FILE *html_data = fopen("output.html", "r");
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


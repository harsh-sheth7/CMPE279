// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char *user = "nobody";
    struct passwd *password;
    pid_t processId;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    pid_t childProcess = fork();
    if (childProcess < 0)
    {
        perror("Fork Process Failed");
        exit(EXIT_FAILURE);
    }
    else if (childProcess == 0)
    {
        password = getpwnam(user);
        if (password == NULL)
        {
            perror("User nobody: not found");
            exit(EXIT_FAILURE);
        }
        processId = password->pw_uid;
        if (setuid(processId) < 0)
        {
            perror("Previlege Separation Failed");
            exit(EXIT_FAILURE);
        }
        valread = read(new_socket, buffer, 1024);
        if (valread < 0)
        {
            perror("Read Failed");
            exit(EXIT_FAILURE);
        }
        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }
    wait(NULL);

    return 0;
}

// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <assert.h>
#define PORT 8080

void fork_process(int server_fd, int new_socket, int valread, char buffer[1024],
                  char *hello, struct sockaddr_in address, int addrlen);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
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

    fork_process(server_fd, new_socket, valread, buffer, hello, address, addrlen);
    return 0;
}

void fork_process(int server_fd, int new_socket, int valread, char buffer[1024],
                  char *hello, struct sockaddr_in address, int addrlen)
{

    pid_t childPID, wpid;

    childPID = fork();
    int status = 0;

    if (childPID >= 0) // fork successful
    {
        printf("Fork successful\n");
        if (childPID == 0) // child process
        {
            printf("Inside child process\n");
            //drop privileges here
            struct passwd *pw = getpwnam("nobody");
            assert(pw != NULL);

            if (pw == NULL)
            {
                printf("Failed to drop privileges since pw is null\n");
                exit(EXIT_FAILURE);
            }

            if (setuid(pw->pw_uid) != 0)
            {
                printf("Failed to drop privileges since setuid not 0\n");
                exit(EXIT_FAILURE);
            }
            printf("Successfully dropped privileges\n");
            //running data processing from client
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            valread = read(new_socket, buffer, 1024);
            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
        }
        // else //Parent process
        // {
        //     //parent process waits
        // }
        //parent process waits
        while ((wpid = wait(&status)) > 0)
            ;
    }
    else // fork failed
    {
        printf("\n Fork has failed\n");
        exit(EXIT_FAILURE);
    }
}
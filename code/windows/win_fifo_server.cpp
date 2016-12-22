#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    bool is_success;
    // Buffer for name of and path to fifo file
    char fifo_name[256];
    char fifo_path[256];
    printf("Enter fifo name: ");
    scanf("%200s", fifo_name);
    sprintf(fifo_path, "\\\\.\\pipe\\%s", fifo_name);

    // Handler for fifo
    HANDLE hFifo;

    // Create fifo
    hFifo = CreateNamedPipe(fifo_path, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_WAIT,
                                1, 1000, 1000, 100, NULL);
    if (hFifo == INVALID_HANDLE_VALUE)
    {
        printf("Failed to create fifo: %s\n", fifo_name);
        system("pause");
        return 1;
    }
    printf("Server started, fifo: %s\n", fifo_name);
   
    // Try to connect to client
    printf("Start waiting for client\n");
    is_success = ConnectNamedPipe(hFifo, NULL);
    if (is_success == false)
    {
        printf("Waiting of client is failed\n");
        system("pause");
        return 1;
    }
    printf("Client is connected\n");

    // Communication with client
    int message;
    unsigned long n_bytes;
    while (true)
    {
        // Read from fifo
        is_success = ReadFile(hFifo, &message, sizeof(int), &n_bytes, NULL);
        if (is_success == false || n_bytes!= sizeof(int))
        {
            printf("Failed to read from fifo\n");
            break;
        }

        if (message == -1)
        {
            printf("Server has received stop message\n");
            break;
        }

        printf("Received: %d\n", message);
    }

    // Disconnect client
    DisconnectNamedPipe(hFifo);
    // Release fifo resources
    CloseHandle(hFifo);

    printf("Server stopped\n");
    system("pause");
    return 0;
}
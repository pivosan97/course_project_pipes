#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STOP_MESSAGE -1

int main(int argc, char **argv)
{
    // Buffer for name of fifo file
    char fifo_name[256];
    printf("Enter fifo name (full path) ");
    scanf("%255s", fifo_name);

    // Try to open fifo for writing
    int fifo_handler = open(fifo_name, O_WRONLY, 0);
    if(fifo_handler < 0)
    {
        printf("Failed to open fifo: %s\n", fifo_name);
        return 1;
    }
    printf("Client is connected to fifo: %s\n", fifo_name);

    // Communication with server
    int message;
    int n_bytes;
    while(true)
    {
        printf("Sent (message or %d to stop): ", STOP_MESSAGE);
        scanf("%d", &message);

        // Write to fifo
        n_bytes = write(fifo_handler, &message, sizeof(message));
        if(n_bytes != sizeof(message))
        {
            printf("Failed to write to fifo\n");
            break;
        }

        if(message == STOP_MESSAGE)
        {
            printf("Communication is stopped by user\n");
            break;
        }
    }

    // Close fifo hanler
    close(fifo_handler);

    printf("Client is stopped\n");
    return 0;
}

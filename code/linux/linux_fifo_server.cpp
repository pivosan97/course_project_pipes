#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#define STOP_MESSAGE -1

int main(int argc, char **argv)
{
    // Buffer for fifo file name
    char fifo_name[256];
    printf("Enter fifo name (full path) ");
    scanf("%s", fifo_name);

    // Create fifo file
    if(mkfifo(fifo_name, S_IRWXO | S_IRWXG | S_IRWXU) != 0)
    {
        printf("Failed to create fifo: %s\n", fifo_name);
        return 1;
    }

    printf("Server started, fifo: %s\n", fifo_name);

    // Try to open fifo for reading, blocked until isn`t opened at other side
    printf("Start waiting for client\n");
    int fifo_handler = open(fifo_name, O_RDONLY, 0);
    if(fifo_handler < 0)
    {
        printf("Failed to open fifo: %s\n", fifo_name);
        // Release fifo resources
        close(fifo_handler);
        unlink(fifo_name);
        return 1;
    }
    printf("Client is connected\n");

    // Communication with client
    int message;
    int n_bytes;
    while(true)
    {
        // Read from fifo
        n_bytes = read(fifo_handler, &message, sizeof(int));
        if(n_bytes <= 0)
        {
            printf("Failed to read from fifo\n");
            break;
        }
   
        if(message == STOP_MESSAGE)
        {
            printf("Server have received stop message\n");
            break;
        }

        printf("Received: %d\n", message);
    }

    // Release fifo resources
    close(fifo_handler);
    unlink(fifo_name);

    printf("Server is stopped\n");
    return 0;
}

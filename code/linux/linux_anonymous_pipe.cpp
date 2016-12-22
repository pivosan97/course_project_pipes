#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define STOP_MESSAGE -1

int main(int argc, char ** argv)
{
    pid_t child_pid;
    // Pair anonymous pipe handlers
    int pipe_handlers[2];
    int *write_handler_ptr = &(pipe_handlers[1]);
    int *read_handler_ptr = &(pipe_handlers[0]);

    // Create anonymous pipe
    pipe(pipe_handlers);
    
    // Create child process
    if((child_pid = fork()) == -1)
    {
        printf("Failed to create child process");
        return 1;
    }
    else if(child_pid == 0)
    {
        // Child process
        printf("Child process is started\n");

        // Close excess anonymous pipe write handler
        close(*write_handler_ptr);

        // Communicatoion with parent process
        int message;
        int n_bytes;
        while(true)
        {
            // Read from pipe
            n_bytes = read(*read_handler_ptr, &message, sizeof(int));
            if(n_bytes <= 0)
            {
                printf("Failed to read from anonymous pipe\n");
                return 1;
            }

            if(message == STOP_MESSAGE)
            {
                printf("Child process has received stop message\n");
                break;
            }

            printf("Child output (multiply by -1): %d\n", -1 * message);
        }

        // Close anonymous pipe read handler
        close(*read_handler_ptr);

        printf("Child process is finished\n");
    }
    else
    {
        // Parent
        printf("Parent process is started\n");

        // Close excess anonypous pipe read handler
        close(*read_handler_ptr);

        // Communication with child process
        int message;
        int n_bytes;
        while(true)
        {
            // Some delay: 10ms // To separate processes outputs
            usleep(1000 * 10);

            printf("Parent input (message or %d to stop): ", STOP_MESSAGE);
            scanf("%d", &message);

            // Write to pipe
            n_bytes = write(*write_handler_ptr, &message, sizeof(int));
            if(n_bytes <= 0)
            {
                printf("Failed to write to anonymous pipe\n");
                break;
            }

            if(message == STOP_MESSAGE)
            {
                printf("Communication is stopped by user\n");
                break;
            }
        }

        // Close anonymous pipe write handler, reading from pipe will be stopped
        close(*write_handler_ptr);

        printf("Waiting until child process is not finished\n");
        waitpid(child_pid, NULL, 0);

        printf("Parent process is finished\n");
    }

    return 0;
}

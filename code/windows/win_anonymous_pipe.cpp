#include <stdio.h>
#include <string.h>
#include <windows.h>

#define STOP_MESSAGE -1

int main(int argc, char ** argv)
{
    int message;
    unsigned long n_bytes;
    bool is_success;

    if (argc == 1)
    {
        // Parent process // There are not any command line parameters
        printf("Parent process is started\n");

        // Read and write anonymous pipe handlers
        HANDLE hReadPipe, hWritePipe;
        // Set pipe configuration, that handlers will be inheritable
        SECURITY_ATTRIBUTES PipeSA;
        PipeSA.bInheritHandle = TRUE;
        PipeSA.lpSecurityDescriptor = NULL;
        PipeSA.nLength = sizeof(SECURITY_ATTRIBUTES);

        // Create anonymous pipe
        is_success = CreatePipe(&hReadPipe, &hWritePipe, &PipeSA, 0);
        if (is_success == false)
        {
            printf("Failed to create anonymous pipe: %d\n", GetLastError());
            system("pause");
            return 1;
        }
        printf("Anonimous pipe is created\n");

        // Set child process configurations
        char CommandLine[MAX_PATH] = { '\n' };
        strcat(CommandLine, argv[0]);
        strcat(CommandLine, " Client");
        PROCESS_INFORMATION ProcInfo;
        STARTUPINFO StartInfo;
        GetStartupInfo(&StartInfo);
        // Transfer anonymous pipe read and write handler to child process
        StartInfo.hStdInput = hReadPipe;
        StartInfo.hStdError = hWritePipe;
        StartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        StartInfo.dwFlags = STARTF_USESTDHANDLES;

        // Create child process from same executable (something like Unix fork())
        is_success = CreateProcess(argv[0], (LPTSTR)CommandLine, NULL, NULL, TRUE, 0,
            NULL, NULL, &StartInfo, &ProcInfo);
        if (is_success == false)
        {
            printf("Failed to create child process: %d\n", GetLastError());
            system("pause");
            return 1;
        }
        // Child process handler
        HANDLE hChildProcess = ProcInfo.hProcess;
        printf("Child process is created\n");

        // Close excess anonymous pipe read handler
        CloseHandle(hReadPipe);

        // Communication with child process
        while (true)
        {
            Sleep(100); // Some delay: 100 ms

            printf("Parent input (enter message or %d to stop): ", STOP_MESSAGE);
            scanf("%d", &message);

            // Write to pipe
            is_success = WriteFile(hWritePipe, &message, sizeof(int), &n_bytes, NULL);
            if (is_success == false || n_bytes != sizeof(int))
            {
                printf("Failed to write to anonymous pipe\n");
                break;
            }

            if (message == STOP_MESSAGE)
            {
                printf("Communication is stopped by user\n");
                break;
            }
        }

        // Close anonymous pipe write handler, reading from file will be stopped
        CloseHandle(hWritePipe);

        printf("Waiting until child process is not finished\n");
        WaitForSingleObject(hChildProcess, INFINITE);

        printf("Parent process is finished\n");
        system("pause");
    }
    else if (argc == 2 && strcmp(argv[1], "Client") == 0)
    {
        // Child process // There is one command line parameter and is is equal to "Client"
        printf("Child process is started\n");

        // Get anonymous pipe read and write handlers
        HANDLE hReadPipe = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE hWritePipe = GetStdHandle(STD_ERROR_HANDLE);

        // Close excess anonymos pipe write handler
        CloseHandle(hWritePipe);

        // Communication with parent process
        while (true)
        {
            // Read from pipe
            is_success = ReadFile(hReadPipe, &message, sizeof(int), &n_bytes, NULL);
            if (is_success == false || n_bytes != sizeof(int))
            {
                printf("Failed to read from anonymous pipe\n");
                return 1;
            }

            if (message == STOP_MESSAGE)
            {
                printf("Chiled process has received stop message\n");
                break;
            }

            printf("Child output (multiply by -1): %d\n", -1 * message);
        }

        // Close anonymous pipe read handler
        CloseHandle(hReadPipe);

        printf("Child process is finished\n");
    }
    else
    {
        printf("Invalid parameters");
        return 1;
    }

    return 0;
}
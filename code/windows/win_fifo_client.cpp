#include <stdio.h>
#include <windows.h>
#include <string.h>

int main(int argc, char **argv)
{
    bool is_success;
	// Buffers for name of and full path to fifo file
	char fifo_name[256];
    char fifo_path[256];
	printf("Enter fifo name: ");
	scanf("%200s", fifo_name);
    sprintf(fifo_path, "\\\\.\\pipe\\%s", fifo_name);

    // Handler for fifo
    HANDLE hFifo;

	// Wait available fifo
    is_success = WaitNamedPipe(fifo_path, 100);
	if (is_success == false)
	{
		printf("Failed to find available fifo: %s\n", fifo_name);
        system("pause");
		return 1;
	}

    // Open fifo for writing
    hFifo = CreateFile(fifo_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFifo == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open fifo: %s\n", fifo_name);
        system("pause");
        return 1;
    }
    printf("Client is connected to fifo: %s\n", fifo_name);

    // Communication with server
	int message;
	unsigned long n_bytes;
	while (true)
	{
		printf("Sent (message or -1 to stop): ");
		scanf("%d", &message);

		// Write to fifo
		is_success = WriteFile(hFifo, &message, sizeof(message), &n_bytes, NULL);
		if (is_success == false || n_bytes != sizeof(int))
		{
			printf("Failed to write to fifo\n");
			break;
		}

		if (message == -1)
		{
			printf("Communication is stopped by user\n");
			break;
		}
	}

    // Close fifo handle
    CloseHandle(hFifo);

    printf("Client stopped\n");
    system("pause");
	return 0;
}
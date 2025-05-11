#include "fileSafety.h"

void SafeWriteFile(void *buffer, size_t size, size_t count, FILE *file)
{
    if (buffer == NULL || file == NULL)
    {
        fprintf(stderr, "Error: Null pointer passed to SafeWriteFile.\n");
        exit(EXIT_FAILURE);
    }

    // Perform the write
    size_t elementsWritten = fwrite(buffer, size, count, file);

    // Check for errors or incomplete writes
    if (elementsWritten != count)
    {
        if (ferror(file))
            perror("Error during fwrite");

        exit(EXIT_FAILURE);
    }
}
void SafeReadFile(void *buffer, size_t size, size_t count, FILE *file)
{
    if (buffer == NULL || file == NULL)
    {
        fprintf(stderr, "Error: Null pointer passed to SafeReadFile.\n");
        exit(EXIT_FAILURE);
    }

    // Perform the read
    size_t elementsRead = fread(buffer, size, count, file);

    // Check for errors or incomplete reads
    if (elementsRead != count && ferror(file))
    {
        perror("Error during fread.\n");
        exit(EXIT_FAILURE);
    }
}
void* SafeMalloc(size_t size)
{
    void *ptr;

    ptr = malloc(size);

    if (ptr == NULL)
    {
        fprintf(stderr, "Error allocating memory.\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

#include "vinaC.h"

/*
 * Create the .vc file, and write the directory on it.
 * name: Name of the file;
*/
void CreateArchive(char* name)
{
    FILE* archive;
    struct Directory header = {0};

    archive = fopen(name, "wb+");
    if (archive == NULL)
    {
        fprintf(stderr, "Error creating binary file");
        return;
    }
    
    header.quantity = 0;
    header.name = 'e';
    fwrite(&header, 1, sizeof(struct Directory), archive);

    fclose(archive);
}

/*
 * Open the specified file.
 * It can create the file in case it doesn't exist,
 * Or stop the program and give it a error.
 * name: Name of the file;
 * toCreate: Integer value that control if the archive can be created or no;
*/
FILE* OpenArchive(char* name, int toCreate)
{
    FILE* binaryArchive;

    if(name == NULL)
    {
        fprintf(stderr, "Error when getting the argument\n");
        exit(EXIT_FAILURE);
    }
    
    /* If the archive doesn't exist*/
    if(access(name, F_OK) == -1)
    {
        if(toCreate)
            CreateArchive(name);
        else
        {
            fprintf(stderr, "The file doesn't exist\n");
            exit(EXIT_FAILURE);
        }
    }

    binaryArchive = fopen(name, "rb+");
    if(binaryArchive == NULL)
    {
        fprintf(stderr, "Error when opening the binary archive\n");
        exit(EXIT_FAILURE);
    }
    
    return binaryArchive;
}

int main(int argc, char **argv)
{
    char nextOption;
    
    char* binaryName;
    FILE* binaryArchive;
    FILE* archive;

    if(argc < 3)
    {
        printf("Usage: %s -h to display information about the program\n", argv[0]);
        return 1;
    }

    while ((nextOption = getopt(argc, argv, ":c:i:m:p:r:x:h")) != -1) {
        switch (nextOption)
        {
            case 'h':
                /* Help */
                ExplainProg();
                break;

            case 'p':
                /* Insert without compression */
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 1);

                while (optind < argc)
                {
                    char arg[NAME_SIZE];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Inserting %s without compression\n", arg);

                    archive = fopen(arg, "rb");
                    
                    InsertNormalArchive(archive, binaryArchive, arg);

                    fclose(archive);
                }
                
                free(binaryName);
                fclose(binaryArchive); 
                break;
            
            case 'i':
                /* Insert with compression */
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 1);

                while (optind < argc)
                {
                    char arg[NAME_SIZE];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Inserting %s with compression.\n", arg);

                    archive = fopen(arg, "rb");
                    
                    InsertCompressedArchive(archive, binaryArchive, arg);

                    fclose(archive);
                    
                }

                free(binaryName);
                fclose(binaryArchive); 
                break;

            case 'c':
                /* List members */
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 0);

                ListMembers(binaryArchive);
                free(binaryName);
                fclose(binaryArchive); 
                break;

            case 'x':
                /* Extract members */
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 0);

                /* No arguments - extract all the members */
                if (optind == argc)
                    ExtractAllArchives(binaryArchive);
                else 
                {
                    while (optind < argc)
                    {
                        char arg[NAME_SIZE];

                        strncpy(arg, argv[optind++], sizeof(arg) -1);
                        printf("Extracting: %s\n", arg);

                        ExtractArchive(arg, binaryArchive);
                    }
                }

                free(binaryName);
                fclose(binaryArchive);
                break;

            case 'm':
                /* Move the member */
                int argCount;
                char member1[NAME_SIZE];
                char member2[NAME_SIZE]; 

                argCount = 0;
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 0);

                if(optind < argc)
                {
                    strncpy(member1, argv[optind++], sizeof(member1) -1);
                    printf("Moving: %s\n", member1);

                    argCount++;
                    if(optind < argc)
                    {
                        strncpy(member2, argv[optind++], sizeof(member2) -1);
                        argCount++;
                    }
                }
                
                /* Move the member to the start */
                if(argCount == 1)
                    MoveMembers(member1, NULL, binaryArchive);

                /* Move the member1 to be infront of member2 */
                if(argCount == 2)
                    MoveMembers(member1, member2, binaryArchive);

                free(binaryName);
                fclose(binaryArchive);
                break;

            case 'r':
                /* Remove the member */
                binaryName = strdup(optarg);
                binaryArchive = OpenArchive(binaryName, 0);

                while (optind < argc)
                {
                    char arg[NAME_SIZE];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Removing: %s\n", arg);

                    RemoveMember(arg, binaryArchive);
                }
                
                free(binaryName);
                fclose(binaryArchive); 
                break;

            default:
                printf("This option doesn't exist.\n");
                printf("Usage: %s -h to display information about the program\n", argv[0]);
                break;
        }
    }
}

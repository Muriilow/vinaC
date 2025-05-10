#include "vinaC.h"
#include "TADs/lista.h"
// -i, -p
//TODO: Implementar funcoes de checagem de erro

void CreateArchive(char* name)
{
    FILE* archive;
    struct Directory header = {0};

    //Criando os arquivos
    archive = fopen(name, "wb+");
    if (archive == NULL)
    {
        fprintf(stderr, "Error creating binary file");
        return;
    }
    
    //Colocando o diretorio no arquivo
    header.quantity = 0;
    header.name = 'e';
    fwrite(&header, 1, sizeof(struct Directory), archive);

    fclose(archive);
}

FILE* OpenArchive(char* name, int toCreate)
{
    FILE* binaryArchive;

    if(name == NULL)
    {
        fprintf(stderr, "Error when getting the argument\n");
        exit(EXIT_FAILURE);
    }
    
    //Se o arquivo nao existe, crie ele
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

    if(argc < 2)
    {
        printf("Usage: %s -h to display information about the program\n", argv[0]);
        return 1;
    }

    while ((nextOption = getopt(argc, argv, ":c:i:m:p:r:x:h")) != -1) {
        switch (nextOption)
        {
            case 'h':
                ExplainProg();
                return 0;
            case 'p':
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 1);

                while (optind < argc)
                {
                    char arg[64];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Processing: %s\n", arg);

                    archive = fopen(arg, "rb");
                    
                    InsertNormalArchive(archive, binaryArchive, arg);

                    fclose(archive);
                }
                
                free(binaryName);
                fclose(binaryArchive); 
                break;
            
            case 'c':
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 0);

                ListMembers(binaryArchive);
                free(binaryName);
                fclose(binaryArchive); 
                break;
            case 'i':
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 1);

                while (optind < argc)
                {
                    char arg[64];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Processing: %s\n", arg);

                    archive = fopen(arg, "rb");
                    
                    InsertCompressedArchive(archive, binaryArchive, arg);

                    fclose(archive);
                    
                }

                free(binaryName);
                fclose(binaryArchive); 
                break;
            case 'x':
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 0);
                //No arguments
                if (optind == argc)
                {
                    ExtractAllArchives(binaryArchive);
                }
                else 
                {
                    while (optind < argc)
                    {
                        struct Member tmp;
                        int exist; 
                        char arg[64];

                        strncpy(arg, argv[optind++], sizeof(arg) -1);
                        
                        printf("%s\n", arg);
                        exist = CheckArchive(arg, binaryArchive, &tmp);

                        if(exist == -1)
                        {
                            printf("The file is not on the binary\n");
                            return 1;
                        }

                        archive = fopen(arg, "wb+");
                        if (archive == NULL)
                        {
                            fprintf(stderr, "Error creating file");
                            return 1;
                        }

                        PrintMember(tmp);
                        ExtractArchive(tmp, binaryArchive);

                        fclose(archive);
                    }
                }

                free(binaryName);
                fclose(binaryArchive);
                break;
            case 'm':
                int argCount;
                char member1[64];
                char member2[64]; 

                argCount = 0;
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 0);

                if(optind < argc)
                {
                    strncpy(member1, argv[optind++], sizeof(member1) -1);
                    argCount++;
                    if(optind < argc)
                    {
                        strncpy(member2, argv[optind++], sizeof(member2) -1);
                        argCount++;
                    }
                }
                printf("%s - %s\n", member1, member2);
                printf("%d\n", argCount);

                if(argCount == 1)
                    MoveMembers(member1, NULL, binaryArchive);
                if(argCount == 2)
                    MoveMembers(member1, member2, binaryArchive);

                free(binaryName);
                fclose(binaryArchive);
                break;
            case 'r':
                binaryName = strdup(optarg); //Alocacao dinamica
                binaryArchive = OpenArchive(binaryName, 0);

                while (optind < argc)
                {
                    char arg[64];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Processing: %s\n", arg);

                    RemoveMember(arg, binaryArchive);
                }
                
                free(binaryName);
                fclose(binaryArchive); 
                break;
        }
    }
}

#include "vinaC.h"
#include "TADs/lista.h"
// -i, -p

void createArchive(char* name)
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

    while ((nextOption = getopt(argc, argv, ":c:i:m:p:r:x:ha")) != -1) {
        switch (nextOption)
        {
            case 'h':
                ExplainProg();
                return 0;
            case 'p':
                binaryName = strdup(optarg); //Alocacao dinamica
                if(binaryName == NULL)
                {
                    fprintf(stderr, "Erro ao pegar argumento\n");
                    return 1;
                }
                
                //Se o arquivo nao existe, crie ele
                if(access(binaryName, F_OK) == -1)
                    createArchive(binaryName);

                binaryArchive = fopen(binaryName, "rb+");
                if(binaryArchive == NULL)
                {
                    fprintf(stderr, "Erro ao abrir o arquivo para editar\n");
                    return 1;
                }

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
                if(binaryName == NULL)
                {
                    fprintf(stderr, "Error when getting the argument\n");
                    return 1;
                }
                if(access(binaryName, F_OK) == -1)
                {
                    fprintf(stderr, "The file doesn't exist");
                    return 1;
                }

                binaryArchive = fopen(binaryName, "rb+");
                if(binaryArchive == NULL)
                {
                    fprintf(stderr, "Error when opening the binary archive\n");
                    return 1;
                }

                ListMembers(binaryArchive);
                free(binaryName);
                fclose(binaryArchive); 
                break;
            case 'i':
                binaryName = strdup(optarg); //Alocacao dinamica
                if(binaryName == NULL)
                {
                    fprintf(stderr, "Error when getting the argument\n");
                    return 1;
                }
                
                //Se o arquivo nao existe, crie ele
                if(access(binaryName, F_OK) == -1)
                    createArchive(binaryName);

                binaryArchive = fopen(binaryName, "rb+");
                if(binaryArchive == NULL)
                {
                    fprintf(stderr, "Error when opening the binary archive\n");
                    return 1;
                }

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
                if(binaryName == NULL)
                {
                    fprintf(stderr, "Error when getting the argument\n");
                    return 1;
                }
                
                //Se o arquivo nao existe, crie ele
                if(access(binaryName, F_OK) == -1)
                {
                    fprintf(stderr, "The file doesn't exist");
                    return 1;
                }

                binaryArchive = fopen(binaryName, "rb+");
                if(binaryArchive == NULL)
                {
                    fprintf(stderr, "Error when opening the binary archive\n");
                    return 1;
                }
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

                        printMember(tmp);
                        ExtractArchive(tmp, binaryArchive);

                        fclose(archive);
                    }
                }

                free(binaryName);
                fclose(binaryArchive);
                break;
        }
    }
}

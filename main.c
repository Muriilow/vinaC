#include "vinaC.h"
#include "TADs/lista.h"
// -i, -p

void createArchive(char* name)
{
    FILE* archive;
    struct Directory header;

    //Criando os arquivos
    archive = fopen(name, "wb+");
    if (archive == NULL)
    {
        fprintf(stderr, "Erro ao criar o arquivo");
        return;
    }
    
    //Colocando o diretorio no arquivo
    header.quantity = 0;
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
            case 'a':
               // binaryArchive = fopen("teste.vc", "ab");
               // if(stat("teste.vc", &stats) == 0)
               // {
               //     move(0, stats.st_size, stats.st_size+10);
               // }
                
                break;
            case 'h':
                ExplainProg();
                return 0;
            case 'i':
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

                while (optind < argc && argv[optind][0] != '-')
                {
                    char arg[64];

                    strncpy(arg, argv[optind++], sizeof(arg) -1);
                    printf("Processing: %s\n", arg);

                    archive = fopen(arg, "rb");
                    
                    InsertArquive(archive, binaryArchive, arg);

                    fclose(archive);
                }
                
                //moveData(0, 20, 29, binaryArchive);
                free(binaryName);
                fclose(binaryArchive); 
                break;

//            case 'm':
//                binaryName = strdup(optarg);
//                printf("%s\n", binaryName);
//
//                while (optind < argc && argv[optind][0] != '-')
//                {
//                    char *arg;
//                    FILE* archive;
//
//                    arg = argv[optind++];
//                    printf("Processing: %s\n", arg);
//
//
//                    binaryArchive = fopen(binaryName, "ab");
//                    archive = fopen(arg, "rb");
//                    
//                    InsertArquive(binaryArchive, archive);
//                    
//                    fclose(binaryArchive);
//                    fclose(archive);
//
//                    archive = NULL;
//                    binaryArchive = NULL;
//                }
//                break;
//
//            case 'c':
//                binaryName = strdup(optarg);
//                printf("%s\n", binaryName);
//
//                while (optind < argc && argv[optind][0] != '-')
//                {
//                    char *arg;
//                    FILE* archive;
//
//                    arg = argv[optind++];
//                    printf("Processing: %s\n", arg);
//
//
//                    binaryArchive = fopen(binaryName, "ab");
//                    archive = fopen(arg, "rb");
//                    
//                    InsertArquive(binaryArchive, archive);
//                    
//                    fclose(binaryArchive);
//                    fclose(archive);
//
//                    archive = NULL;
//                    binaryArchive = NULL;
//                }
//                break;
//
//            case 'r':
//                binaryName = strdup(optarg);
//                printf("%s\n", binaryName);
//
//                while (optind < argc && argv[optind][0] != '-')
//                {
//                    char *arg;
//                    FILE* archive;
//
//                    arg = argv[optind++];
//                    printf("Processing: %s\n", arg);
//
//
//                    binaryArchive = fopen(binaryName, "ab");
//                    archive = fopen(arg, "rb");
//                    
//                    InsertArquive(binaryArchive, archive);
//                    
//                    fclose(binaryArchive);
//                    fclose(archive);
//
//                    archive = NULL;
//                    binaryArchive = NULL;
//                }
//                break;
//
//            case 'x':
//                binaryName = strdup(optarg);
//                printf("%s\n", binaryName);
//
//                while (optind < argc && argv[optind][0] != '-')
//                {
//                    char *arg;
//                    FILE* archive;
//
//                    arg = argv[optind++];
//                    printf("Processing: %s\n", arg);
//
//
//                    binaryArchive = fopen(binaryName, "ab");
//                    archive = fopen(arg, "rb");
//                    
//                    InsertArquive(binaryArchive, archive);
//                    
//                    fclose(binaryArchive);
//                    fclose(archive);
//
//                    archive = NULL;
//                    binaryArchive = NULL;
//                }
//                break;
//
//            case 'p':
//                binaryName = strdup(optarg);
//                printf("%s\n", binaryName);
//
//                while (optind < argc && argv[optind][0] != '-')
//                {
//                    char *arg;
//                    FILE* archive;
//
//                    arg = argv[optind++];
//                    printf("Processing: %s\n", arg);
//
//
//                    binaryArchive = fopen(binaryName, "ab");
//                    archive = fopen(arg, "rb");
//                    
//                    InsertArquive(binaryArchive, archive);
//                    
//                    fclose(binaryArchive);
//                    fclose(archive);
//
//                    archive = NULL;
//                    binaryArchive = NULL;
//                }
//                break;
//
//            case ':':
//                fprintf(stderr, "Option -%c requires an operand\n", optopt);
//                break;
//
//            case '?':
//                fprintf(stderr,"Unrecognized option: -%c\n", optopt);
//                break;
//
//            default:
//                printf("Usage: %s -h to display information about the program\n", argv[0]);
//                return 1;
        }
    }
}

#include "vinaC.h"
#include "TADs/lista.h"
// -i, -p

int main(int argc, char **argv)
{
    char nextOption;
    
    char* binaryName;
    FILE* binaryArchive;
    int isArchive = 1;

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
                
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);

                    //Abrindo os arquivos
                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(archive, binaryArchive, arg);

                    fclose(archive);
                }

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

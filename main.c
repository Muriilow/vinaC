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

    while ((nextOption = getopt(argc, argv, ":c:i:m:p:r:x:h")) != -1) {
        switch (nextOption)
        {
            case 'h':
                ExplainProg();
                return 0;
            case 'i':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case 'm':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case 'c':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case 'r':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case 'x':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case 'p':
                binaryName = strdup(optarg);
                printf("%s\n", binaryName);

                while (optind < argc && argv[optind][0] != '-')
                {
                    char *arg;
                    FILE* archive;

                    arg = argv[optind++];
                    printf("Processing: %s\n", arg);


                    binaryArchive = fopen(binaryName, "ab");
                    archive = fopen(arg, "rb");
                    
                    InsertArquive(binaryArchive, archive);
                    
                    fclose(binaryArchive);
                    fclose(archive);

                    archive = NULL;
                    binaryArchive = NULL;
                }
                break;

            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                break;

            case '?':
                fprintf(stderr,"Unrecognized option: -%c\n", optopt);
                break;

            default:
                printf("Usage: %s -h to display information about the program\n", argv[0]);
                return 1;
        }
    }

    //struct stat stats;
    //struct Member* info;
    //FILE* archive;
    //FILE* archiveTmp;

    //char* name = argv[2];
    //printf("%s\n", name);
    //return 0;
    //if(strcmp(argv[1], "a") == 0)
    //{
    //    char* name = argv[2];
    //    printf("%s\n", name);

    //    archive = fopen("teste.vc", "ab");
    //    archiveTmp = fopen(name, "r");

    //    if(stat(name, &stats) == 0)
    //    {
    //        info = malloc(sizeof(struct Member));

    //        info->UID = stats.st_uid;
    //        info->origSize = stats.st_size;
    //        info->comprSize = stats.st_size;
    //        info->modifData = stats.st_mtime;
    //        info->order = 1;
    //        info->pos = 1;

    //        //struct List* q = CreateList();
    //        //InsertList(q, info, -1);
    //        //PrintList(q); 
    //        char* content = malloc(sizeof(info->origSize));
    //        fread(content, sizeof(info->origSize), 1, archiveTmp);
    //        
    //        //fseek(archiveTmp, 0, SEEK_END);
    //        //int size = ftell(archiveTmp);
    //        //fseek(archiveTmp, 0, SEEK_SET);
    //        //printf("%d\n", size);
    //        //
    //        printf("%d\n", info->origSize);
    //        if(fwrite(content, sizeof(info->origSize), 1, archive) == 1)
    //            printf("Escrevi o binario\n");
    //        if(fwrite(info, sizeof(struct Member), 1, archive) == 1)
    //            printf("deu bom fila\n");
    //        

    //        free(info);
    //        free(content);
    //        //DestroyList(q);
    //    }
    //}
    //if(strcmp(argv[1], "b") == 0)
    //{
    //    archive = fopen("teste.vc", "r");
    //    archiveTmp = fopen("teste.txt", "r");

    //    fseek(archive, -(sizeof(struct Member) + 1), SEEK_END);
    //    struct Member* content = malloc(sizeof(struct Member));

    //    fread(content, sizeof(struct Member), 1, archive);

    //    //fread(a, sizeof(struct List), 1, archive);
    //    printf("%d\n", content->pos);
    //}
    //
    //if(fclose(archive) != 0 || fclose(archiveTmp) != 0)
    //{
    //    printf("Error closing arquive\n");
    //    return 1;
    //}
}

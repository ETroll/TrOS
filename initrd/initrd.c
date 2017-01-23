/**
    A initrd creator.

    Creates a flat binary with a header pointing to the location of the files.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define MAX_FILES 16
#define MAX_NAME 16

typedef struct {
    char name[MAX_NAME];
    unsigned int start;
    unsigned int size;
} __attribute__((packed)) trfs_file_t;

typedef struct {
    unsigned char key[4];
    unsigned int size;
    unsigned int numfiles;
    trfs_file_t files[MAX_FILES];
} __attribute__((packed)) trfs_header_t;



void file_create(const char* path);
void file_add(const char* path, const char* filepath);
void file_list(const char* path);

int main(int argc, const char* argv[])
{
    if(argc < 3)
    {
        printf("Invalid arguments: Usage initrd <path> <command>\n");
    }
    else
    {
        for(int i = 2; i<argc; i++)
        {
            if(argv[i][0] == '-')
            {
                switch (argv[i][1]) {
                    case 'c':
                    {
                        file_create(argv[1]);
                    }break;
                    case 'l':
                    {
                        file_list(argv[1]);
                    }break;
                    case 'a':
                    {
                        if(i < argc)
                        {
                            file_add(argv[1], argv[++i]);
                        }
                        else
                        {
                            printf("Invalid arguments for add\n");
                            return -1;
                        }
                    }break;
                    default:
                    break;
                }
            }
            else {
                printf("Non argument: %s\n", argv[i]);
            }
        }
        return 0;
    }
}

void file_create(const char* path)
{
    FILE *file = fopen(path, "w+");
    if(file)
    {
        trfs_header_t header;
        strncpy(header.key, "TRFS", 4);
        // header.key = 0xFADAFAD;
        header.size = sizeof(trfs_header_t);
        header.numfiles = 0;
        memset(header.files, 0, sizeof(trfs_file_t)*MAX_FILES);
        fwrite(&header, 1, sizeof(trfs_header_t), file);
        fclose(file);
    }
    else
    {
        printf("Error creating file %s\n", path);
    }
}

void file_add(const char* path, const char* filepath)
{
    FILE *file = fopen(path, "r+");
    if(file)
    {
        FILE* newfile = fopen(filepath, "r");

        if(newfile)
        {
            trfs_header_t header;

            fseek(file, 0L, SEEK_SET);
            fread(&header, sizeof(trfs_header_t), 1, file);
            // perror ("The following error occurred");
            if(header.numfiles < MAX_FILES)
            {
                trfs_file_t *ptr = (trfs_file_t*)&header.files[header.numfiles++];
                char* filename = basename((char*)filepath);
                printf("Image has %i files before\n", header.numfiles-1);

                if(strlen(filename) < MAX_NAME)
                {
                    strcpy(ptr->name, filename);
                    fseek(file, 0L, SEEK_END);
                    fseek(newfile, 0L, SEEK_END);

                    ptr->start = ftell(file);
                    ptr->size = ftell(newfile);

                    printf("adding new file size: %i at 0x%x\n", ptr->size, ptr->start);
                    char* buffer = (char*)malloc(ptr->size);
                    fseek(newfile, 0L, SEEK_SET);
                    fread(buffer, 1, ptr->size, newfile);
                    fwrite(buffer, 1, ptr->size, file);
                    free(buffer);

                    fseek(file, 0L, SEEK_SET);
                    fwrite(&header, 1, sizeof(trfs_header_t), file);
                }
                else
                {
                    printf("Filename to large\n");
                }
            }
            else
            {
                printf("Image full. Maximum of %i files have been reaced (%i)\n", MAX_FILES, header.numfiles);
            }
            fclose(newfile);
        }
        else {
            printf("Could not open file %s\n", filepath);
        }
        fclose(file);
    }
    else
    {
        printf("Error adding file %s\n", filepath);
    }
}

void file_list(const char* path)
{
    FILE *file = fopen(path, "r");
    if(file)
    {
        trfs_header_t header;
        fread(&header, sizeof(trfs_header_t), 1, file);
        printf("| Name \t\t | Size \t | From \t | To \t\t |\n");
        for(int i = 0; i<header.numfiles; i++)
        {
            trfs_file_t *ptr = (trfs_file_t*)&header.files[i];
            printf("| %s \t | %d \t | 0x%x \t | 0x%x \t |\n",
                ptr->name,
                ptr->size,
                ptr->start,
                (ptr->start + ptr->size));
        }
        fclose(file);
    }
    else
    {
        printf("Error reading files from %s\n", path);
    }
}

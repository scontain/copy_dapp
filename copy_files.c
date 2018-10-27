// Simple Copying of Files
//

#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* filename, size_t* size) {
    *size=-1;
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror ("Error opening file for reading"); 
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    if (string == NULL) {
        fprintf(stderr, "cannot allocated sufficient memory '%ld' for file '%s'\n", fsize, filename);
        return NULL;
    }
    size_t sz = fread(string, fsize, 1, f);
    if (sz != 1) {
        fprintf(stderr, "reading file '%s': expected %zu bytes but only got %zu\n", filename, fsize, sz);
        return NULL;
    }
    fclose(f);

    string[fsize] = 0;
    *size=fsize;
    return string;
}

int write_file(const char* filename, char* buf, size_t size) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror ("Error opening file for writing"); 
        return 0;
    }
    size_t sz = fwrite(buf, size, 1, f);
    if (sz != 1) {
        fprintf(stderr, "writing file '%s': wanted to write %zu bytes but only written %zu\n", filename, size, sz);
        return 0;
    }
    free(buf);
    fclose(f);
    return 1;
}

int copy_file(const char* from, const char* to) {
    size_t size=0;
    char* buf;

    buf = read_file(from, &size);
    if (!buf) {
        return 0;
    }
    return write_file(to, buf, size);
}

#include <sys/types.h>

 #include <sys/stat.h>

 #include <assert.h>
 #include <dirent.h>
 #include <limits.h>
 #include <stdio.h>
 #include <string.h>


static int ptree(char *curpath, char * const path, const char* prefix, int index) {
        char ep[PATH_MAX];
        char p[PATH_MAX];
        char to[PATH_MAX];
        DIR *dirp;
        struct dirent entry;
        struct dirent *endp;
        struct stat st;

        if (curpath != NULL)
                snprintf(ep, sizeof(ep), "%s/%s", curpath, path);
        else
                snprintf(ep, sizeof(ep), "%s", path);
        if (stat(ep, &st) == -1)
                return -1;
        if ((dirp = opendir(ep)) == NULL)
                return -1;
        for (;;) {
                endp = NULL;
                if (readdir_r(dirp, &entry, &endp) == -1) {
                        closedir(dirp);
                        return -1;
                }
                if (endp == NULL)
                        break;
                assert(endp == &entry);
                if (strcmp(entry.d_name, ".") == 0 ||
                    strcmp(entry.d_name, "..") == 0)
                        continue;
                if (curpath != NULL)
                        snprintf(ep, sizeof(ep), "%s/%s/%s", curpath,
                            path, entry.d_name);
                else
                        snprintf(ep, sizeof(ep), "%s/%s", path,
                            entry.d_name);
                if (stat(ep, &st) == -1) {
                        closedir(dirp);
                        return -1;
                }
                if (S_ISREG(st.st_mode)) {
                        ep[sizeof(ep)-1]=0;
                        snprintf(to, sizeof(to), "%s/%s", prefix, &ep[index]);
                        printf("copying file %s to %s\n", ep, to);
                        if (! copy_file(ep, to)) {
                            printf("Error copying file '%s'\n", ep);
                            exit(1);
                        }
                }
                if (S_ISDIR(st.st_mode) == 0)
                        continue;
                if (curpath != NULL)
                        snprintf(p, sizeof(p), "%s/%s", curpath, path);
                else
                        snprintf(p, sizeof(p), "%s", path);
                snprintf(ep, sizeof(ep), "%s", entry.d_name);
                ptree(p, ep, prefix, index);
        }
        closedir(dirp);
        return 0;
}
int main(int argc, char** argv) {
    printf("%s: # arguments = %d", argv[0], argc);
    for (int i = 1 ; i < argc ; ++i)
        printf("   argument %d: %s", i, argv[i]);
    ptree(NULL, "/encryptedInputs", "/encryptedOutputs", strlen("/encryptedInputs/"));
}

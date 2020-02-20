#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>
 
enum {
    WALK_OK = 0,
    WALK_BADPATTERN,
    WALK_NAMETOOLONG,
    WALK_BADIO,
};
 
#define WS_NONE        0
#define WS_RECURSIVE    (1 << 0)
#define WS_DEFAULT    WS_RECURSIVE
#define WS_FOLLOWLINK    (1 << 1)    /* follow symlinks */
#define WS_DOTFILES    (1 << 2)    /* per unix convention, .file is hidden */
#define WS_MATCHDIRS    (1 << 3)    /* if pattern is used on dir names too */


struct myFile
{
unsigned long long size;
char * name;
};

struct myFile fileList[7000];
int i;
 
int walk_recur(char *dname)
{
    const char* arr[100];
    struct dirent *dent;
    DIR *dir;
    struct stat st;
    char fn[FILENAME_MAX];
    int res = WALK_OK;
    int len = strlen(dname);

    
    
    
    strcpy(fn, dname);
    fn[len++] = '/';
 
    if (!(dir = opendir(dname))) {
        warn("can't open %s", dname);
        return WALK_BADIO;
    }
 
    errno = 0;
    while ((dent = readdir(dir))) {
        
        if (!(WS_DOTFILES) && dent->d_name[0] == '.')
            continue;
        if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
            continue;
 
        strncpy(fn + len, dent->d_name, FILENAME_MAX - len);
        if (lstat(fn, &st) == -1) {
            warn("Can't stat %s", fn);
            res = WALK_BADIO;
            continue;
        }
 
        /* don't follow symlink unless told so */
        if (S_ISLNK(st.st_mode) && !(WS_FOLLOWLINK))
            continue;
 
        /* will be false for symlinked dirs */
        if (S_ISDIR(st.st_mode)) {
            /* recursively follow dirs */
                walk_recur(fn);

        }
        
        
        /* pattern match */
        struct stat st;
            if (stat(fn, &st) == 0)
            {
            fileList[i].size=st.st_size;
            fileList[i].name=fn;
            i++;
            }
    }
 
    if (dir) closedir(dir);
    return res ? res : errno ? WALK_BADIO : WALK_OK;
}
 
int walk_dir(char *dname)
{
    regex_t r;
    int res;
    res = walk_recur(dname);
    regfree(&r);
 
    return res;
}

void SwapV(struct myFile *xpoint, struct myFile *ypoint)
{
    struct myFile temp = *xpoint;
    *xpoint = *ypoint;
    *ypoint = temp;
}

void bSort(struct myFile arr[], int n)
{
   for (int i = 0; i < n-1; i++)
       // Last elements i are already set into the place
       for (int j = 0; j < n-i-1; j++)
           if (arr[j].size > arr[j+1].size)
              SwapV(&arr[j], &arr[j+1]);
}

void PrintArr(struct myFile arr[], int size)
{
    for (int i=0; i < size; i++)
        printf("%llu\t%s\n", arr[i].size, arr[i].name);
}
 
 
int main(int argc, char *argv[])
{
    int r = walk_dir(argv[1]);
    bSort(fileList, 20);
    PrintArr(fileList, 20);
    
    
    return 0;
}

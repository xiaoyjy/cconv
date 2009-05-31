#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "dict.h"

language_zh_map *g_dict = NULL;
void dict_append(language_zh_map **f_dst, language_zh_map *f_src, int f_size);

/**
 * check whether file extension of the 'f_filename' is '.dic'.
 *
 * the value returned are 'false' if the 'f_filename' falls into the tested
 * class, and 'true' value if not.
 */
int is_dict(const char *f_filename) {
    int ret = 0;

    char *dot = strchr(f_filename, '.');
    if (dot)
        ret = !strcasecmp(dot, ".dic");

    return ret;
}

/**
 * load words in user dictionary file whose name is the string pointed to by
 * 'f_dict' into the buffer pointed to by 'f_buf', 'f_size' is passed to
 * indicate the size of the buffer.
 *
 * returns the number of word in 'f_dict'.
 */
int dict_load(const char *f_dict, language_zh_map **f_buf, int *f_size) {
    FILE *fp = fopen(f_dict, "r");
    if (!fp) {
        return 0;
    }

    size_t length = 256;
    char *line = (char*)malloc(sizeof(char) * length);
    int size = 0;
#ifdef _GNU_SOURCE
    if (getline(&line, &length, fp) != -1)
#else
    if (fgets(line, length, fp))
#endif
    {
        int ret = sscanf(line, "%d\n", &size);
        if (ret == 0 || size <= 0){
            fclose(fp);
            free(line);
            return 0;
        }
    }

    size += *f_size;
    language_zh_map* buf = (language_zh_map*)malloc(sizeof(language_zh_map) * size);
    bzero((void*)buf, sizeof(language_zh_map) * size);
    if (*f_size > 0) {
        dict_append(&buf, *f_buf, *f_size);
        dict_unload(f_buf, *f_size);
    }

    char key[100] = {0};
    char val[100] = {0};
    int cond = 0;
    int load = 0;
    int i;
#ifdef _GNU_SOURCE
    for (i = *f_size; (i < size) && (getline(&line, &length, fp) != -1); )
#else
    for (i = *f_size; (i < size) && fgets(line, length, fp); )
#endif
    {
        int ret = sscanf(line, "{\"%[^\"]\", \"%[^\"]\", %d}\n", key, val, &cond);
        if (ret == 0) {
            continue;
        }

        buf[i].key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
        buf[i].val = (char*)malloc(sizeof(char) * (strlen(val) + 1));
        strcpy(buf[i].key, key);
        strcpy(buf[i].val, val);
        buf[i].cond = cond;

        load = ++i;
    }

    free(line);
    fclose(fp);
    *f_buf = buf;
    *f_size = size;
    return load;
}

/**
 * appends the 'f_src' buffer to the 'f_dst' buffer, and the 'f_dst' buffer
 * must have enough space for the result. it will use at most 'f_size' words
 * from 'f_src'.
 */
void dict_append(language_zh_map **f_dst, language_zh_map *f_src, int f_size) {
    language_zh_map *dst = *f_dst;

    if (!dst || !f_src || f_size <= 0) {
        return;
    }

    int i;
    for (i = 0; i < f_size; ++i) {
        if (!f_src[i].key || !f_src[i].val)
            continue;

        dst[i].key = (char*)malloc(sizeof(char) * (strlen(f_src[i].key) + 1));
        dst[i].val = (char*)malloc(sizeof(char) * (strlen(f_src[i].val) + 1));
        strcpy(dst[i].key, f_src[i].key);
        strcpy(dst[i].val, f_src[i].val);
        dst[i].cond = f_src[i].cond;
    }
}

/**
 *
 */
void dict_find() {

}


/**
 * free the memory space pointed to by 'f_dict', 'f_size' is passed to indicate
 * the size of the buffer.
 */
void dict_unload(language_zh_map **f_dict, int f_size) {
    language_zh_map *dict = *f_dict;

    if (!dict)
        return;

    language_zh_map p;
    int i;
    for (i = 0; i < f_size; ++i) {
        p = dict[i];
        if (p.key) { free(p.key); p.key = NULL; }
        if (p.val) { free(p.val); p.val = NULL; }
    }

    free(dict);
    dict = NULL;
}

void dict_test(language_zh_map **f_dict, int f_size) {
    printf("number of the word: %d\n", f_size);
    language_zh_map p;
    int i;
    for (i = 0; i < f_size; ++i) {
        p = (*f_dict)[i];
        printf("  key: '%s', val: '%s', cond: '%d'\n", p.key, p.val, p.cond);
    }
}

#ifdef _DEBUG
language_zh_map **f_dict = &g_dict;
int main(int argc, char* argv[])
#else
int dict_init(language_zh_map **f_dict)
#endif
{
    char *HOME;
    if (HOME = getenv("HOME"))
        printf("HOME=%s\n", HOME);

    char *dir;
#ifdef _GNU_SOURCE
    asprintf(&dir, "%s/.cconv/", HOME);
#else
    dir = (char*)malloc(sizeof(char) * (strlen(HOME) + 9));
    sprintf(dir, "%s/.cconv/", HOME);
#endif

    DIR *dp;
    struct dirent *filename;
    if (!(dp = opendir(dir))) {
        printf("null: %s\n", dir);
        free(dir);
        return 0;
    }

    int size = 0;
    while (filename = readdir(dp)) {
        if (!is_dict(filename->d_name))
            continue;

        char *dictionary;
#ifdef _GNU_SOURCE
        asprintf(&dictionary, "%s%s", dir, filename->d_name);
#else
        dictionary = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(filename->d_name) + 1));
        sprintf(dictionary, "%s%s", dir, filename->d_name);
#endif
        int load = dict_load(dictionary, f_dict, &size);
        free(dictionary);
    }

#ifdef _DEBUG
    dict_test(f_dict, size);
#endif
    closedir(dp);
    free(dir);
    return size;
}


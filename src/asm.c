
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void error(char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}

void usage()
{
    fprintf(stderr, "usage: asm source.s");
    exit(1);
}


uint32_t dj2(char* str)
{
    uint32_t hash = 5381;
    char c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

struct pre_label_entry
{
    uint32_t addr;
    uint32_t hash;
    char name[128];
} pre_label_table[10000] = { 0 };
uint32_t pre_label_index = 0;

void register_label(char* name, uint32_t addr)
{
    struct pre_label_entry* l = &pre_label_table[pre_label_index++];
    l->addr = addr;
    l->hash = dj2(name);
    strcpy(l->name, name);
}

char fpeek(FILE* stream)
{
    char c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

bool is_space(char c)
{
    if (c == ' ' ) return true;
    if (c == '\n') return true;
    if (c == '\t') return true;
    return false;
}

char* tok(FILE* f)
{
restart:
    while (is_space(fpeek(f))) fgetc(f);

    char c = fpeek(f);
    if (c == EOF) return NULL;

    static char buffer[128];
    char* out = buffer;

    switch (c)
    {
        case ';':
            while (fgetc(f) != '\n');
            goto restart;

        case '"':
            fgetc(f);
            while (fpeek(f) != '"')
                *out++ = fgetc(f);
            fgetc(f);
            break;

        default:
            while (!is_space(fpeek(f)))
                *out++ = fgetc(f);
            break;
    }

    *out = '\0';
    return buffer;
}



void explore(char* path)
{
    FILE* f = fopen(path, "r");

    char* t;
    while ((t = tok(f)))
        printf("%s\n", t);

}



int main(int argc, char** argv)
{
    if (argc != 2) usage();
    char* path = argv[1];
    
    explore(path);


    return 0;
}




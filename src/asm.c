
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void usage()
{
    fprintf(stderr, "usage: asm source.s");
    exit(1);
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

static uint32_t line_no = 0;
char tgetc(FILE* f)
{
    char c = fgetc(f);

    if (c == '\n') line_no++;

    return c;
}


char* tok(FILE* f)
{
restart:
    while (is_space(fpeek(f))) tgetc(f);

    char c = fpeek(f);
    if (c == EOF) return NULL;

    static char buffer[128];
    char* out = buffer;

    switch (c)
    {
        case ';':
            while (tgetc(f) != '\n');
            goto restart;

        case '"':
            tgetc(f);
            while (fpeek(f) != '"')
                *out++ = tgetc(f);
            tgetc(f);
            break;

        default:
            while (!is_space(fpeek(f)))
                *out++ = tgetc(f);
            break;
    }

    *out = '\0';
    return buffer;
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
    uint16_t addr;
    uint32_t hash;
    char name[128];
} pre_label_table[10000] = { 0 };
uint32_t pre_label_index = 0;

void register_label(char* name, uint16_t addr)
{
    struct pre_label_entry* l = &pre_label_table[pre_label_index++];
    l->addr = addr;
    l->hash = dj2(name);
    strcpy(l->name, name);
}
uint16_t lookup_label(char* name)
{
    uint32_t hash = dj2(name);
    
    for (uint32_t i = 0; i < pre_label_index; i++)
    {
        if (hash != pre_label_table[i].hash) continue;
        if (strcmp(name, pre_label_table[i].name)) continue;
        return pre_label_table[i].addr;
    }

    fprintf(stderr, "error at line %d: unable to resolve label '%s'.\n", line_no, name);
    exit(1);
}


struct var_entry
{
    char name[128];
} var_table[1000] = { 0 };
uint32_t var_table_index = 0;

void alloc_var(char* name)
{
    strcpy(
        var_table[var_table_index++].name,
        name
    );
}
uint32_t lookup_var(char* name)
{
    for (uint32_t addr = 0; addr < var_table_index; addr++)
    {
        if (strcmp(name, var_table[addr].name)) continue;
        return addr;
    }

    fprintf(stderr, "error at line %d: unable to resolve variable '%s'.\n", line_no, name);
    exit(1);
}





uint16_t addr = 0;
void explore(char* path)
{
    FILE* f = fopen(path, "r");

    char* t;
    while ((t = tok(f)))
    {
        /*zero paramter*/ #define zP(str) else if (!strcmp(t, str))           addr++;
        /*one  paramter*/ #define oP(str) else if (!strcmp(t, str)) { tok(f); addr += 2; }
        /*jump paramter*/ #define jP(str) else if (!strcmp(t, str)) { tok(f); addr += 3; }

        if (0) {}
        zP("brk") zP("inc") zP("pop") zP("swp") zP("dup") oP("lit")
        zP("equ") zP("neq") zP("gth") zP("lth")
        jP("jmp") jP("jcn") jP("jsr") zP("ret")
        oP("ldv") oP("stv") zP("lda") zP("sta")
        zP("inp") zP("out")
        zP("add") zP("sub") zP("mul") zP("div")
        zP("and") zP("aor") zP("xor") zP("shl") zP("shr") zP("not")
        zP("dbg") 

        else if (!strcmp(t, "str"))
            addr += strlen(tok(f)) + 2; //+1 instruction +1 terminator

        else if (!strcmp(t, "lab"))
            register_label(tok(f), addr);

        else if (!strcmp(t, "var"))
            alloc_var(tok(f));

        else if (!strcmp(t, "use"))
        {
            char buf[128];
            sprintf(buf, "lib/%s", tok(f));
            explore(buf);
        }

        else if (t[0] == 's')
            addr++;

    }
}

void fjump(char* label, FILE* out)
{
    //little endian
    uint16_t addr = lookup_label(label);
    fputc((addr >> 0) & 0xff, out);
    fputc((addr >> 8) & 0xff, out);
}

void assemble(char* path, FILE* out)
{
    line_no = 0;
    FILE* f = fopen(path, "r");

    char* t;
    while ((t = tok(f)))
    {
        /**/ if (!strcmp(t, "brk")) fputc(0x00, out);
        else if (!strcmp(t, "inc")) fputc(0x01, out);
        else if (!strcmp(t, "pop")) fputc(0x02, out);
        else if (!strcmp(t, "swp")) fputc(0x03, out);
        else if (!strcmp(t, "dup")) fputc(0x04, out);
        else if (!strcmp(t, "lit")) { fputc(0x05, out); fputc(atoi(tok(f)), out); }
        
        else if (!strcmp(t, "equ")) fputc(0x06, out); 
        else if (!strcmp(t, "neq")) fputc(0x07, out);
        else if (!strcmp(t, "gth")) fputc(0x08, out);
        else if (!strcmp(t, "lth")) fputc(0x09, out);

        else if (!strcmp(t, "jmp")) { fputc(0x0a, out); fjump(tok(f), out); }
        else if (!strcmp(t, "jcn")) { fputc(0x0b, out); fjump(tok(f), out); }
        else if (!strcmp(t, "jsr")) { fputc(0x0c, out); fjump(tok(f), out); }
        else if (!strcmp(t, "ret")) fputc(0x0d, out);

        else if (!strcmp(t, "ldv")) { fputc(0x0e, out); fputc(lookup_var(tok(f)), out); }
        else if (!strcmp(t, "stv")) { fputc(0x0f, out); fputc(lookup_var(tok(f)), out); }

        else if (!strcmp(t, "lda")) fputc(0x10, out);
        else if (!strcmp(t, "sta")) fputc(0x11, out);
            
        else if (!strcmp(t, "inp")) fputc(0x12, out);
        else if (!strcmp(t, "out")) fputc(0x13, out);

        else if (!strcmp(t, "add")) fputc(0x14, out);
        else if (!strcmp(t, "sub")) fputc(0x15, out);
        else if (!strcmp(t, "mul")) fputc(0x16, out);
        else if (!strcmp(t, "div")) fputc(0x17, out);

        else if (!strcmp(t, "and")) fputc(0x18, out);
        else if (!strcmp(t, "aor")) fputc(0x19, out);
        else if (!strcmp(t, "xor")) fputc(0x1a, out);
        else if (!strcmp(t, "shl")) fputc(0x1b, out);
        else if (!strcmp(t, "shr")) fputc(0x1c, out);
        else if (!strcmp(t, "not")) fputc(0x1d, out);

        else if (!strcmp(t, "dbg")) fputc(0x1e, out);
        else if (!strcmp(t, "str"))
        {
            fputc(0x1f, out);
            char* str = tok(f);
            for (; *str; str++)
                fputc(*str, out);
            fputc(0x00, out); //termi
        }

        else if (!strcmp(t, "lab")) tok(f);
        else if (!strcmp(t, "var")) tok(f);

        else if (t[0] == 's')
        {
            t[1] -= '0';
            t[2] -= '0';
            uint8_t inst = (t[1] * 10) + t[2];
            fputc(inst | 0x80, out);
        }
        else if (!strcmp(t, "use"))
        {
            char buf[128];
            sprintf(buf, "lib/%s", tok(f));
            assemble(buf, out);
        }

        else
        {
            fprintf(stderr, "error at line %d: invalid instruction '%s'.\n", line_no, t);
            exit(1);
        }

    }

}

void list_labels()
{
    for (int i = 0; i < pre_label_index; i++)
        printf("%x: %s\n", pre_label_table[i].addr, pre_label_table[i].name);
}



int main(int argc, char** argv)
{
    if (argc != 2) usage();
    char* path = argv[1];

    FILE* out = fopen("build", "wb");
    
    explore(path);
    assemble(path, out);

    list_labels();

    fclose(out);
    return 0;
}




#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1024
#define MAX_LABELS 256
#define MAX_WORDS 65536
#define MAX_TEXT 256

typedef struct {
    char text[MAX_TEXT];
    int line_no;
} Line;

typedef struct {
    char name[64];
    uint16_t address;
} Label;

static Line lines[MAX_LINES];
static int line_count;

static Label labels[MAX_LABELS];
static int label_count;

static uint16_t words[MAX_WORDS];
static int word_count;
static uint16_t origin;
static int have_origin;

static void die_line(int line_no, const char *message)
{
    fprintf(stderr, "line %d: %s\n", line_no, message);
    exit(1);
}

static char *trim(char *s)
{
    while (isspace((unsigned char)*s)) {
        ++s;
    }

    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return s;
}

static void strip_comment(char *s)
{
    int in_string = 0;
    for (; *s; ++s) {
        if (*s == '"') {
            in_string = !in_string;
        } else if (*s == ';' && !in_string) {
            *s = '\0';
            return;
        }
    }
}

static void uppercase(char *s)
{
    for (; *s; ++s) {
        *s = (char)toupper((unsigned char)*s);
    }
}

static int starts_with_directive_or_opcode(const char *s)
{
    char token[64];
    if (sscanf(s, "%63s", token) != 1) {
        return 0;
    }
    uppercase(token);

    return token[0] == '.' ||
           strcmp(token, "LEA") == 0 ||
           strcmp(token, "PUTS") == 0 ||
           strcmp(token, "HALT") == 0;
}

static uint16_t parse_number(const char *s, int line_no)
{
    int base = 10;
    const char *p = s;

    if (*p == 'x' || *p == 'X') {
        base = 16;
        ++p;
    } else if (*p == '#') {
        base = 10;
        ++p;
    }

    char *end = NULL;
    long value = strtol(p, &end, base);
    if (*p == '\0' || *end != '\0' || value < 0 || value > 0xFFFF) {
        die_line(line_no, "invalid number");
    }
    return (uint16_t)value;
}

static int parse_register(const char *s, int line_no)
{
    if ((s[0] != 'R' && s[0] != 'r') || s[1] < '0' || s[1] > '7' || s[2] != '\0') {
        die_line(line_no, "invalid register");
    }
    return s[1] - '0';
}

static void add_label(const char *name, uint16_t address, int line_no)
{
    if (label_count >= MAX_LABELS) {
        die_line(line_no, "too many labels");
    }

    for (int i = 0; i < label_count; ++i) {
        if (strcmp(labels[i].name, name) == 0) {
            die_line(line_no, "duplicate label");
        }
    }

    snprintf(labels[label_count].name, sizeof(labels[label_count].name), "%s", name);
    labels[label_count].address = address;
    ++label_count;
}

static uint16_t find_label(const char *name, int line_no)
{
    for (int i = 0; i < label_count; ++i) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    die_line(line_no, "unknown label");
    return 0;
}

static char *next_token(char **cursor)
{
    char *s = trim(*cursor);
    if (*s == '\0') {
        *cursor = s;
        return NULL;
    }

    char *start = s;
    while (*s && !isspace((unsigned char)*s) && *s != ',') {
        ++s;
    }

    if (*s) {
        *s++ = '\0';
    }
    while (isspace((unsigned char)*s) || *s == ',') {
        ++s;
    }
    *cursor = s;
    return start;
}

static int stringz_word_count(const char *s, int line_no)
{
    const char *start = strchr(s, '"');
    const char *end = strrchr(s, '"');
    if (start == NULL || end == NULL || start == end) {
        die_line(line_no, "invalid .STRINGZ");
    }

    int count = 1;
    for (const char *p = start + 1; p < end; ++p) {
        if (*p == '\\' && p + 1 < end) {
            ++p;
        }
        ++count;
    }
    return count;
}

static void load_lines(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        exit(1);
    }

    char buffer[MAX_TEXT];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (line_count >= MAX_LINES) {
            fprintf(stderr, "too many lines\n");
            exit(1);
        }
        strip_comment(buffer);
        char *clean = trim(buffer);
        if (*clean == '\0') {
            continue;
        }
        snprintf(lines[line_count].text, sizeof(lines[line_count].text), "%s", clean);
        lines[line_count].line_no = line_count + 1;
        ++line_count;
    }

    fclose(file);
}

static void first_pass(void)
{
    uint16_t loc = 0;

    for (int i = 0; i < line_count; ++i) {
        char line[MAX_TEXT];
        snprintf(line, sizeof(line), "%s", lines[i].text);
        int line_no = lines[i].line_no;

        char *cursor = trim(line);
        if (!starts_with_directive_or_opcode(cursor)) {
            char *label = next_token(&cursor);
            add_label(label, loc, line_no);
        }

        char rest[MAX_TEXT];
        snprintf(rest, sizeof(rest), "%s", trim(cursor));
        cursor = rest;
        char *op = next_token(&cursor);
        if (op == NULL) {
            continue;
        }
        uppercase(op);

        if (strcmp(op, ".ORIG") == 0) {
            char *arg = next_token(&cursor);
            if (arg == NULL) {
                die_line(line_no, ".ORIG needs an address");
            }
            origin = parse_number(arg, line_no);
            loc = origin;
            have_origin = 1;
        } else if (strcmp(op, ".END") == 0) {
            return;
        } else if (!have_origin) {
            die_line(line_no, "missing .ORIG");
        } else if (strcmp(op, ".STRINGZ") == 0) {
            loc = (uint16_t)(loc + stringz_word_count(cursor, line_no));
        } else if (strcmp(op, "LEA") == 0 ||
                   strcmp(op, "PUTS") == 0 ||
                   strcmp(op, "HALT") == 0) {
            ++loc;
        } else {
            die_line(line_no, "unsupported operation");
        }
    }
}

static void emit(uint16_t word, int line_no)
{
    if (word_count >= MAX_WORDS) {
        die_line(line_no, "too much output");
    }
    words[word_count++] = word;
}

static void emit_stringz(const char *s, int line_no)
{
    const char *start = strchr(s, '"');
    const char *end = strrchr(s, '"');
    if (start == NULL || end == NULL || start == end) {
        die_line(line_no, "invalid .STRINGZ");
    }

    for (const char *p = start + 1; p < end; ++p) {
        if (*p == '\\' && p + 1 < end) {
            ++p;
            switch (*p) {
                case 'n': emit('\n', line_no); break;
                case 't': emit('\t', line_no); break;
                case '\\': emit('\\', line_no); break;
                case '"': emit('"', line_no); break;
                default: die_line(line_no, "unsupported escape sequence");
            }
        } else {
            emit((uint16_t)(unsigned char)*p, line_no);
        }
    }
    emit(0, line_no);
}

static void second_pass(void)
{
    uint16_t loc = 0;

    for (int i = 0; i < line_count; ++i) {
        char line[MAX_TEXT];
        snprintf(line, sizeof(line), "%s", lines[i].text);
        int line_no = lines[i].line_no;

        char *cursor = trim(line);
        if (!starts_with_directive_or_opcode(cursor)) {
            (void)next_token(&cursor);
        }

        char *op = next_token(&cursor);
        if (op == NULL) {
            continue;
        }
        uppercase(op);

        if (strcmp(op, ".ORIG") == 0) {
            char *arg = next_token(&cursor);
            loc = parse_number(arg, line_no);
            emit(loc, line_no);
        } else if (strcmp(op, ".END") == 0) {
            return;
        } else if (strcmp(op, ".STRINGZ") == 0) {
            emit_stringz(cursor, line_no);
            loc = (uint16_t)(origin + word_count - 1);
        } else if (strcmp(op, "LEA") == 0) {
            char *dr_text = next_token(&cursor);
            char *label = next_token(&cursor);
            if (dr_text == NULL || label == NULL) {
                die_line(line_no, "LEA needs register and label");
            }

            int dr = parse_register(dr_text, line_no);
            uint16_t target = find_label(label, line_no);
            int offset = (int)target - ((int)loc + 1);
            if (offset < -256 || offset > 255) {
                die_line(line_no, "LEA target is out of range");
            }
            emit((uint16_t)(0xE000 | (dr << 9) | (offset & 0x1FF)), line_no);
            ++loc;
        } else if (strcmp(op, "PUTS") == 0) {
            emit(0xF022, line_no);
            ++loc;
        } else if (strcmp(op, "HALT") == 0) {
            emit(0xF025, line_no);
            ++loc;
        } else {
            die_line(line_no, "unsupported operation");
        }
    }
}

static void write_obj(const char *path)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror(path);
        exit(1);
    }

    for (int i = 0; i < word_count; ++i) {
        unsigned char bytes[2];
        bytes[0] = (unsigned char)(words[i] >> 8);
        bytes[1] = (unsigned char)(words[i] & 0xFF);
        fwrite(bytes, 1, 2, file);
    }

    fclose(file);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.asm output.obj\n", argv[0]);
        return 2;
    }

    load_lines(argv[1]);
    first_pass();
    if (!have_origin) {
        fprintf(stderr, "missing .ORIG\n");
        return 1;
    }
    second_pass();
    write_obj(argv[2]);
    return 0;
}

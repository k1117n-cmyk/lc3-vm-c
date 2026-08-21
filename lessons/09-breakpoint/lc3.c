#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
/* unix only */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load registar */
    OP_STR,    /* store registar */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};
enum
{
    MR_KBSR = 0xFE00,  /* keyboard status */
    MR_KBDR = 0xFE02   /* keyboard data */
};
enum
{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

#define MEMORY_MAX (1 << 16)
#define OPCODE_COUNT 16

uint16_t memory[MEMORY_MAX];  /* 65536 locations */
uint16_t reg[R_COUNT];

static uint64_t total_instructions;
static uint64_t op_counts[OPCODE_COUNT];

struct termios original_tio;

void disable_input_buffering(void)
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

uint16_t check_key(void)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void handle_interrupt(int signal)
{
    (void)signal;

    restore_input_buffering();
    printf("\n");
    exit(-2);
}

uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

void update_flags(uint16_t r)
{
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

void read_image_file(FILE* file)
{
    /* the origin tells us where in memory to place the image */
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    /* we know the maximum file size so we only need one fread */
    uint16_t max_read = MEMORY_MAX - origin;
    uint16_t* p = memory + origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    /* swap to little endian */
    while (read-- > 0)
    {
        *p = swap16(*p);
        ++p;
    }
}

int read_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");
    if (!file) { return 0; }
    read_image_file(file);
    fclose(file);
    return 1;
}

void mem_write(uint16_t address, uint16_t val)
{
    memory[address] = val;
}

uint16_t mem_read(uint16_t address)
{
    if (address == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

// 追加: x3002 や 0x3002 のような文字列をLC-3アドレスへ変換する
static int parse_address(const char *s, uint16_t *out)
{
    char *end;
    unsigned long value;

    if (s == NULL || *s == '\0') {
        return 0;
    }

    if (s[0] == 'x' || s[0] == 'X') {
        ++s;
    }

    if (*s == '\0') {
        return 0;
    }

    value = strtoul(s, &end, 16);
    if (*end != '\0' || value > 0xFFFF) {
        return 0;
    }

    *out = (uint16_t)value;
    return 1;
}

static const char *opcode_name(uint16_t op)
{
    switch (op) {
        case OP_BR: return "BR";
        case OP_ADD: return "ADD";
        case OP_LD: return "LD";
        case OP_ST: return "ST";
        case OP_JSR: return "JSR";
        case OP_AND: return "AND";
        case OP_LDR: return "LDR";
        case OP_STR: return "STR";
        case OP_RTI: return "RTI";
        case OP_NOT: return "NOT";
        case OP_LDI: return "LDI";
        case OP_STI: return "STI";
        case OP_JMP: return "JMP";
        case OP_RES: return "RES";
        case OP_LEA: return "LEA";
        case OP_TRAP: return "TRAP";
        default: return "UNKNOWN";
    }
}

static const char *trap_name(uint16_t trapvect)
{
    switch (trapvect) {
        case TRAP_GETC: return "GETC";
        case TRAP_OUT: return "OUT";
        case TRAP_PUTS: return "PUTS";
        case TRAP_IN: return "IN";
        case TRAP_PUTSP: return "PUTSP";
        case TRAP_HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

static const char *cond_name(uint16_t cond)
{
    switch (cond) {
        case FL_POS: return "P";
        case FL_ZRO: return "Z";
        case FL_NEG: return "N";
        default: return "?";
    }
}

static void trace_instruction(uint16_t pc_before, uint16_t instr, uint16_t op)
{
    printf("PC=%04X INSTR=%04X OP=%s", pc_before, instr, opcode_name(op));

    if (op == OP_TRAP) {
        uint16_t trapvect = instr & 0xFF;
        printf(" %s", trap_name(trapvect));
    }

    printf("\n");
}

static void trace_registers(void)
{
    printf("R0=%04X R1=%04X R2=%04X R3=%04X ", reg[R_R0], reg[R_R1], reg[R_R2], reg[R_R3]);
    printf("R4=%04X R5=%04X R6=%04X R7=%04X ", reg[R_R4], reg[R_R5], reg[R_R6], reg[R_R7]);
    printf("PC=%04X COND=%s\n", reg[R_PC], cond_name(reg[R_COND]));
}

static void print_profile(void)
{
    printf("\nprofile:\n");
    printf("total instructions: %llu\n\n",
           (unsigned long long)total_instructions);

    for (uint16_t op = 0; op < OPCODE_COUNT; ++op) {
        if (op_counts[op] == 0) {
            continue;
        }

        printf("%-6s %10llu\n",
               opcode_name(op),
               (unsigned long long)op_counts[op]);
    }
}

static void wait_for_step(void)
{
    int c;

    printf("step> ");
    fflush(stdout);

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    printf("\n");
}

// 追加: breakpoint到達時に、続行するまで待つ
static void wait_for_break(void)
{
    int c;

    printf("break> ");
    fflush(stdout);

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    printf("\n");
}

int main(int argc, const char* argv[])
{
    int trace_enabled = 0;
    int profile_enabled = 0;
    int step_enabled = 0;
    int break_enabled = 0; // 追加: --breakオプションが指定されたか
    uint16_t breakpoint = 0; // 追加: 停止するLC-3アドレス
    const char *image_path = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--trace") == 0) {
            trace_enabled = 1;
        } else if (strcmp(argv[i], "--profile") == 0) {
            profile_enabled = 1;
        } else if (strcmp(argv[i], "--step") == 0) {
            step_enabled = 1;
        // 追加: --breakの次の引数をブレークポイント番地として読む
        } else if (strcmp(argv[i], "--break") == 0) {
            if (i + 1 >= argc || !parse_address(argv[i + 1], &breakpoint)) {
                fprintf(stderr, "usage: %s [--trace] [--profile] [--step] [--break xADDR] image.obj\n", argv[0]);
                return 2;
            }
            break_enabled = 1;
            ++i;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "unknown option: %s\n", argv[i]);
            fprintf(stderr, "usage: %s [--trace] [--profile] [--step] [--break xADDR] image.obj\n", argv[0]);
            return 2;
        } else if (image_path == NULL) {
            image_path = argv[i];
        } else {
            fprintf(stderr, "usage: %s [--trace] [--profile] [--step] [--break xADDR] image.obj\n", argv[0]);
            return 2;
        }
    }

    if (image_path == NULL) {
        fprintf(stderr, "usage: %s [--trace] [--profile] [--step] [--break xADDR] image.obj\n", argv[0]);
        return 2;
    }

    if (step_enabled) {
        trace_enabled = 1;
    }

    if (!read_image(image_path))
    {
        printf("failed to load image: %s\n", image_path);
        exit(1);
    }

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    /* since exactly one condition flag should be set at any given time, set the Z flag */
    reg[R_COND] = FL_ZRO;

    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        /* FETCH */
        // 追加: 指定番地に到達したら、命令を実行する前に状態を表示して止める
        if (break_enabled && reg[R_PC] == breakpoint) {
            uint16_t pc = reg[R_PC];
            uint16_t instr = mem_read(pc);
            uint16_t op = instr >> 12;

            printf("breakpoint hit at x%04X\n", pc);
            trace_instruction(pc, instr, op);
            trace_registers();
            printf("\n");
            wait_for_break();
        }

        uint16_t pc_before = reg[R_PC];

        uint16_t instr = mem_read(reg[R_PC]++);

        uint16_t op = instr >> 12;

        if (profile_enabled) {
            ++total_instructions;
            ++op_counts[op];
        }

        if (trace_enabled) {
            trace_instruction(pc_before, instr, op);
        }

        switch (op)
        {
            case OP_ADD:
                {
                    /* destination register (DR) */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    /* first operand (SR1) */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    /* whether we are in immediate mode */
                    uint16_t imm_flag = (instr >> 5) & 0x1;

                    if (imm_flag)
                    {
                        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                        reg[r0] = reg[r1] + imm5;
                    }
                    else
                    {
                        uint16_t r2 = instr & 0x7;
                        reg[r0] = reg[r1] + reg[r2];
                    }

                    update_flags(r0);
                }
                break;
            case OP_AND:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t r1 = (instr >> 6) & 0x7;
                    uint16_t imm_flag = (instr >> 5) & 0x1;

                    if (imm_flag)
                    {
                        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                        reg[r0] = reg[r1] & imm5;
                    }
                    else
                    {
                        uint16_t r2 = instr & 0x7;
                        reg[r0] = reg[r1] & reg[r2];
                    }
                    update_flags(r0);
                }
                break;
            case OP_NOT:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t r1 = (instr >> 6) & 0x7;

                    reg[r0] = ~reg[r1];
                    update_flags(r0);
                }
                break;
            case OP_BR:
                {
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    uint16_t cond_flag = (instr >> 9) & 0x7;
                    if (cond_flag & reg[R_COND])
                    {
                        reg[R_PC] += pc_offset;
                    }
                }
                break;
            case OP_JMP:
                {
                    /* Also handles RET */
                    uint16_t r1 = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[r1];
                }
                break;
            case OP_JSR:
                {
                    uint16_t long_flag = (instr >> 11) & 1;
                    reg[R_R7] = reg[R_PC];
                    if (long_flag)
                    {
                        uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
                        reg[R_PC] += long_pc_offset; /* JSR */
                    }
                    else
                    {
                        uint16_t r1 = (instr >> 6) & 0x7;
                        reg[R_PC] = reg[r1]; /* JSRR */
                    }
                }
                break;
            case OP_LD:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    reg[r0] = mem_read(reg[R_PC] + pc_offset);
                    update_flags(r0);
                }
                break;
            case OP_LDI:
                {
                    /* destination registar (DR) */
                    uint16_t r0 = (instr >> 9) & 0x7;
                    /* PCoffset 9 */
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    /* add pc_offset to the current PC, look at that memory location to get the final address */
                    reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                    update_flags(r0);
                }
                break;
            case OP_LDR:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t r1 = (instr >> 6) & 0x7;
                    uint16_t offset = sign_extend(instr & 0x3F, 6);
                    reg[r0] = mem_read(reg[r1] + offset);
                    update_flags(r0);
                }
                break;
            case OP_LEA:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    reg[r0] = reg[R_PC] + pc_offset;
                    update_flags(r0);
                }
                break;
            case OP_ST:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    mem_write(reg[R_PC] + pc_offset, reg[r0]);
                }
                break;
            case OP_STI:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                    mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
                }
                break;
            case OP_STR:
                {
                    uint16_t r0 = (instr >> 9) & 0x7;
                    uint16_t r1 = (instr >> 6) & 0x7;
                    uint16_t offset = sign_extend(instr & 0x3F, 6);
                    mem_write(reg[r1] + offset, reg[r0]);
                }
                break;
            case OP_TRAP:
                reg[R_R7] = reg[R_PC];

                switch (instr & 0xFF)
                {
                    case TRAP_GETC:
                        /* read a single ASCII char */
                        reg[R_R0] = (uint16_t)getchar();
                        update_flags(R_R0);
                        break;
                    case TRAP_OUT:
                        putc((char)reg[R_R0], stdout);
                        fflush(stdout);
                        break;
                    case TRAP_PUTS:
                        {
                            /* one char per word */
                            uint16_t* c = memory + reg[R_R0];
                            while (*c)
                            {
                                putc((char)*c, stdout);
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_IN:
                        {
                            printf("Enter a character: ");
                            char c = getchar();
                            putc(c, stdout);
                            fflush(stdout);
                            reg[R_R0] = (uint16_t)c;
                            update_flags(R_R0);
                        }
                        break;
                    case TRAP_PUTSP:
                        {
                            /* one char per byte (two bytes per word)
                               here we need to swap back to
                               big endian format */
                            uint16_t* c = memory + reg[R_R0];
                            while (*c)
                            {
                                char char1 = (*c) & 0xFF;
                                putc(char1, stdout);
                                char char2 = (*c) >> 8;
                                if (char2) putc(char2, stdout);
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_HALT:
                        puts("HALT");
                        fflush(stdout);
                        running = 0;
                        break;
                }
                break;
            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;
        }

        if (trace_enabled) {
            trace_registers();
            if (running) {
                printf("\n");
            }
        }

        if (step_enabled && running) {
            wait_for_step();
        }
    }
    restore_input_buffering();

    if (profile_enabled) {
        print_profile();
    }

    return 0;
}

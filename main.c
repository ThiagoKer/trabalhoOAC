#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 100

// Converte registrador "x0".."x31" para seu número inteiro
int reg_to_int(const char *reg) {
    if (reg[0] == 'x') return atoi(&reg[1]);
    return -1;
}

// Converte número para string binária de tamanho fixo
void int_to_bin(unsigned int val, int bits, char *out) {
    for (int i = bits - 1; i >= 0; i--)
        out[bits - 1 - i] = (val & (1 << i)) ? '1' : '0';
    out[bits] = '\0';
}

void to_binary(char *line, FILE *out) {
    char rd[10], rs1[10], rs2[10];
    int imm;

    char bin[33];
    bin[32] = '\0';

    if (sscanf(line, "add %[^,], %[^,], %s", rd, rs1, rs2) == 3) {
        int_to_bin(0b0000000, 7, bin);
        int_to_bin(reg_to_int(rs2), 5, bin + 7);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b000, 3, bin + 17);
        int_to_bin(reg_to_int(rd), 5, bin + 20);
        int_to_bin(0b0110011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "xor %[^,], %[^,], %s", rd, rs1, rs2) == 3) {
        int_to_bin(0b0000000, 7, bin);
        int_to_bin(reg_to_int(rs2), 5, bin + 7);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b100, 3, bin + 17);
        int_to_bin(reg_to_int(rd), 5, bin + 20);
        int_to_bin(0b0110011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "addi %[^,], %[^,], %d", rd, rs1, &imm) == 3) {
        int_to_bin(imm & 0xFFF, 12, bin);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b000, 3, bin + 17);
        int_to_bin(reg_to_int(rd), 5, bin + 20);
        int_to_bin(0b0010011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "sll %[^,], %[^,], %s", rd, rs1, rs2) == 3) {
        int_to_bin(0b0000000, 7, bin);
        int_to_bin(reg_to_int(rs2), 5, bin + 7);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b001, 3, bin + 17);
        int_to_bin(reg_to_int(rd), 5, bin + 20);
        int_to_bin(0b0110011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "lw %[^,], %d(%[^)])", rd, &imm, rs1) == 3) {
        int_to_bin(imm & 0xFFF, 12, bin);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b010, 3, bin + 17);
        int_to_bin(reg_to_int(rd), 5, bin + 20);
        int_to_bin(0b0000011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "sw %[^,], %d(%[^)])", rs2, &imm, rs1) == 3) {
        int imm11_5 = (imm >> 5) & 0x7F;
        int imm4_0 = imm & 0x1F;
        int_to_bin(imm11_5, 7, bin);
        int_to_bin(reg_to_int(rs2), 5, bin + 7);
        int_to_bin(reg_to_int(rs1), 5, bin + 12);
        int_to_bin(0b010, 3, bin + 17);
        int_to_bin(imm4_0, 5, bin + 20);
        int_to_bin(0b0100011, 7, bin + 25);
        fprintf(out, "%s\n", bin);
    } else if (sscanf(line, "bne %[^,], %[^,], %d", rs1, rs2, &imm) == 3) {
        char b[33] = {0};

        int imm12 = (imm >> 12) & 0x1;
        int imm11 = (imm >> 11) & 0x1;
        int imm10_5 = (imm >> 5) & 0x3F;
        int imm4_1 = (imm >> 1) & 0xF;

        // imm[12], imm[10:5], rs2, rs1, funct3, imm[4:1], imm[11], opcode
        int_to_bin(imm12, 1, b);
        int_to_bin(imm10_5, 6, b + 1);
        int_to_bin(reg_to_int(rs2), 5, b + 7);
        int_to_bin(reg_to_int(rs1), 5, b + 12);
        int_to_bin(0b001, 3, b + 17);
        int_to_bin(imm4_1, 4, b + 20);
        int_to_bin(imm11, 1, b + 24);
        int_to_bin(0b1100011, 7, b + 25);

        fprintf(out, "%s\n", b);
    } else {
        fprintf(out, "// Ignorado: %s", line);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s entrada.asm [-o saida.txt]\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *out = stdout;
    if (argc == 4 && strcmp(argv[2], "-o") == 0) {
        out = fopen(argv[3], "w");
        if (!out) {
            perror("Erro ao abrir arquivo de saída");
            fclose(in);
            return 1;
        }
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        to_binary(line, out);
    }

    fclose(in);
    if (out != stdout) fclose(out);

    return 0;
}

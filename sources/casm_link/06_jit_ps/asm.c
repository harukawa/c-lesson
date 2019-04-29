#include "test_util.h"
#include "asm.h"
#include <stdio.h>

int asm_stmdb_one(int reg) {
    // Data processing P48
    int stmdb = 0xe92d0000;
    int register_list = 0x1;

    stmdb += register_list << reg;
    return stmdb;  
}

int asm_ldmia_two(int reg, int reg2) {
    // Data processing P48
    int ldmia = 0xe8bd0000;
    int register_list = 0x1;

    ldmia += register_list << reg;
    ldmia += register_list << reg2;
    return ldmia;
}

int asm_add(int rd, int rn, int operand) {
    // Data processing P29
    int add = 0xe0800000;
    
    add += rd << 12;
    add += rn << 16;
    add += operand;
    return add;
}

int asm_sub(int rd, int operand, int rn) {
    // Data processing P29
    int sub = 0xe0400000;
    
    sub += rd << 12;
    sub += rn << 16;
    sub += operand;
    return sub;
}

int asm_mov_immediate(int rd, int operand) {
    // Data processing P29
    int mov = 0xe3a00000;
    
    mov += rd << 12;
    mov += operand;
    return mov;
}

int asm_mov_register(int rd, int operand) {
    // Data processing P29
    int mov = 0xe1a00000;
    
    mov += rd << 12;
    mov += operand;
    return mov;
}

int asm_mul(int rd, int rm, int rs) {
    // Data processing P40
    int mul = 0xe0000090;
    
    mul += rd << 16;
    mul += rs << 8;
    mul += rm;
    return mul;
}

static void test_asm_stmdb_one() {
    int expect = 0xe92d0010; // stmdb r13!, {r4}
    int actual;

    actual = asm_stmdb_one(4);
    assert_int_eq(expect, actual);
}

static void test_asm_ldmia_two() {
    int expect = 0xe8bd0110; // ldmia r13!, {r4,r8}
    int actual;

    actual = asm_ldmia_two(4, 8);
    assert_int_eq(expect, actual);
}

static void test_asm_add() {
    int expect = 0xe0823001; // add r3, r2, r1
    int actual;

    actual = asm_add(3, 2, 1);
    assert_int_eq(expect, actual);
}

static void test_asm_sub() {
    int expect = 0xe0423001; // sub r3, r2, r1
    int actual;

    actual = asm_sub(3, 2, 1);
    assert_int_eq(expect, actual);
}

static void test_asm_mov_immediate() {
    int expect = 0xe3a01011; // mov r1, #17
    int actual;

    actual = asm_mov_immediate(1, 17);
    assert_int_eq(expect, actual);
}

static void test_asm_mov_register() {
    int expect = 0xe1a0100c; // mov r1, r12
    int actual;

    actual = asm_mov_register(1, 12);
    assert_int_eq(expect, actual);
}

static void test_asm_mul() {
    int expect = 0xe0030192; // mul r3, r2, r1
    int actual;

    actual = asm_mul(3, 2, 1);
    assert_int_eq(expect, actual);
}

static void unit_tests() {
    test_asm_stmdb_one();
    test_asm_ldmia_two();
    test_asm_add();
    test_asm_sub();
    test_asm_mov_immediate();
    test_asm_mov_register();
    test_asm_mul();
}
#if 0
int main() {
    unit_tests();
    return 0;
}
#endif


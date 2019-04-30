#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "parser.h"
#include "test_util.h"
#include "asm.h"

extern int eval(int r0, int r1, char *str);

struct Emitter{
    unsigned char *buf;
    int pos;
};

struct Emitter emitter;
static unsigned char g_byte_buf[1024];

static void emit_word(struct Emitter *emitter, int oneword) {
    char *buf = &oneword;
    int pos = emitter->pos;
    for(int i=0; i<4; i++) {
        emitter->buf[pos+i] = buf[i];
    }
    emitter->pos = pos + 4;
}

static void emit_output(struct Emitter *emitter, int pos, int *outword) {
    int oneword = 0x0;
    int number;
    for(int i=3; i >= 0; i--) {
        if(i != 3) {
            oneword = oneword << 8;
        }
        number = emitter->buf[pos+i];
        oneword += number;
    }
    *outword = oneword;
}

static void jit_asm_div(struct Emitter *emitter) {
    int oneword;
    oneword = asm_mov_immediate(4, 1); // mov r4, #1
    emit_word(emitter, oneword);
    // loop
    emit_word(emitter, 0xe1530002); // cmp r3, r2
    emit_word(emitter, 0x0a000002); // beq end
    oneword = asm_sub(3,3,2);        // sub r3, r3, r2
    emit_word(emitter, oneword);
    emit_word(emitter, 0xe2844001); // add r4, r4, #1
    emit_word(emitter, 0xeafffffa); // b loop

    // end
    oneword = asm_mov_register(2, 4); // mov r2, r4
    
    emit_word(emitter, oneword);
}

/*
JIT
*/
int *binary_buf = NULL;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void ensure_jit_buf() {
    if(binary_buf == NULL) {
        binary_buf = allocate_executable_buf(1024);
    }
}

void set_emitter_buf(struct Emitter *emitter) {
    int oneword;
    int index_buf = 0;
    for(int i=0; i<emitter->pos; i = i+4) {
        emit_output(emitter, i, &oneword);
        binary_buf[index_buf] = oneword;
        index_buf++;
    }
}

int* jit_script(char *input) {
    ensure_jit_buf();
    /*
    TODO: emit binary here
    */
    // setup emitter
    emitter.buf = g_byte_buf;
    emitter.pos = 0;

    //eval
    struct Substr remain={input, strlen(input)};
    int oneword, val;

    while(!is_end(&remain)) {
        skip_space(&remain);
        if(is_number(remain.ptr)) {
            oneword = asm_mov_immediate(2, parse_number(remain.ptr)); // mov r2, #immediate
            emit_word(&emitter, oneword);
            oneword = asm_stmdb_one(2); // stmdb r13!, {r2}
            emit_word(&emitter, oneword);
            skip_token(&remain);
            continue;

        }else if(is_register(remain.ptr)) {
            if(remain.ptr[1] == '1') {
                oneword = asm_stmdb_one(1); // stmdb r13!, {r1}
            } else {
                oneword = asm_stmdb_one(0); // stmdb r13!, {r0}
            }
            emit_word(&emitter, oneword);
            skip_token(&remain);
            continue;
        } else {
            // must be op.
            int arg1, arg2;

            val = parse_word(&remain);
            skip_token(&remain);

            oneword = asm_ldmia_two(2,3); // ldmia r13!, {r2, r3}
            emit_word(&emitter, oneword);

            switch(val) {
                case OP_ADD:
                    oneword = asm_add(2,2,3); // asm r2, r2, r3
                    emit_word(&emitter, oneword);
                    break;
                case OP_SUB:
                    oneword = asm_sub(2,3,2); // sub r2, r3, r2
                    emit_word(&emitter, oneword); 
                    break;
                case OP_MUL:
                    oneword = asm_mul(2,2,3); // mul r2, r2, r3
                    emit_word(&emitter, oneword);              
                    break;
                case OP_DIV:
                    jit_asm_div(&emitter);  
                    break;
            }
            oneword = asm_stmdb_one(2); //  stmdb r13!, {r2}
            emit_word(&emitter, oneword);
            continue;
        }
    }    

    oneword = asm_mov_register(0,2); // mov r0 r2
    emit_word(&emitter, oneword);
    oneword = asm_mov_register(15,14); // mov r15 r14
    emit_word(&emitter, oneword);

    // setup binary
    set_emitter_buf(&emitter);
    return binary_buf;
}

static void test_add() {
    int (*funcvar)(int, int);
    char *input = "10 8 add";
    int expect = 18;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void test_register() {
    int (*funcvar)(int, int);
    char *input = "r1 r0 add";
    int expect = 12;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(7, 5);
    assert_int_eq(expect, actual);
}

static void test_sub() {
    int (*funcvar)(int, int);
    char *input = "10 8 sub";
    int expect = 2;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void test_sub_multi() {
    int (*funcvar)(int, int);
    char *input = "20 8 sub 6 sub 3 sub";
    int expect = 3;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void test_mul() {
    int (*funcvar)(int, int);
    char *input = "4 5 mul";
    int expect = 20;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void test_div() {
    int (*funcvar)(int, int);
    char *input = "8 4 div";
    int expect = 2;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void test_multi() {
    int (*funcvar)(int, int);
    char *input = "8 4 div 5 mul 2 div 4 add 5 sub";
    int expect = 4;
    int actual;

    funcvar = (int(*)(int, int))jit_script(input);
    actual = funcvar(1, 5);
    assert_int_eq(expect, actual);
}

static void run_unit_tests() {
    test_register();
    test_add();
    test_sub();
    test_sub_multi();
    test_div();
    test_multi();
    printf("all test done\n");
}

//#if 0
int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();

    res = eval(1, 5, "3 7 add r1 sub 4 mul");
    printf("res=%d\n", res);

    /*
     TODO: Make below test pass.
    */
   funcvar = (int(*)(int, int))jit_script("3 7 add r1 sub 4 mul");
    printf("funcvar\n");
    res = funcvar(1, 5);
    printf("res %d\n",res);
    assert_int_eq(20, res);

    res = funcvar(1, 4);
    assert_int_eq(24, res);

    return 0;
}
//#endif


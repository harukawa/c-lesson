#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "parser.h"
#include "test_util.h"

extern int eval(int r0, int r1, char *str);

struct Emitter{
    unsigned char *buf;
    int pos;
};

struct Emitter emitter;
static unsigned char g_byte_buf[1024];

void emit_word(struct Emitter *emitter, int oneword) {
    char *buf = &oneword;
    int pos = emitter->pos;
    for(int i=0; i<4; i++) {
        emitter->buf[pos+i] = buf[i];
    }
    emitter->pos = pos + 4;
}

void emit_output(struct Emitter *emitter, int pos, int *outword) {
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
        printf("set %d %x \n",index_buf, oneword);
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
    struct Substr remain={str, strlen(str)};
    int oneword;

    while(!is_end(&remain)) {
        skip_space(&remain);
        if(is_number(remain.ptr)) {
            oneword = asm_mov_immediate(2, parse_number(remain.ptr)); // mov r2, #immediate
            emit_word(&emitter, oneword);
            oneword = asm_stmdb_one(2); // stmdb r13!, {r2}
            emit_word(&emitter, oneword);
            continue;

        }else if(is_register(remain.ptr)) {
            if(remain.ptr[1] == '1') {
                oneword = asm_stmdb_one(1); // stmdb r13!, {r1}
            } else {
                oneword = asm_stmdb_one(2); // stmdb r13!, {r2}
            }
            emit_word(&emitter, oneword);
            continue;
        } else {
            // must be op.
            int arg1, arg2;

            val = parse_word(&remain);
            skip_token(&remain);

            oneword = asm_ldmia_two(2,3); // ldmia r13!, {r2, r3}

            switch(val) {
                case OP_ADD:
                    oneword = asm_add(2,2,3); // asm r2, r2, r3
                    break;
                case OP_SUB:
                    oneword = asm_sub(2,2,3); // sub r2, r2, r3
                    break;
                case OP_MUL:
                    oneword = asm_mul(2,2,3); // mul r2, r2, r3              
                    break;
                case OP_DIV:
                    //oneword = asm_div(2,2,3); 
                    break;
            }
            emit_word(&emitter, oneword);
            oneword = asm_stmdb_one(2); //  stmdb r13!, {r2}
            continue;
        }
    }
    
    // setup binary
    oneword = asm_mov_register(15,14); // mov r15 r14
    emit_word(&emitter, oneword);
    //emit_word(&emitter, 0xe1a0f00e); // mov r15 r14
    set_emitter_buf(&emitter);

    return binary_buf;
}


static void run_unit_tests() {
    printf("all test done\n");
}


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

    res = funcvar(1, 5);
    printf("res %d\n",res);
    assert_int_eq(20, res);

    res = funcvar(1, 4);
    assert_int_eq(24, res);

    return 0;
}


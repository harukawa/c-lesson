int asm_stmdb_one(int reg);
int asm_ldmia_two(int reg, int reg2);
int asm_add(int rd, int rn, int operand);
int asm_sub(int rd, int rn, int operand);
int asm_mov_immediate(int rd, int operand);
int asm_mov_register(int rd, int operand);
int asm_mul(int rd, int rm, int rs);



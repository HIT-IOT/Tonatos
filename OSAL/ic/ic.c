#include <configs.h>
// this function called by POA
const long ctx_addr = CTX;
void store_context(void)
{
    asm(" MOVA R15, ctx_addr");
    asm(" MOVA R14, ctx_addr+4");
    asm(" MOVA R13, ctx_addr+8");
    asm(" MOVA R12, ctx_addr+12");
    asm(" MOVA R11, ctx_addr+16");
    asm(" MOVA R10, ctx_addr+20");
    asm(" MOVA R9, ctx_addr+24");
    asm(" MOVA R8, ctx_addr+28");
    asm(" MOVA R7, ctx_addr+32");
    asm(" MOVA R6, ctx_addr+36");
    asm(" MOVA R5, ctx_addr+40");
    asm(" MOVA R4, ctx_addr+44");
    asm(" MOVA R3, ctx_addr+48");
    asm(" MOVA R2, ctx_addr+52");
    asm(" MOVA R1, ctx_addr+56");
    asm(" MOVA R0, ctx_addr+60");
}

int exist_context(void)
{
    int data = (*(int*) ctx_addr);
    if (data == 0x55AA) {
        return 0;
    }
    return 1;
}

void restore_context(void)
{
    asm(" MOVA ctx_addr,R15");
    asm(" MOVA ctx_addr+4,R14");
    asm(" MOVA ctx_addr+8,R13");
    asm(" MOVA ctx_addr+12,R12");
    asm(" MOVA ctx_addr+16,R11");
    asm(" MOVA ctx_addr+20,R10");
    asm(" MOVA ctx_addr+24,R9");
    asm(" MOVA ctx_addr+28,R8");
    asm(" MOVA ctx_addr+32,R7");
    asm(" MOVA ctx_addr+36,R6");
    asm(" MOVA ctx_addr+40,R5");
    asm(" MOVA ctx_addr+44,R4");
    asm(" MOVA ctx_addr+48,R3");
    asm(" MOVA ctx_addr+52,R2");
    asm(" MOVA ctx_addr+56,R1");
    asm(" MOVA ctx_addr+60,R0");
}

void init_context(void)
{
    int data = (*(int*) ctx_addr);
    data = 0x55AA;
}

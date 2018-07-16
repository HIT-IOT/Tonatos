#include <configs.h>
#include <mm.h>

unsigned long end_addr = MEM_END;

mcb mems[CONFIG_MEMS];

void *mem_alloc_no_release(unsigned long size)
{
    end_addr -= size;
    return (void*) end_addr;
}

char* mem_alloc(unsigned long size)
{
    int i, selected = -1;
    for (i=0; i<CONFIG_MEMS;i++)
    {
        if (size <= mems[i].blockSize && mems[i].nFreeBlks > 0)
        {
            selected = i;
            break;
        }
    }

    if (selected == -1)
        return NULL;

    char* ret = (char*) mems[selected].freeLst;
    mems[selected].freeLst = (*(long*) ret);
    return ret;
}

void mem_free(char* ptr, unsigned long size)
{
    int i, selected;
    for (i=0; i<CONFIG_MEMS;i++)
    {
        if (size <= mems[i].blockSize && ptr >= mems[i].addr && ptr <= (mems[i].blockSize*mems[i].nBlks))
        {
            selected = i;
            break;
        }
    }

    if (selected == -1)
        return;

    if ((long)ptr == mems[i].addr)
    {
        (*(long*)ptr) = mems[i].freeLst;
        mems[i].freeLst = (long)ptr;
    } else {
        long tmp = (*(long*) ((long)ptr -size));
        (*(long*) ((long)ptr -size)) = (long) ptr;
        (*(long*)ptr) = (*(long*)tmp);
    }
}

void mem_init(void)
{
    int i;
    for(i=0;i<CONFIG_MEMS;i++)
    {
        mems[i].addr = CONFIG_MEM_START + i*32*1024;
        mems[i].blockSize = 1 << (i+2);
        mems[i].freeLst = mems[i].addr;
        mems[i].nBlks = 32*1024 / mems[i].blockSize;
        mems[i].nFreeBlks = mems[i].nBlks;
        long tmpaddr = mems[i].addr;
        while(tmpaddr != mems[i].addr + (mems[i].nBlks - 1) * mems[i].blockSize )
        {
           *(long*) (tmpaddr) = (long) tmpaddr + mems[i].blockSize;
           tmpaddr += mems[i].blockSize;
        }

        *(long*) (mems[i].nBlks*mems[i].blockSize) = 0x55AA;
    }
}

#include "memdebug.h"
#include <stdlib.h>
#include <avr/io.h>
#include "Arduino.h"
 
 
size_t getMemoryUsed(void)
{
  size_t used;
  FREELIST *fp;
 
  /* __brkval = 0 thì chưa có memory được cấp phát */
  if(__brkval == 0) {
    return 0;
  }
  /* __brkval di chuyển từ __malloc_heap_start đến __malloc_heap_end khi bộ nhớ heap được sử dụng */
  used = __brkval - __malloc_heap_start;
 
 
  /* Vì heap đã được sử dụng nên có thể có những block đã được cấp phát và free(). Nên có những block đang
  chờ tái sử dụng. */
  for(fp = __flp; fp != NULL; fp = fp->nx) {
    used -= fp->sz + HEAP_BLOCK_HEADER_SIZE;
  }

  return used;
}
 

/**
 * @note
 * - __malloc_margin: vùng đệm nhỏ(thường là vài chục byte) được thêm sau vùng heap cuối cùng. Vùng này để tránh 
 * xung đột giữa stack và heap nếu phát triển quá gần nhau.
 * - AVR_STACK_POINTER_REG: Con trỏ stack hiện tại
 */
size_t getFreeMemory(void) {
  return (size_t)AVR_STACK_POINTER_REG - (size_t)__malloc_margin - (size_t)__malloc_heap_start - getMemoryUsed();
}
 
 
/**
 * Block này có thể đến từ:
 * + Block đã từng được malloc/free - hiện đang do FREELIST quản lí => getLargestBlockInFreeList();
 * + Block chưng từng được sử dụng => getLargestNonFreeListBlock()
 */
size_t getLargestAvailableMemoryBlock()
{
  size_t largestFreeListBlock;
  size_t largestUnusedHeapBlock;
 
  largestFreeListBlock = getLargestBlockInFreeList(); 
  largestUnusedHeapBlock = getLargestNonFreeListBlock();
 
  return largestFreeListBlock > largestUnusedHeapBlock ? largestFreeListBlock : largestUnusedHeapBlock;
}
 
size_t getLargestBlockInFreeList()
{
  FREELIST *fp;
  size_t maxsize = 0;
 
  for(fp = __flp; fp ; fp = fp->nx)
    if(fp->sz > maxsize)
      maxsize = fp->sz;
 
  return maxsize;
}
 
int getNumberOfBlocksInFreeList()
{
  FREELIST *fp;
  int i;
 
  for(i = 0, fp = __flp ; fp ; fp = fp->nx , i++);
  return i;
}
 
 
/**
 * Get total size of free list (includes library overhead)
 */
 
size_t getFreeListSize()
{
    FREELIST *fp = __flp;
    size_t total = 0;

    while(fp) {
        total += fp->sz + HEAP_BLOCK_HEADER_SIZE;  
        fp = fp->nx;
    }
    return total;
}

 
/**
 * Kích thước lớn nhất của vùng bộ nhớ khả dụng nhưng chưa nằm trong freelist.
 * Ví dụ: vùng trống giữa heap và stack mà malloc() có thể được cấp phát, nhưng chưa từng được malloc/free.
 */
 
size_t getLargestNonFreeListBlock()
{
  char *cp,*brkval;
 
  /* Xác định điểm bắt đầu của vùng heap đang dùng 
     - Nếu chưa bao giờ malloc => __brkval = 0
  */
  brkval = __brkval == 0 ? __malloc_heap_start : __brkval;
 
  /*
  __malloc_heap_end có thể được cấu hình tùy chỉnh bởi linker và user.
  */
  cp = __malloc_heap_end; 
  if(cp == NULL)
    cp = (char *)AVR_STACK_POINTER_REG - __malloc_margin;

  if(cp <= brkval)
    return 0;
 
  return cp - brkval;
}

/* ---- Debug API ---- */
void printMallocHeader(void *p) {
    if (p == NULL) {
        // Serial.println("Pointer is NULL!");
        return;
    }

    uint8_t *ptr = (uint8_t*)p;

    // trừ 2 byte để vào header
    uint8_t header1 = *(ptr - 2); // low byte size
    uint8_t header2 = *(ptr - 1); // high byte size và 0 nếu block < 256

    Serial.print("Header bytes: ");
    Serial.print("0x");
    Serial.print(header1, HEX);
    Serial.print(" ");
    Serial.print("0x");
    Serial.println(header2, HEX);
}
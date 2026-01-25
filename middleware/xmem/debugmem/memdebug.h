#ifndef H_MEMORY_DEBUG_H
#define H_MEMORY_DEBUG_H
 
/**
 * Reference: https://andybrown.me.uk/2011/01/01/debugging-avr-dynamic-memory-allocation/ 
 */

#ifdef __cplusplus
extern "C" {
#endif
 
#include <stddef.h>

/**
 * This must match the definition in "stdlib_private.h"
 */
// https://github.com/avrdudes/avr-libc/blob/main/libc/stdlib/stdlib_private.h#L35
extern char *__brkval;
extern char *__malloc_heap_start;
extern char *__malloc_heap_end;
extern size_t __malloc_margin;

typedef struct __freelist {
	size_t sz;
	struct __freelist *nx;
} FREELIST;

extern FREELIST *__flp;

/*
 * Comlier AVR (GCC AVR) định nghĩa: #define  unsigned int size_t 
 * 
 * Trong kiến trúc AVR: unsigned int = 2 byte
 */ 
#define HEAP_BLOCK_HEADER_SIZE sizeof(size_t)


/**
 * @brief Tính tổng số byte Heap đang thực sự "được sử dụng" bởi các malloc() hiện tại, loại trừ các block đã 
 * được free().
 * @param none
 * @retval Số byte heap đang chiếm dụng.
 */
size_t getMemoryUsed(void);

/**
 * @brief Tính toán các byte Heap còn trống. 
 * @param none
 * @retval Giá trị trả về là tổng vùng nhớ heap còn trống, nhưng chưa chắc đã liên tục. Có thể chứa những block nhỏ 
 * do phân mảnh memory.
 */
size_t getFreeMemory(void);

/**
 * @brief Tìm block memory phù hợp để cấp phát
 * @param none
 * @retval Kích thước của block memory phù hợp
 */
size_t getLargestAvailableMemoryBlock(void);

/**
 * @brief Kích thước block memory lớn nhất trong freelist
 * @param none
 * @retval Kích thước block  memory
 */
size_t getLargestBlockInFreeList(void);

/**
 * @brief Kích thước của block memory lớn nhất trong heap (vùng nhớ liên tục chưa được sử dụng), không nằm trong freelist
 * @param none
 * @retval Kích thước block memory
 */
size_t getLargestNonFreeListBlock(void);

/**
 * @brief Tính số lượng block có trong freelist
 * @param none
 * @retval Số lượng block
 */
int    getNumberOfBlocksInFreeList(void);


/**
 * @brief Tính tổng các block nằm trong freelist
 * @param none
 * @retval Giá trị của tổng các block
 */
size_t getFreeListSize(void);

 
#ifdef __cplusplus
}
#endif
 

/* ---- Debug API ---- */
/**
 * @brief In ra terminal header của block được cấp phát bởi malloc().
 */
void printMallocHeader(void *p);



#endif // H_MEMORY_DEBUG_H
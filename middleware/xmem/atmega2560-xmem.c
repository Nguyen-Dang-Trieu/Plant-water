/**
 * Extended Memory interface for the Atmega2560 MCU.
 */

#include <stdlib.h>
#include <avr/io.h>

#include "conf_xmem.h"
#include "atmega2560-xmem.h"

/* Nếu dung lượng bộ nhớ không phải bội số của 64KB, chúng ta cần xác định kích thước của bank cuối cùng. */
// Xác định ranh giới kết thúc của __heap_malloc_end
#if (XMEM_TOTAL_MEMORY % 65536) == 0
#define XMEM_LAST_BANK_END  ((void *)0xffff)
#else
#define XMEM_LAST_BANK_END  ((void *)((XMEM_TOTAL_MEMORY % 65536) - 1))
#endif

/* Atmega XMEM address space block */
#define XMEM_START      ((void *)0x2200)
#define XMEM_END        ((void *)0xffff)

/* The address space to use for 8KB low SRAM  */
#define XMEM_SHADOWED_START ((void *)0x8000)
#define XMEM_SHADOWED_END   ((void *)0x9fff)  // Thay đổi địa chỉ này tùy theo dung lượng SRAM

struct bank_heap_state {
    void *__brkval;             /* Pointer between __malloc_heap_start and __malloc_heap_end, shows growth. */
    void *__flp;                /* Pointer to the free block list that malloc handles. */
    char *__malloc_heap_start;  /* Pointer to the beginning of the heap. */
    char *__malloc_heap_end;    /* Pointer to the end of the heap, 0 if the heap is below the stack. */
};

/* Private heap variables */
#ifdef __cplusplus
extern "C" {
#endif
    extern void *__flp;     // Linked list quản lí các block trống
    extern void *__brkval;  // Đỉnh hiện tại của heap
#ifdef __cplusplus
}
#endif


/* Lưu trữ trạng thái Heap của internal RAM. */
struct bank_heap_state _system_heap_state;

/* Mảng dùng để lưu trạng thái heap cho từng bank. Nếu có nhiều bank > 64KB */
struct bank_heap_state _bank_state[XMEM_BANKS]; 

/**
 * @brief Cờ (flag) chỉ vị trí hiện tại của heap.
 *
 * Theo dõi xem việc cấp phát bộ nhớ động (malloc/free) đang sử dụng SRAM nội bộ
 * hay RAM ngoài (XMEM) được quản lý bởi thư viện này.
 *
 * Các giá trị:
 * -  0 = False : Heap đang ở RAM ngoài (XMEM). malloc/free hoạt động trên bank hiện tại.
 * -  1 = True  : Heap đang ở SRAM nội bộ. malloc/free hoạt động trên RAM nội bộ.
 *
 * @note Không thay đổi trực tiếp biến này. Sử dụng xmem_set_system_heap() hoặc
 *       xmem_set_xmem_heap() để chuyển vị trí heap một cách an toàn.
 */
uint8_t _system_heap_in_place = 0;


/* BANK hiện tại mà Heap đang sử dụng */
uint8_t _current_bank = -1; // initial _current_bank

/**
 * @brief Lưu trạng thái của heap của bank hiện tại
 * @param current_bank Bank hiện tại muốn lưu trạng thái của Heap.
 * @retval none
 * 
 */
static void _xmem_save_bank_state (struct bank_heap_state *current_bank) {
    current_bank->__brkval              = __brkval;
    current_bank->__flp                 = __flp;
    current_bank->__malloc_heap_start   = __malloc_heap_start;
    current_bank->__malloc_heap_end     = __malloc_heap_end;
}

/**
 * @brief Khôi phục lại trạng thái Heap của bank mong muốn
 * @param desired_bank Bank mong muốn sử dụng.
 * @retval none
 * 
 */
static void _xmem_load_bank_state (struct bank_heap_state *desired_bank) {
    __brkval            = desired_bank->__brkval;
    __flp               = desired_bank->__flp;
    __malloc_heap_start = desired_bank->__malloc_heap_start;
    __malloc_heap_end   = desired_bank->__malloc_heap_end;
}

/**
 * @brief Mở vùng 8KB đầu của external RAM bị che bởi internal RAM.
 * @param none
 * @retval Pointer đến block 8KB đầu của external RAM.
 * @note Sau khi gọi hàm này, MCU có thể truy cập trực tiếp 8KB đầu của external RAM. Gọi xmem_restore_full_memory()
 *  khi xong để trả external RAM về chế độ full-address.
 */
void *xmem_access_lower_8kb(void) {
    /* Configures PORTC pins as output pins. */
    DDRC = 0xff;

    /* Port C has all 0s now. */
    PORTC = 0x00;

    /* Release the 5,6,7 pins from extended memory addressing duty. They are still
       addressing memory, they are just always set to 0 and that will leave us with
       only 13 pins (8KB) of address in external memory. Since these pins are zeroed out,
       you will be effectively addressing the lower 8KB of external memory. */
    XMCRB = (1 << XMM0) | (1 << XMM1);

    return XMEM_SHADOWED_START;
}

/**
 * @brief Khôi phục chế độ full-address của external RAM. 
 * @param none
 * @retval none
 * @note Sau khi gọi xmem_access_lower_8kb(), external RAM bị giới hạn chỉ còn 8KB đầu. Hàm này sẽ đặt lại các bit 
 * địa chỉ cao của bus external memory, trả external RAM về chế độ bình thường (full 16-bit addressable).
 */
void xmem_restore_full_memory(void) {
    /* Configures PORTC pins as output pins. */
    DDRC = 0xff;

    /* Reset Port C. */
    PORTC = 0x00;

    /* Set every pin to regular memory addressing duty. */
    XMCRB = 0;
}

/**
 * @brief Chuyển đổi các BANK MEMORY (64KB ~ 1 BANK) để hoạt động.
 * @param bank Bank mà user muốn sử dụng
 * @retval none
 */
void xmem_switch_bank (uint8_t bank) {
    if (_current_bank == bank || bank >= XMEM_BANKS) {
        return;
    }

    /* _system_heap_in_place = 0 -> Đang sử dụng external RAM */
    if (!_system_heap_in_place) {
        /* Lưu trạng thái của bank heap hiện tại */
        _xmem_save_bank_state(&_bank_state[_current_bank]);

        /* Khôi phục lại trạng thái bank mà user muốn sử dụng */
        _xmem_load_bank_state(&_bank_state[bank]);
    }

    _current_bank = bank;

    /* Have the user set the higher bits */
    XMEM_USER_SWITCH_BANK(bank);
}

/**
 * @brief Chuyển từ external RAM sang internal RAM để cấp phát malloc().
 * @param none
 * @retval none
 */
void xmem_set_system_heap (void) {
    /* Nếu đang sử dụng internal RAM -> Không làm gì!  */
    if (_system_heap_in_place) {
        return;
    }

    _xmem_save_bank_state(&_bank_state[_current_bank]);
    _xmem_load_bank_state(&_system_heap_state);

    // Flag = 1 -> Sử dụng internal RAM để cấp phát malloc().
    _system_heap_in_place = 1;
}

/**
 * @brief Chuyển đổi từ internal RAM sang external RAM để cấp phát cho malloc().
 * @param none
 * @retval none
 */
void xmem_set_xmem_heap (void) {
    /* Nếu đang sử dụng external RAM -> Không làm gì! */
    if (!_system_heap_in_place) {
        return;
    }

    _xmem_save_bank_state(&_system_heap_state);
    _xmem_load_bank_state(&_bank_state[_current_bank]);

    // Flag = 0 -> Sử dụng external RAM để cấp phát malloc().
    _system_heap_in_place = 0; 
}

/**
 * @brief Trả về địa chỉ bắt đầu của bank hiện tại
 * @param none
 * @retval Địa chỉ bắt đầu heap của bank
 */
void *xmem_get_current_bank_address_start (void) {
    return (void *)_bank_state[_current_bank].__malloc_heap_start;
}

/**
 * @brief Trả về địa chỉ kết thúc của bank hiện tại
 * @param none
 * @retval Địa chỉ kết thúc heap của bank
 */
void *xmem_get_current_bank_address_end (void) {
    return (void *)_bank_state[_current_bank].__malloc_heap_end;
}

/**
 * @brief Khởi tạo external SRAM 
 * @param none
 * @retval none
 */
void xmem_init (void) {
    XMCRB = 0;

    /* Ram ngoài được bật và thiết lập tốc độ truy cập */
    XMCRA = (1 << SRE) | (XMEM_WAIT_STATES << SRW10);

    /* Have the user configure his extra pins. */
    XMEM_USER_INIT();

    /* Lưu trạng thái Heap của internal RAM hiện tại */
    _xmem_save_bank_state(&_system_heap_state);

    /**
     * The variables __malloc_heap_start and __malloc_heap_end can be used to restrict the malloc() function to a 
     * certain memory region. These variables are statically initialized to point to __heap_start and __heap_end, 
     * respectively, where __heap_start is filled in by the linker to point just beyond .bss, and __heap_end is 
     * set to 0 which makes malloc() assume the heap is below the stack.
     * 
     * Link: https://www.datsi.fi.upm.es/docencia/DMC/avr-libc-user-manual-2.2.0/malloc.html
     */

    /* avr-libc malloc/free được chuyển sang sử dụng RAM ngoài. */
    __malloc_heap_start = (char *)XMEM_START;
    __malloc_heap_end   = (char *)XMEM_END;
    __brkval            = (char *)XMEM_START;

#ifdef XMEM_MULTIBANK
    /* Tất cả bank đều có kích thước là 64KB, ngoại trừ bank cuối cùng cần phải tính */
    for (uint8_t i = 0; i < XMEM_BANKS - 1; i++) {
        _xmem_save_bank_state(&_bank_state[i]);
    }
#endif /* XMEM_MULTIBANK */

    /**
     * Tính toán lại địa chỉ của __malloc_heap_end trong trường hợp:
     *  - Có nhiều banks và bank cuối cùng < 64KB.
     *  - Hoặc nếu chỉ có một bank duy nhất nhưng < 64KB.
     */
    __malloc_heap_end   = (char *)XMEM_LAST_BANK_END;
    _xmem_save_bank_state(&_bank_state[XMEM_BANKS - 1]);

    // Set flag _system_heap_in_place = 0 -> Sử dụng external SRAM.
    _system_heap_in_place = 0; 
  
    xmem_switch_bank(0);
}

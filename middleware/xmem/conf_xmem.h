#ifndef H_CONF_XMEM_H
#define H_CONF_XMEM_H

/**
 * Extended Memory interface for the Atmega2560 MCU.
 * 
 * Configuration file.
 */

/* Dung lượng của external SRAM  */
#define XMEM_TOTAL_MEMORY  32768  // Tùy chỉnh với mỗi kích thước SRAM khác nhau: 23KB, 64KB, 128KB, ...

/**
 * Khởi tạo thêm chân GPIO nếu cần thiết.
 */
#define XMEM_USER_INIT() ((void) 0)

/* Nếu sử dụng SRAM > 64KB thì user cần tự động thiết lập cơ chế chuyển đổi các bank khác nhau. Dễ hiểu hơn là
 * macro này sẽ điều khiển trực tiếp phần cứng để có thể "chọn bank".
 */
#define XMEM_USER_SWITCH_BANK(bank_) ((void) 0)

/* Số lượng wait-states cho external sram truy cập, phụ thuộc vào tốc độ của SRAM so với MCU
   0 = No wait-states.
   1 = Wait one cycle during read/write strobe.
   2 = Wait two cycles during read/write strobe.
   3 = Wait two cycles during read/write and wait one cycle before driving out new address */
#define XMEM_WAIT_STATES  0

#endif /* H_CONF_XMEM_H */

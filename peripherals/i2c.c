#include "i2c.h"

void I2Cx_Init(void) {
    // Tần số = F_CPU / (16 + 2*TWBR*Prescaler)
    TWSR = 0x00;               // Prescaler = 1
    TWBR = 72;                 // Tốc độ I2C ~100kHz nếu F_CPU = 16MHz
    TWCR = (1 << TWEN);        // Bật TWI
}

void I2Cx_Start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);   // Gửi START
    while (!(TWCR & (1 << TWINT)));                     // Đợi START hoàn thành
}

void I2Cx_Stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);   // Gửi STOP
    // Không cần đợi TWINT ở đây
}

void I2Cx_Write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);                 // Bắt đầu gửi
    while (!(TWCR & (1 << TWINT)));                    // Đợi gửi xong
}

uint8_t I2Cx_Read_ACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);   // ACK
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t I2Cx_Read_NACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);                 
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t I2Cx_GetStatus(void) {
    uint8_t status = TWSR & 0xF8;                      // Chỉ lấy 5 bit trạng thái
    return status;
}

uint8_t I2Cx_RequestFrom(uint8_t slave_address, uint8_t num_bytes, uint8_t* buffer) {
    if (num_bytes == 0) return 0;

    I2Cx_Start();

    // Send slave address (7-bit) and Read bit (1-bit)
    I2Cx_Write((slave_address << 1) | 0x01);

    // uint8_t status = TWI_GetStatus();

    uint8_t bytes; // number bytes of read
    for (bytes = 0; bytes < num_bytes; bytes++) {
        if (bytes < num_bytes - 1) {
            buffer[i] = I2Cx_Read_ACK();   // ACK -> continue read
        } else {
            buffer[i] = I2Cx_Read_NACK(); // NACK -> end read
        }
    }

    I2Cx_Stop();
    return bytes; 
}

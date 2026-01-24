#ifndef I2C_H_
#define I2C_H_

void I2Cx_Init();
void I2Cx_Start();
void I2Cx_Stop();
void I2Cx_Write();
uint8_t I2Cx_Read_ACK();
uint8_t I2Cx_Read_NACK();
uint8_t I2Cx_GetStatus(void);
uint8_t I2Cx_RequestFrom(uint8_t slave_address, uint8_t num_bytes, uint8_t* buffer);

#endif
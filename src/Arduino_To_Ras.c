#include "i2c.h"
#include "gpio.h"
#include "adc.h"
#include "uart.h"
#include "DHT22.h"      
#include "BH1750.h"                                     

#define DHT22_Pin         3   // Pin D3                                               
#define RELAY_Pin         2   // Pin D2                                 
#define SOIL_MOISTURE_Pin 0   // Pin A0                                   


/* Variables contains necessary data to be send to Rasp Pi */
char   motorState[10];
float  temperatureData;
int    humidityData;
float  lightData;
String messageUartData; // Variable storing combined data from motor, temperature, humidity, light

struct DHT22 DHT22_Sensor;

void initSystem() {
    ADCx_Init();
    USARTx_Init(MYUBRR);

    /* Initialize the DHT22, BH1750 sensor */
    DHT22_Init(55, DHT22_Pin, DHT22_Sensor);
    BH1750_Init(CONTINUOUS_HIGH_RES_MODE, BH1750_DEFAULT_MTREG);

    /* Configure the pin MODE*/
    GPIOx_ConfigMode(&DDRA, &PORTA, SOIL_MOISTURE_Pin, INPUT);
    GPIOx_ConfigMode(&DDRD, &PORTD, RELAY_Pin, OUTPUT);
}

int readSoilMoisture() {
  int raw = ADCx_Read(SOIL_MOISTURE_Pin);
  int percent = map(raw, 0, 1023, 0, 100);
  return 100 - percent;
}

void readDHT22() {
  temperatureData = DHT22_ReadTemperature(false, DHT22_Sensor);
  humidityData    = DHT22_ReadHumidity(DHT22_Sensor);
}

void readLightSensor() {
  lightData = BH1750_ReadData();
}

void controlRelay(int soilMoisture) {
  if (soilMoisture < 45) {
    strcpy(motorState, "OON");
    GPIOx_Write(RELAY_Pin, HIGH);
  } else {
    strcpy(motorState, "OFF");
    GPIOx_Write(RELAY_Pin, LOW);
  }
}

void sendUartData() {
  sprintf(messageUartData, "%s%05.2f%02d", motorState, temperatureData, (int)humidityData);
  USARTx_Print(messageUartData);
}


void setup() {
  initSystem();
}

void loop() {
  int soilMoistureData = readSoilMoisture();
  readDHT22();
  readLightSensor();
  controlRelay(soilMoistureData);
  sendUartData();
  delay(1000); 
  
}

/* Example:
motorState = "OON";
temperatureData = 28.53;
humidityData = 7;

Data send = "OON28.5307\n"
 */



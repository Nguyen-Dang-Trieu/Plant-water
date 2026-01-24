#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include <errno.h>
#include "unistd.h"
#include "MQTTClient.h"
#include <wiringPi.h>
#include <wiringSerial.h>

/*
Broker: broker.emqx.io
TCP Port: 1883 
*/
#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "publisher_demo"
#define PUB_TOPIC   "k19"
#define QOS         1

//Dinh nghia dung cho uart 
#define BUFFER_SIZE 200
#define UART_DEVICE "/dev/ttyAMA0"

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("Message '%s' with delivery token %d delivered\n", payload, token);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //conn_opts.username = "your_username>>";
    //conn_opts.password = "password";

    /* MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL); */

    int rc;  // return code
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
   
    //listen for operation
    // MQTTClient_subscribe(client, SUB_TOPIC, 0);
    
    // ----------------------- Phan code giao tiep UART ----------------------------------- //
    int fd;
    char c;
    char buffer[BUFFER_SIZE];
    int bufferIndex = 0;
    char Motor[4];      //trạng thái hoạt động của motor
    char value1[6];
    char value2[3];
    
	//2 biến lưu tempareture và humidity
	float temperature;
	int humidity;

    if ((fd = serialOpen(UART_DEVICE, 9600)) < 0) {
        fprintf(stderr, "Khong the mo cua UART: %s\n", strerror(errno));
        return 1;
    }


    // 3 giá trị cần phải gửi từ ras lên MQTT : motor, temperature, humidity
    char motor_mqtt[4];
    float temperature_mqtt;
    int humidity_mqtt;

    
    
    while(1) {
        // -------------------------- Kiem tra du lieu va gan gia tri cac bien - UART ------------------------- //
        if (serialDataAvail(fd)) {
            c = serialGetchar(fd);
            if (c == '\n') {
                buffer[bufferIndex] = '\0';
                
                strncpy(motor_mqtt, buffer, 3);
                motor_mqtt[3] = '\0';

                strncpy(value1, buffer + 3, 5);
                value1[5] = '\0';

                strncpy(value2, buffer + 8, 2);
                value2[2] = '\0';
                
                //Hien thi trang thai haot dong cua dong co: ON or OFF
                //printf("Motor: %s\n", motor_mqtt);

                //Hiển thị nhiệt độ
				temperature_mqtt = atof(value1);      // chuyển kí tự về dạng float
                //printf("Tempareture: %0.2f\n", temperature_mqtt);

				//Hiển thị độ ẩm
				humidity_mqtt = atoi(value2);         // chuyển kí tự về dạng int
                //printf("Humidity: %d\n", humidity_mqtt);

                // Reset buffer và chỉ số
                memset(buffer, 0, sizeof(buffer));
                bufferIndex = 0;

                //--------------------------------- Phan gui message len MQTT ------------------------------- //
                int mode ;
                if(strcmp(motor_mqtt, "OON") == 0) {
                    mode = 1;
                }
                else if(strcmp(motor_mqtt, "OFF") == 0) {
                    mode = 0;
                }
                char msg [20];
                //sprintf(msg,"Motor: %s Temperature: %0.2f Humidity: %0.2f",motor,temperature,humidity);
                sprintf(msg,"%d %0.2f %d",mode,temperature_mqtt,humidity_mqtt);
                //send temperature measurement
                publish(client, PUB_TOPIC, msg);
            } else {
                buffer[bufferIndex] = c;
                bufferIndex++;
                if (bufferIndex >= BUFFER_SIZE) {
                    // Buffer đã đầy, xử lý lỗi hoặc thực hiện reset buffer tại đây
                    memset(buffer, 0, sizeof(buffer));
                    bufferIndex = 0;
                }
            }
        }
        
        /*
        //--------------------------------- Phan gui message len MQTT ---------------------------------------- //
        char msg [20];
        //sprintf(msg,"Motor: %s Temperature: %0.2f Humidity: %0.2f",motor,temperature,humidity);
        sprintf(msg,"%s %0.2f %d",motor_mqtt,temperature_mqtt,humidity_mqtt);
        //send temperature measurement
        publish(client, PUB_TOPIC, msg);
        sleep(3);
        */
        
    }
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}

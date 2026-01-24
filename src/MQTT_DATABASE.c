#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <stdio.h>
#include <mysql.h>

//------------------------------------------ Kết nối MQTT ---------------------------------------------//
#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "subscriber_demo"
#define SUB_TOPIC   "k19"
#define QOS         1


//------------------------------------------ Kết nối MySQL---------------------------------------------//
char *server = "localhost"; 
char *user = "trongtin";
char *password = "123"; 
char *database = "IOT";

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}


char motor[10];
float temperature;
int humidity;

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    printf("\nReceived message: %s\n", payload);
    sscanf(payload,"%s %f %d",motor,&temperature, &humidity);
   
    printf("\nmotor: %s",motor);  
    printf("\ntemperature: %0.2f",temperature); 
    printf("\nhumidity: %d",humidity); 
   
    
    MYSQL *conn = mysql_init(NULL);
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }  
    
    char sql[200];
    sprintf(sql, "INSERT INTO DULIEU (mode,temperature, SOIL) VALUES ('%s','%0.2f','%d')", motor,temperature,humidity);
    
    if (mysql_query(conn, sql) != 0) {
           fprintf(stderr, "Error: %s\n", mysql_error(conn));
    }
   
    mysql_close(conn); 

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //conn_opts.username = "your_username>>";
    //conn_opts.password = "password";


    //hàm MQTTClient_setCallbacks sẽ được gọi mỗi khi nhận được tin nhắn mới từ Borket MQTT
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
   
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, QOS);

    
    while(1) {
        //send temperature measurement
        // publish(client, PUB_TOPIC, "HELLO WORLD!");
        // sleep(3);
    }
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}

#include <DHT.h>                                                // Gọi thư viện DHT22

#define DHTPIN 7                                                // Đọc dữ liệu từ DHT22 ở chân D7 trên mạch Arduino
#define DHTTYPE  DHT22                                          // Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
#define relay 2                                                 // Chân kích relay D2

DHT dht(DHTPIN, DHTTYPE);

int cambien = A0;                                               // Chân cảm biến ở chân Analog: A0


//Các biến dùng để gửi dữ liệu sang ras
char Motor[10];
float temperature;
int humidity;
String mes_uart;

void setup() {
  Serial.begin(9600);                                           // opens serial port, baudrate : 9600 bps
  dht.begin();                                                  // Khởi động cảm biến
  pinMode (cambien, INPUT);                                     //Tín hiệu vào từ cảm biến
  pinMode(relay,OUTPUT);                                        // Tín hiệu ra chân D0 dùng để kích relay

}
 
void loop() {
  digitalWrite(relay,HIGH);
  // -------------------------------------------- Đọc tín hiệu analog từ cảm biến độ ẩm đất --------------------------------------------------- //
  int doc_cambien = analogRead(cambien);
  int phantram = map(doc_cambien,0,1023,1,100);
  int phantramthuc = 100 - phantram;
  humidity = phantramthuc;
  
  //Serial.print("Humidity: ");
  //Serial.println(humidity);

  //Serial. print("Analog: "); Serial. print(doc_cambien); Serial. print(" "); Serial. print("~"); Serial. print(" ");
  //Serial. print(phantramthuc); Serial. println("%");


  
  // ----------------------------------------------------- Hiển thị trạng thái của động cơ ---------------------------------------------------- //
  if(phantramthuc  < 45)                        // Độ ẩm đất < 40% thì động cơ sẽ hoạt động để bơm nước tưới cho cây
  {
    strcpy(Motor,"OON");
    //Serial.println(Motor);                    // Dùng để test
    digitalWrite(relay,HIGH);                   // Cho động cơ chạy

    // -------------------------------------------- Điều khiển role bât tắt để máy bơm nước --------------------------------------------- //
    /*
     Code here
    //myStepper.step(stepsPerRevolution); 
    */
  }
  else 
  {
    strcpy(Motor,"OFF");
    //Serial.println(Motor);                    // Dùng để test
    digitalWrite(relay,LOW);                    // Cho động cơ tắt
  }
  phantramthuc = 0;
  
  
  
  // -----------------------------------------------------------  Code DHT22 -------------------------------------------------------------------- //
  float h = dht.readHumidity();                 // Đọc độ ẩm khong khi
  float t = dht.readTemperature();              // Đọc nhiệt độ cua DHT22
  
  temperature = t;
  
  //Serial.print("Nhiet do: ");
  //Serial.println(t); //Xuất nhiệt độ
  
  
  // --------------------------------------- Gửi dữ liệu qua cho ras thông qua UART -------------------------------------------- //

  String humidity_Str;
  if (humidity < 10) {
    humidity_Str = "0" + String(humidity);      // Nếu độ ẩm dưới 10, thêm số 0 phía trước
  } else {
    humidity_Str = String(humidity);
  }
  mes_uart = Motor + String(temperature,2) + humidity_Str;
  Serial.println(mes_uart);
  
  

  delay(2000); //2 giây 1 lần lặp 
  
}

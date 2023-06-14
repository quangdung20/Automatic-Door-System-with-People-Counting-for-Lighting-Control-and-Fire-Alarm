#include<Wire.h>
#include <Servo.h> 
#include <LiquidCrystal_I2C.h> // Khai thư viện I2C xem lại TẠI ĐÂY
LiquidCrystal_I2C lcd(0x27,16,2); // LCD 16×02
Servo servo; 
#define servo_pin 9
// Khai báo digital Input cho IR sensor
int IR1=2;   
int IR2=3;  
int Led = 4;       
int PeopleInRoom;  // biến để lưu số người trong phòng
String state = ""; // state để lưu trữ trạng thái vào ra
int timeoutcounter = 0;
// // Khai báo digital I/O cho Flame sensor and buzzer
int buzzer = 11;      // digital pin 11 for buzzer ( còi báo động )
int flame = 5;       // digital pin for flame sensor
int flash_light = 12; // đèn nháy báo hiệu
const unsigned int servoInterval = 2000;  // time servo đóng
unsigned int prevServo = 0;

//unsigned int warningtime =0;
bool dooropen = false; // trạng thái mở cửa
bool Alarm = false; // trạng thái báo động

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");
  delay(1000);
  // put your setup code here, to run once:
  pinMode(IR1, INPUT); 
  pinMode(IR2, INPUT);
  pinMode(Led, OUTPUT);
  pinMode(flame, INPUT);
  pinMode(flash_light, OUTPUT); 
  pinMode(buzzer, OUTPUT);
  servo.attach(servo_pin);
  servo.write(100);  // trạng thái đóng cửa
  Serial.begin(9600);
  PeopleInRoom=0;
  delay(500); 
}

void loop() {

  int val_in = digitalRead(IR1);  
  int val_out = digitalRead(IR2);
  int val_flame=digitalRead(flame); 
  
  unsigned long current= millis(); 
  contrlo_servo(val_in , val_out);
 thời gian thực tại 
//   Kiểm tra xem cửa có vật cản không thì mở cửa
   if ((!val_in || !val_out) && dooropen==false) {
    servo.write(0); 
    dooropen = true; 
    prevServo = current; // lưu thời gian lúc mở cửa
  }
  // Kiểm tra thời gian để đóng cửa tự động
//   kiểm tra xem cửa đã mở chưa, 
//    thời gian cửa đóng là 2s từ khi mở,
//    và kiểm tra xem cửa có vật cản không,
//    không có thì đóng.
  if ( current - prevServo >= servoInterval) { 
     // cửa đóng
    if(val_in && val_out && dooropen){ 
      dooropen = false;
    }
    servo.write(100); // di chuyển servo về vị trí đóng
    prevServo = current;
  }

 // Thực hiện đếm người ra/vào phòng qua 4 trạng thái  
  if(PeopleInRoom==0){digitalWrite(Led,LOW);}
  else {digitalWrite(Led,HIGH);}
  if(val_in==0 && state.charAt(0)!='1')
  {
    state+="1";
  }
  else if(val_out==0 && state.charAt(0)!='2')
  {
    state+="2";
  }
 
  if(state.equals("12"))
  {
    PeopleInRoom++;
    state="";
    delay(500);
  }
 else if(state.equals("21") && PeopleInRoom>0)
 {
    PeopleInRoom--;
    state="";
    delay(500);
 }
 // Reset state nếu giá trị hàng đợi sai (11 hoặc 22) hoặc hết thời gian chờ
  if(state.length()>2 || state.equals("11") || state.equals("22") ||timeoutcounter>200)
  {
      state="";
  }
  if(state.length()==1)
  {timeoutcounter++;}
  else {timeoutcounter=0;}

  // thiết lập hiển thị mà hình LCD các trạng thái của căn phòng
     if (val_flame == 1 && Alarm == false){
     Serial.print("A");
     Serial.print(PeopleInRoom);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Number People:");
     lcd.setCursor(1,1);
     lcd.print(PeopleInRoom);
     delay(100);
    }
      if(val_flame == 0){
        Serial.print("X");
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("WARNING!!!");
        lcd.setCursor(3,1);
        lcd.print("WARNING!!!");
        delay(100);
    }
//  hệ thống báo cháy  
    if (val_flame == 1) {
        digitalWrite(flash_light,LOW); //tắt đèn báo
        noTone(buzzer);    
        Alarm = false;
      } 

    if(val_flame == 0&& Alarm == false) {
      Alarm = true;
      digitalWrite(flash_light, HIGH); // bật đèn báo động
      tone(buzzer,1500); //bật còi
      delay(50);
      dooropen = true;    
    }
}

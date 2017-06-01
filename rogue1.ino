#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
//lcd i2c
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  
//bluetooth
#define TX 12 //11 nano   
#define RX 13 //12 nano
SoftwareSerial bluetooth(RX,TX); 
//motor two
int enB = 5;
int in3 = 7;//forward
int in4 = 6;//backward
char stateB="3";
char old_stateB="3";
int counter=1;

//sensor
#define DHTPIN 2          
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);




void setup() {
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  initInfo();
  Serial.println("rogue 1 connected!");
  bluetooth.begin(38400);
  //start sensor
  dht.begin(); 
  //motor conf
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  delay(1000);
  lcd.clear();
}

void loop() {
  //reciveCmd();
  reciveMsg();
  displaySensor();
  if(stateB =='1'){
    goForwardB();
  }else if(stateB =='2'){
    goBackwardB();
  }else if(stateB =='3'){
    stopB();
  }
  heartbeatMsg(counter);
  delay(500);
}


void reciveCmd(){
  if(Serial.available()>0){
    Serial.println("data received");
    stateB=Serial.read(); 
  } 
}

void reciveMsg(){
  if(bluetooth.available()>0){
    stateB=bluetooth.read(); 
  } 
}


void sendMsg(String msg){
  bluetooth.println(msg);
  Serial.println(msg);
}

void silentStopB(){
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stopB(){
  if(old_stateB==stateB){
    return;
  }
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  displayMsg("stop motor");
  //store state
  old_stateB=stateB;
}

void goForwardB(){
  if(old_stateB==stateB){
    return;
  }
  if(old_stateB=='2'){
    silentStopB();
    delay(500);   
   }
   digitalWrite(in3, HIGH);//pierwszy przod
   digitalWrite(in4, LOW);
   displayMsg("go forward");
   //store state
   old_stateB=stateB;
}

void goBackwardB(){
  if(old_stateB==stateB){
    return;
  }
  if(old_stateB=='1'){
    silentStopB();
    delay(500);   
   }
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  displayMsg("go backward");
  old_stateB=stateB;
  }

void initInfo(){
  //i2c implementation
  lcd.home();   
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); 
  lcd.print("Initializing");
  for(int i=0;i<15;i++){
    lcd.setCursor(i,1 );
    lcd.print(".");
    delay(200);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("rogue1");
  lcd.setCursor(0,1);
  lcd.print("Initialized");
}


void displayMsg(char * msg){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.setCursor(0,1);
  lcd.print(msg);
  sendMsg(String(msg));
}




void heartbeatMsg(int &counter){
  if( counter%60 == 0 ){
    displayMsg("Status: OK");
    float t = dht.readTemperature();  
    float h = dht.readHumidity(); 
    if (isnan(t) || isnan(h)){
      displayMsg("sensor not working");
    }else{
      sendMsg("Temp: "+String(t)+" *C");
      sendMsg("Hum: "+String(t)+" %");
    }
    counter=1;
  }else{
    counter++;
  }
}

void displaySensor(){
  float t = dht.readTemperature();  
  float h = dht.readHumidity(); 
  if (isnan(t) || isnan(h)){
    lcd.print("is none");
  }else{
     lcd.setCursor(0,0);
     lcd.print(t);
     lcd.setCursor(6,0);
     lcd.print("*C");
     lcd.setCursor(9,0);
     lcd.print(h);
     lcd.setCursor(15,0);
     lcd.print("%");
  }
}








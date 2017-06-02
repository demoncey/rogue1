#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
//lcd i2c
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  
#define CLEAR_ROW "                "
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

//sensor DHT11
#define DHTPIN 2          
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);

//ultra sonic sensor HC-SR04

#define TRIG 11
#define ECHO 10


struct measurment
{
   float temp;
   int hum;
   int distance;
};
typedef struct measurment Measurment;



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
  //sonar
  pinMode(TRIG, OUTPUT); //Pin, do którego podłączymy trig jako wyjście
  pinMode(ECHO, INPUT); //a echo, jako wejście
  delay(1000);
  lcd.clear();
}

void loop() {
  Measurment ms;
  //reciveCmd();
  reciveMsg();
  getMeasurments(ms);
  checkStop(&ms);
  displayMeasurments(&ms);
  if(stateB =='1'){
    goForwardB();
  }else if(stateB =='2'){
    goBackwardB();
  }else if(stateB =='3'){
    stopB();
  }
  heartbeatMsg(counter,ms);
  delay(500);
}

void getMeasurments(Measurment &ms){
  float t = dht.readTemperature();  
  float h = dht.readHumidity(); 
  if (isnan(t) || isnan(h)){
    lcd.print("is none");
  }
  ms.temp=t;
  ms.hum=h;
  ms.distance=calcDistance();
}


int calcDistance(){
  long t , distance;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  t = pulseIn(ECHO, HIGH);
  distance = t / 58;
  return distance;
}

void checkStop(Measurment *ms){
  if(ms->distance<10){
    stateB='3';
    //displayMsg(String(ms->distance)+" cm",false);
  }
  
}

void reciveCmd(){
  if(Serial.available()>0){
    Serial.println("data received");
    stateB=Serial.read(); 
  } 
}

void reciveMsg(){
  if(bluetooth.available()>0){
    Serial.println("data received");
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
  lcd.setCursor(0,1);
  lcd.print(CLEAR_ROW);
  lcd.setCursor(0,1);
  lcd.print(msg);
  sendMsg(String(msg));
}

void displayMsg(String msg){
  lcd.setCursor(0,1);
  lcd.print(CLEAR_ROW);
  lcd.setCursor(0,1);
  lcd.print(msg);
  sendMsg(msg);
}

void displayMsg(String msg ,boolean send){
  lcd.setCursor(0,1);
  lcd.print(CLEAR_ROW);
  lcd.setCursor(0,1);
  lcd.print(msg);
  if(send)
    sendMsg(msg);
}


void heartbeatMsg(int &counter,Measurment &ms){
  if( counter%60 == 0 ){
    displayMsg("Status: OK");
    sendMsg("Temp: "+String(ms.temp)+" *C");
    sendMsg("Hum: "+String(ms.hum)+" %");
    sendMsg("Distance: "+String(ms.distance)+" cm");
    counter=1;
  }else{
    counter++;
  }
}

void displayMeasurments(Measurment *ms){
  lcd.setCursor(0,0);
  lcd.print(ms->temp);
  lcd.setCursor(5,0);
  lcd.print("C");
  lcd.setCursor(7,0);
  lcd.print(ms->hum);
  lcd.setCursor(9,0);
  lcd.print("%");
  lcd.setCursor(11,0);
  lcd.print(ms->distance);
  lcd.setCursor(14,0);
  lcd.print("cm");
}








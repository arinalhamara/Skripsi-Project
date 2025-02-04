#include <MQUnifiedsensor.h>
#include <ESP8266WiFi.h>
#include <ThingESP.h>

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0 
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 
#define RatioMQ135CleanAir 3.6 
#define FlameSensor1 14
#define FlameSensor2 12
#define Buzz 0
#define Buzz2 15
#define Pompa1 16
#define Pompa2 2
#define Fan 3
float Tambah, Tambah2;

ThingESP8266 thing("arinalhamara", "MonitoringRuang", "fireandair");

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
char ssid[] = "SERENITY_plus";
char pass[] = "babykenya";

void setup() {
  Serial.begin(115200);
  thing.SetWiFi(ssid, pass);
  thing.initDevice();
  
  pinMode(FlameSensor1, INPUT);
  pinMode(FlameSensor2, INPUT);
  pinMode(Buzz, OUTPUT);
  pinMode(Buzz2, OUTPUT); 
  pinMode(Pompa1, OUTPUT);
  pinMode(Pompa2, OUTPUT);
  pinMode(Fan, OUTPUT);
  digitalWrite(Buzz, LOW);
  digitalWrite(Buzz2, LOW);
  digitalWrite(Pompa1, LOW);
  digitalWrite(Pompa2, LOW);
  digitalWrite(Fan, LOW);
  Tambah = 0;
  Tambah2 = 0;
  
  MQ135.setRegressionMethod(1); 
  MQ135.init(); 
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
}

String HandleResponse(String query)
{
   MQ135.update();

  MQ135.setA(80.897); MQ135.setB(-2.431);
  float LPG= MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();
  Tambah = (LPG+150);
  Tambah2 = (CO2+400);

  if (query == "Kualitas"){
    return "Gas: " + String(Tambah);
    
  }
  else if(query == "CO2"){
    return "CO2: " + String(Tambah2);
  }
  else return "SALAH ENTRI";
  
}
void loop() {
  int FireSensor1 = digitalRead(FlameSensor1);
  int FireSensor2 = digitalRead(FlameSensor2);

  thing.Handle();
  
  MQ135.update();

  MQ135.setA(80.897); MQ135.setB(-2.431);
  float LPG= MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();

  Tambah = (LPG+150);
  Tambah2 = (CO2+400);
  Serial.print("Kadar LPG : "); 
  Serial.print(Tambah);
  Serial.println(" ppm");
   
  Serial.print("Kadar CO2 : "); 
  Serial.print(Tambah2); 
  Serial.println(" ppm");

  Serial.println("----------------------");
  
  Serial.print("Nilai Flame Sensor 1 : ");
  Serial.println(FireSensor1);
  Serial.print("Nilai Flame Sensor 2 : ");
  Serial.println(FireSensor2);

  Serial.println("----------------------");
  
  /*
    Exponential regression:
  GAS      | a      | b
  CO       | 605.18 | -3.937  
  Alcohol  | 77.255 | -3.18 
  CO2      | 110.47 | -2.862
  Tolueno  | 44.947 | -3.445
  NH4      | 102.2  | -2.473
  Acetona  | 34.668 | -3.369
  */
  
  if(FireSensor1 == 0)
  {
    digitalWrite(Pompa1, HIGH);
    digitalWrite(Buzz, HIGH);
  }
  if(FireSensor2 == 0)
  {
    digitalWrite(Pompa2, HIGH);
    digitalWrite(Buzz2, HIGH);
  }
  if(FireSensor2 == 1){
    digitalWrite(Pompa2, LOW);
    digitalWrite(Buzz2, LOW);
  }
  if(FireSensor1 == 1){
    digitalWrite(Pompa1, LOW);
    digitalWrite(Buzz, LOW);
  }
  if((Tambah2)>=600 || (Tambah)>= 400)
  {
    digitalWrite(Fan, HIGH);
  }
  if((Tambah2)<500 || (Tambah)< 300)
  {
    digitalWrite(Fan, LOW);
  }
  
  delay(1000);
}

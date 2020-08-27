#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WinsenZH03.h>
#include <SoftwareSerial.h>
#include <SparkFunCCS811.h>
#include <SparkFunBME280.h>     
#include <ClosedCube_HDC1080.h>  
#include <Ticker.h>
#include <dht.h>

#define CCS811_ADDR 0x5A
CCS811 myCCS811(CCS811_ADDR);
ClosedCube_HDC1080 myHDC1080;
BME280 myBME280;
Ticker secondTick;
volatile int watchdogCount = 0;

IPAddress server_addr(139,162,149,133);  // IP of the MySQL server here
char user[] = "frendonja";              // MySQL user login username
char password[] = "frendonja";        // MySQL user login password
char ssid[] = "#Doverska 3";    // your SSID
char pass[] = "5sa61flj2knJ"; 
// Sample query
char INSERT_DATA[] = "INSERT INTO ethernet.prvi(TEMPERATURA, VLAGA, CO2, TVOC, PM1, PM2_5, PM10, MQ) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')";
char query[256];
char temperatura[10];
char vlaga[10];
char co2[10];
char voc[10];
char MQ[10];
char cPM1_0[10];
char cPM2_5[10];
char cPM10[10];
float t,h,carb2,Voc;
int sensorValue;
int PM1_0, PM2_5, PM10;


WiFiClient client;
MySQL_Connection conn((Client *)&client);

WinsenZH03 sensorZH;
SoftwareSerial mySerial (13, 12);

dht DHT;

#define DHT11_PIN 14

void ISRwatchdog()
{
  watchdogCount++;
  if(watchdogCount == 20) {
    Serial.println();
    Serial.println("WDT bites");
    ESP.restart();
    }  
}

void setup() {
  
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect
  WiFi.begin(ssid, pass);// connects to the WiFi router
  
  while (WiFi.status() != WL_CONNECTED) {    delay(500);
  }
  Serial.println("blabla");
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x76;
  myBME280.settings.runMode = 3; //Normal mode
  myBME280.settings.tStandby = 0;
  myBME280.settings.filter = 4;
  myBME280.settings.tempOverSample = 5;
  myBME280.settings.pressOverSample = 5;
  myBME280.settings.humidOverSample = 5;
  myBME280.begin();
  myHDC1080.begin(0x40);
  CCS811Core::status returnCode = myCCS811.begin();

  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println(".begin() returned with an error.");
    while (1);      //No reason to go further
  }
  mySerial.begin(9600); //UART to sensor
  sensorZH.begin(&mySerial);//configure the serial that the sensor will use
  sensorZH.setAs(QA);//configure the sensor QA mode 
  
  secondTick.attach(1,ISRwatchdog);
}

void loop() {
 delay(12000);
  watchdogCount = 0;
  Serial.println("1");
  if (myCCS811.dataAvailable())
  {
    Serial.println("2");
    watchdogCount = 0;
    
  myCCS811.readAlgorithmResults();
  Serial.println("3");
  int chk = DHT.read11(DHT11_PIN);
  t = DHT.temperature;
  h = DHT.humidity;
  Serial.println(t);
  Serial.println(h);
  carb2 = myCCS811.getCO2();
  Voc = myCCS811.getTVOC();
  }
    Serial.println("4");
    watchdogCount = 0;
    //AirQuality
    sensorValue = analogRead(0);
  Serial.println("5");
    //ZH
    sensorZH.readOnce();
    PM1_0 = sensorZH.readPM1_0();
    PM2_5 = sensorZH.readPM2_5();
    PM10 = sensorZH.readPM10();
    Serial.println("6");
    
    if (conn.connect(server_addr, 3306, user, password)) {
      watchdogCount = 0;
   Serial.println("7");
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Save
    dtostrf(t, 5, 2, temperatura);
    dtostrf(h, 5, 2, vlaga);
    dtostrf(carb2, 6, 2, co2);
    dtostrf(Voc, 7, 2, voc);
    dtostrf(sensorValue, 6, 2, MQ);
    dtostrf(PM1_0, 6, 2, cPM1_0);
    dtostrf(PM2_5, 6, 2, cPM2_5);
    dtostrf(PM10, 6, 2, cPM10);
    
    Serial.println("8");
    
    sprintf(query, INSERT_DATA, temperatura, vlaga, co2, voc, cPM1_0, cPM2_5, cPM10, MQ);
    Serial.println("9");
    // Execute the query
    cur_mem->execute(query);
    Serial.println("10");
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    delete cur_mem;
    conn.close();
    Serial.println("11");
    for (int i = 0; i<24; i++)
    {
    Serial.println("12");
    watchdogCount = 0;
    delay(12000);
    }
   }}

#include "ServoTimer2.h" //skinut biblioteku
#include <GSM.h>
#include <EEPROM.h>

ServoTimer2 myservo;  

#define PINNUMBER ""

GSM gsmAccess;
GSM_SMS sms;
GSMVoiceCall vcs;



char numtel[20]; //broj koji zove
//char broj = "+38597983139"; //naš broj koji može zvati

char senderNumber[13]; // tko šalje sms
String SMSbroj = "+385976983139"; //broj koji može upisivati druge brojeve u eeprom
int addr = 31;
char c; //bytovi smsa

String izeeproma;

void setup() {
  
  Serial.begin(9600);
  Serial.println("Pričekaj sekund"); //inicijalizacija

  bool notConnected = true;

  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  vcs.hangCall();

  Serial.println("Čekanje poziva");
}

void loop() {
  if (sms.available()) {
    Serial.println("Message received from:");
    sms.remoteNumber(senderNumber, 13); //hvatanje broja
    Serial.println(senderNumber);
     
     if(String(senderNumber) = SMSbroj){
      
        while (c = sms.read()) {
        Serial.println(c);
        
        EEPROM.put(addr, c);
       
       }
          delay(100);
     }
 }
   
  sms.flush();
  
    
 switch (vcs.getvoiceCallStatus()) {
    case IDLE_CALL: //ceka poziv

      break;

  case RECEIVINGCALL: 

      Serial.println("RECEIVING CALL");

      vcs.retrieveCallingNumber(numtel, 20); 

      Serial.print("Number:");
      Serial.print(numtel);
      Serial.println();
    
      broi();
     
      vcs.answerCall();
      break;
  case TALKING:
      
      
  vcs.hangCall();
      Serial.println("Čekanje drugog poziva");
      break;
}
}


void servo()
{
     int pos = 0;
     for (pos = 0; pos <= 1500; pos += 1) {      
     myservo.attach(9);
     myservo.write(pos);              
     delay(2);  
     myservo.detach();  
    
                         
      }
     Serial.println("Dizanje rampe");
     delay(3000);
     
     for (pos = 1500; pos >= 0; pos -= 1) {  
     myservo.attach(9);
     myservo.write(pos);              
     delay(2);
     myservo.detach();
        

}
  vcs.hangCall(); 
}



void broi()
{
   for(addr=31; addr<512; addr=addr+13){
      
       
      EEPROM.get(addr,izeeproma);
      Serial.println(izeeproma);
   
   if(String(numtel) == izeeproma)
      servo();
   else
   Serial.println("Broj nije tocan"); 
   
   }
   }

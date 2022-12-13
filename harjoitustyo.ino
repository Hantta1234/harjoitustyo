#include <Servo.h>                                      //servon kirjasto

#define JOYSTICKXPIN A0                                 //määritellään pinnit
#define JOYSTICKYPIN A1
#define BTNREADYPIN 2
#define BTNCONTROLPIN 3
#define GLEDPIN 4
#define DCMOTORPIN 5
#define LATCHPIN 10
#define DATAPIN 11
#define CLOCKPIN 12

int seg = 0;                                            //7-osaisen näytön muuttuja
int btnCounter = 0;
int angleServo = 90;                                    //servon kulman muuttuja
int speedDC = 0;                                        //DC-moottorin nopeuden muuttuja
int readyServo = false;
int angleLimit = false;
unsigned long timer = millis();                         //luodaan muuttuja ajastimelle

Servo servoMotor;
//testi1

void setup()
{
  Serial.begin(115200);
 
  servoMotor.attach(9, 500, 2500);                      //servon määritys

  pinMode(BTNREADYPIN, INPUT_PULLUP);                   //pinnien toiminnan määrittelyt
  pinMode(BTNCONTROLPIN, INPUT_PULLUP);
  pinMode(JOYSTICKXPIN, INPUT);
  pinMode(JOYSTICKYPIN, INPUT);
  pinMode(GLEDPIN, OUTPUT);
  pinMode(DCMOTORPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);

}

void loop()
{
  bool readyBtn = !digitalRead(BTNREADYPIN);            //luetaan nappien tilaa
  bool controlBtn = !digitalRead(BTNCONTROLPIN);
  int angleJoyX = map(analogRead(JOYSTICKXPIN), 0, 1023, 0, 180); //ohjaussauvan X-akselin arvo konvertoidaan asteiksi
  int angleJoyY = analogRead(JOYSTICKYPIN);             //luetaan ohjaussauvan Y-akselin asento

  
  if(angleJoyY >= 500 && angleJoyY <= 513){             //jos ohjaussauvan Y-akseli on suurinpiirtein keskellä DC-moottori ei pyöri
   speedDC = 0;
  }
  else if(angleJoyY < 500){                             //jos ohjaussauvaan poikkeutetaan Y-akselin suuntaisesti, DC-moottori pyörii
   speedDC = map(angleJoyY, 0, 500, 255, 0);            //konvertoidaan ojaussauvan arvo DC-moottorille sopivaksi
  }
  else if(angleJoyY > 513){                             //jos ohjaussauvaan poikkeutetaan Y-akselin suuntaisesti, DC-moottori pyörii
   speedDC = map(angleJoyY, 513, 1023, 0, 255);         //konvertoidaan ojaussauvan arvo DC-moottorille sopivaksi
  }
    
  analogWrite(DCMOTORPIN, speedDC);                     //syötetään arvo moottorille

  switch (btnCounter) {                                 //luetaan servon valmiustilojen määrä. Saadun määrän perusteella syötetään arvo seg muuttujaan. Seg arvo muutetaan myöhemmin binääriluvuksi.
  case 0:
    seg = 63;
    break;
    
  case 1:
    seg = 6;
    break;
    
  case 2:
    seg = 91;
    break;
    
  case 3:
    seg = 79;
    break;
    
  case 4:
    seg = 102;
    break;
    
  case 5:
    seg = 109;
    break;
    
  case 6:
    seg = 125;
    break;
    
  case 7:
    seg = 7;
    break;
    
  case 8:
    seg = 127;
    break;
    
  case 9:
    seg = 103;
    break;
    
  case 10:
    btnCounter = 0;                                           //jos valmiustiloja on ollut 10 silloin arvo muutetaan nollaksi, koska 7-osan näytöllä ei voida näyttää 9 isompaa numeroa
    break;

}

  if(readyBtn && angleJoyX == 89 && !readyServo){             //luetaan nappiarvo, onko ohjaussan X-akseli keskellä sekä onko servo valmiustilassa
    readyServo = true;                                        //asetetaan servon valmiustilan muuttuja trueksi
    digitalWrite(GLEDPIN, HIGH);                              //sytytetään ledi valmiustilan merkiksi
    btnCounter = btnCounter + 1;                              //lisätään valmiustilojen muuttujaan 1 aina kun valmiustila on kytketty päälle
    delay(500);
  }
  else if(readyBtn && readyServo){
    readyServo = false;                                       //asetetaan servon valmiustilan muuttuja falseksi
    digitalWrite(GLEDPIN, LOW);                               //sammutetaan valmiustilan merkkivalo
    delay(500);
  }

  digitalWrite(LATCHPIN, LOW);                                
  shiftOut(DATAPIN, CLOCKPIN, MSBFIRST, seg);                 //luetaan seg muuttujan luku suurin bitti ensin, lähetetään datapinniin
  digitalWrite(LATCHPIN, HIGH);
    
  
  if(readyServo){                                             //servon ohjauksen koodi
    if(controlBtn){
      if(millis() >= timer + 1000){                           //käytetään millis komentoa jumitilan välttämiseksi. Servo liikkuu 10 astetta sekunnin välein jos nappia pidetään pohjassa
        timer = millis();       
        if(angleServo >= 0 && angleLimit == false){           //luetaan servon kulma sekä kulmanrajamuuttuja
          angleServo = angleServo + 10;
          if(angleServo >= 180){                              //kun servo saavuttaa 180 astetta, asetetaan muuttuja trueksi, jotta servo osaa vaihtaa suuntaa eikä jumitu
          angleLimit = true;
          }
        }
        if(angleServo <= 180 && angleLimit == true){     
          angleServo = angleServo - 10;
          if(angleServo <= 0){                                //kun servo saavuttaa 0 astetta, asetetaan muuttuja trueksi, jotta servo osaa vaihtaa suuntaa eikä jumitu
          angleLimit = false;
          }
        }
        servoMotor.write(angleServo);
      }      
    }    
    else{ 
      servoMotor.write(angleJoyX);                            //servon ohjaus ohjainsauvalla                          
      angleServo = 90;                                        //asetetaan angleServo muuttujan arvoks 90 jotta napin ollessa taas pohjassa servo aloittaa liikkeen keskeltä  
    }
           
  }
  
  Serial.println(timer);                                      //arvojen ja muuttujien monitorointia
  Serial.println(angleServo);
  Serial.println(angleJoyX);
  Serial.println(angleJoyY);
  Serial.println(readyServo);
  Serial.println(speedDC);
  Serial.println(" ");
  delay(1);
    
}

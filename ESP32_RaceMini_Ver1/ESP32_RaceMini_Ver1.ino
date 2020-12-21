// *****************************************************************
// * Pilotage du Racemini Ver1.0001
// * Sylvain GRIMAL 2020
// * ***************************************************************

#include <HardwareSerial.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "BluetoothSerial.h"
#include "config.h"

//#include <Servo.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define ECRAN

#ifdef ECRAN
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // déclaration afficheur Oled I2C
#endif

bool freinage;

String ordreBT="";
String infoCam="";

//Servo direction;

int mode=1,n;
float dist=100,consigneVitesse=0,consigneDirection=0;

bool cam=true;

float comDirect=0, comVitesse=0;
float kp=0.5; //pas de correction de position
float pos,tangAlpha;
float xd1,yd1,xd2,yd2;
float xg1,yg1,xg2,yg2;


unsigned long tempsAff;
unsigned long tempsCom;
unsigned long tempsCli;


void setup()
{ 
  debug.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);

 
  ledcAttachPin(PWM_PIN,PWM_CANAL); // association du PWM_CANAL à la patte PWM_PIN 
  ledcSetup(PWM_CANAL,PWM_FREQ,PWM_RES); // init du canal PWM

  //direction.attach( SERVO_PIN, SERVO_CANAL, 45, 120);
   ledcSetup(SERVO_CANAL, 50, 16); // channel, 50 Hz, 16-bit width
   ledcAttachPin(SERVO_PIN, SERVO_CANAL);   // GPIO 22 assigned to channel 1
  
  delay(1000);
  debug.println("C'est parti");  

# ifdef ECRAN
  u8g2.begin();
  delay(1000);
  debug.println("u8g2 démarré");
  affichage("Initialisation Bluetooth");
#endif  

  bluetooth.begin(NOM_BT);
  debug.println("Bluetooth activé");
  
  camera.begin(115200);
  debug.println("camera connectée");
  
  tempsAff=millis()+tempoAff;
  tempsCom=millis()+tempoCom;
  tempsCli=millis()+tempoCli;
  debug.println("Setup fini");
  
}

void loop()
{
  int8_t nb;
  float xp,yp,xv,yv;
  float alpha,deltaCrem,beta;
  
  
  // reception bluetooth
  while (bluetooth.available()>0) {
    char caract=bluetooth.read();
    //debug.print(caract);
    if (caract==13) {
      gestionCommandes(ordreBT);
      ordreBT="";
    }
    else if (caract>=32) ordreBT+=caract;
  }
  // reception camera
  while (camera.available()>0) {
    char caract=camera.read();
    //debug.print(caract);
    if (caract==13) {
      gestionCamera(infoCam);
      infoCam="";      
    }
    else if (caract>=32) infoCam+=caract;
  }


  // calcul de la commande
  if (millis()>tempsCom) {
      tempsCom=millis()+tempoCom;
      // *** Mode manuel ***
      if (mode==1) {
        comDirect=consigneDirection;
        comVitesse=consigneVitesse;
      }     
      // *** Mode auto ***
      if (mode==2) {
        xp=(xg1+xd1+xg2+xd2)/4;
        yp=(yg1+yd1+yg2+yd2)/4;
        xv=yg2+yd2-yg1-yd1;
        yv=-xg2-xd2+xg1+xd1;
        // Calcul de la commande
        #ifdef TOTAL
           alpha= atan(EMPATTEMENT*yv/(xp*yv+yp*xv-yv*DEMIVOIE));
           deltaCrem = sqrt(square(LB) -square(D-LP*cos(alpha+ALPHA0)))-LP*sin(alpha+ALPHA0)-sqrt(square(LB)-square(D-LP*cos(ALPHA0)))+LP*sin(ALPHA0);
           debug.print(deltaCrem);debug.print("\t");
           beta=deltaCrem/RP*180/PI;
           debug.println(beta);  
        #endif
        #ifdef ORDRE1
          beta=COEF1*atan(yv/xv);
        #endif
        #ifdef ORDRE2
          beta=COEF1*atan(yv/xv)+COEF2*square(atan(yv/xv));
        #endif
        
        // correction position
        comDirect=beta+kp*COEFPOS*xp/yp;
      
        
        freinage=false;
        if (square(consigneVitesse)*abs(yv/(yv*xp+yp*xv))<GMAX){
          comVitesse=consigneVitesse;
        } else {
          comVitesse=sqrt(abs(GMAX*(yv*xp+yp*xv)/yv));
          debug.println("Freinage");
          freinage=true;
        }
     }      
     commande(comVitesse,comDirect);
  }
  // Clignotement led
  if (millis()>tempsCli) {
     tempsCli=millis()+tempoCli;    
     digitalWrite(LED_PIN,!digitalRead(LED_PIN));    
  }
  // Affichage ecran
  if (millis()>tempsAff) {
     tempsAff=millis()+tempoAff;    
     if (mode==1) affichage("Manu.");
     if (mode==2) affichage("Auto.");
  }
}

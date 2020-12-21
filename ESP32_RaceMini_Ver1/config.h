// definitions des constantes liées au materiel elec.

#define NOM_BT "RaceMini1"
#define LED_PIN 2

// Servo Direction
#define SERVO_PIN 19
#define SERVO_CANAL 2
#define COEFSERVO 18.2 // coef temps impulsion position depend du servo
#define ZEROSERVO 4915 // position 0
#define SERVOMAX 1638  // limitation rotation du servo
#define TRIM -300 

// Moteur
#define PWM_PIN 25
#define PWM_CANAL 0
#define PWM_FREQ 5000   // 5khz
#define PWM_RES 10 // 10 bits 

#define IN2_PIN 33 // 0 0 brake 1 1 libre 
#define IN1_PIN 32 // 1 0 forwward 0 1 reverse

// Tempo
#define tempoAff 200  // affichage 5 fois par seconde
#define tempoCli 500  // Clignotement led PC13 2 fois par seconde
#define tempoCom 50  //  commande 20 fois par seconde

#define debug Serial
BluetoothSerial SerialBT;
#define bluetooth SerialBT // port bluetooth déclaré en deux fois pour permettre debugage sur usb sans BT

#define camera Serial2



// Définition des constantes liées aux dimensions en cm

#define DEMIVOIE 9.0
#define EMPATTEMENT 21.8
#define LP 1.7
#define LB 3
#define D 1.4
#define RP 0.5
#define ALPHA0 14*PI/180

#define GMAX 100 // acc transversale max sinon freinage attention unité en %²/m: pourcentage de commande² pour un rayon de 1 m !

#define TOTAL // TOTAL equation complète // ORDRE1 modèle linéaire // ORDRE2 modèle quadratique
#define COEFPOS 45 // coefficent degre pour regul proportionnelle position avec kp normalise
#define COEF1 2
#define COEF2 0.2  

//#define RATIO 0.2 // ratio de vitesse si route perpendiculaire
//const float GAMMA=(1-RATIO)*4/(M_PI*M_PI); // pour relation vitesse angle

#define square(a) (a)*(a)

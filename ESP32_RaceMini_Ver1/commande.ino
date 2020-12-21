// Commande du moteur et du servo-moteur de direction

void commande(float vitesse,float direct){
   // commande du moteur
     if (vitesse>0) {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH); // Marche avant
        uint32_t duty = vitesse*10.23;
        ledcWrite(PWM_CANAL,duty);
        
     } else {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW); // Marche arriere
        uint32_t duty = -vitesse*10.23;
        ledcWrite(PWM_CANAL,duty);
     }
     // commande du servo de direction

     if (direct*COEFSERVO>SERVOMAX)  direct=SERVOMAX/COEFSERVO;
     if (direct*COEFSERVO<-SERVOMAX)  direct=-SERVOMAX/COEFSERVO; 
     ledcWrite(SERVO_CANAL,direct*COEFSERVO+ZEROSERVO+TRIM);
}

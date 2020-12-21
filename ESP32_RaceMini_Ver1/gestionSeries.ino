
void gestionCommandes(String ordre){
  // transmission bluetooth

  
  if (ordre.indexOf("acc")>-1) {
          consigneVitesse=ordre.substring(ordre.indexOf("acc")+3).toInt();
          Serial.print("Vitesse:");Serial.println(consigneVitesse);
          bluetooth.print("Vitesse:");bluetooth.println(consigneVitesse);

          
        }
  if (ordre.indexOf("dir")>-1){
          consigneDirection=-ordre.substring(ordre.indexOf("dir")+3).toInt();
          Serial.print("Direction:");Serial.println(consigneDirection);
          bluetooth.print("Direction:");bluetooth.println(consigneDirection);
  }
  if (ordre.indexOf("kp")>-1){
          kp=ordre.substring(ordre.indexOf("kp")+2).toFloat()/50;
          Serial.print("kp:");Serial.println(kp);
          bluetooth.print("kp:");bluetooth.println(kp);
  }
  
  if (ordre.indexOf("ral")>-1){
          dist=ordre.substring(ordre.indexOf("ral")+3).toInt();
  }
  
  if (ordre.indexOf("auto")>-1){
          mode=2;
          Serial.println("Mode auto");
          bluetooth.print("Mode auto");
          affichage("Automatique");
  }
  if (ordre.indexOf("manu")>-1){
          mode=1;
          Serial.println("Mode manu");
          bluetooth.print("Mode manu");
          affichage("Manuel");  
          consigneVitesse=0;
          consigneDirection=0;       
  }
}

void gestionCamera(String ordre){  
  // transmission camera
  // ligne de droite  
  if (ordre.indexOf("xd1")>-1){
          xd1=ordre.substring(ordre.indexOf("xd1")+3).toFloat();
  }
  if (ordre.indexOf("zd1")>-1){
          yd1=ordre.substring(ordre.indexOf("zd1")+3).toFloat();
  }
  if (ordre.indexOf("xd2")>-1){
          xd2=ordre.substring(ordre.indexOf("xd2")+3).toFloat();
  }
  if (ordre.indexOf("zd2")>-1){
          yd2=ordre.substring(ordre.indexOf("zd2")+3).toFloat();
  }
  // ligne de gauche
  if (ordre.indexOf("xg1")>-1){
          xg1=ordre.substring(ordre.indexOf("xg1")+3).toFloat();
  }
  if (ordre.indexOf("zg1")>-1){
          yg1=ordre.substring(ordre.indexOf("zg1")+3).toFloat();
  }
  if (ordre.indexOf("xg2")>-1){
          xg2=ordre.substring(ordre.indexOf("xg2")+3).toFloat();
  }
  if (ordre.indexOf("zg2")>-1){
          yg2=ordre.substring(ordre.indexOf("zg2")+3).toFloat();
          /*
          debug.print(xd1); debug.print("\t");debug.print(yd1);debug.print("\t");debug.print(xd2);debug.print("\t");debug.print(yd2);debug.print("\t");
          debug.print(xg1); debug.print("\t");debug.print(yg1);debug.print("\t");debug.print(xg2);debug.print("\t");debug.print(yg2);
          debug.println(); */
  }
}

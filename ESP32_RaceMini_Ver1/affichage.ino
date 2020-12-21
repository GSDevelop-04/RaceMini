void affichage(char* texte) {
#ifdef ECRAN   
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_courB08_tr);
  u8g2.drawStr(0,64,texte);
  
  char affiche[10];

  snprintf(affiche,sizeof(affiche),"%d",int(comVitesse));
  u8g2.drawStr(48,64,affiche);
  

  snprintf(affiche,sizeof(affiche),"%d",int(comDirect));
  u8g2.drawStr(92,64,affiche);
  
  /*    
  if (tangAlpha>=0) sprintf(affiche," %d.%03d", (int)tangAlpha, (int)(tangAlpha*1000)%1000);
  else sprintf(affiche,"-%d.%03d", (int)-tangAlpha, (int)-(tangAlpha*1000)%1000);
  
  if (pos>=0) sprintf(affiche," %d.%03d", (int)pos, (int)(pos*1000)%1000);
  else sprintf(affiche,"-%d.%03d", (int)-pos, (int)-(pos*1000)%1000);
  
  if (xd1>=0) sprintf(affiche," %d.%01d", (int)xd1, (int)(xd1*10)%10);
  else sprintf(affiche,"-%d.%01d", (int)-xd1, (int)-(xd1*10)%10);
  u8g2.drawStr(0,64,affiche);²

  if (xd2>=0) sprintf(affiche," %d.%01d", (int)xd2, (int)(xd2*10)%10);
  else sprintf(affiche,"-%d.%01d", (int)-xd2, (int)-(xd2*10)%10);
  u8g2.drawStr(30,64,affiche);
  
  if (xd1>=0) sprintf(affiche," %d.%01d", (int)xg1, (int)(xg1*10)%10);
  else sprintf(affiche,"-%d.%01d", (int)-xg1, (int)-(xg1*10)%10);
  u8g2.drawStr(60,64,affiche);

  if (xd2>=0) sprintf(affiche," %d.%01d", (int)xg2, (int)(xg2*10)%10);
  else sprintf(affiche,"-%d.%01d", (int)-xg2, (int)-(xg2*10)%10);
  u8g2.drawStr(90,64,affiche);
  */

  if (freinage) u8g2.drawBox(0,41,128,8);
  u8g2.drawRFrame(0,0,128,40,2);
  u8g2_uint_t x1=64+xd1*1.5;
  u8g2_uint_t x2=64+xd2*1.5;
  u8g2.drawLine(x1,0,x2,39);
  x1=64+xg1*1.5;
  x2=64+xg2*1.5;
  u8g2.drawLine(x1,0,x2,39);
  u8g2.sendBuffer();
#endif
}
/*  if (tangAlpha>=0) sprintf(affiche," %d.%03d", (int)tangAlpha, (int)(tangAlpha*1000)%1000);
  else sprintf(affiche,"-%d.%03d", (int)-tangAlpha, (int)-(tangAlpha*1000)%1000);
  //sprintf(affiche,"%d",int(tangAlpha*100));
  u8g2.drawStr(3,26,affiche);
  
  if (pos>=0) sprintf(affiche," %d.%03d", (int)pos, (int)(pos*1000)%1000);
  else sprintf(affiche,"-%d.%03d", (int)-pos, (int)-(pos*1000)%1000);
  u8g2.drawStr(3,36,affiche);
*/

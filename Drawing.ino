void runningInfoMode1(){
  static unsigned int x;
  if (reset_x !=0) {x=0;reset_x = 0;}
  if(adzan==1 || flag1==0) { return; }
  
  int Speed = speedText;
  
  static unsigned long lsRn;
  unsigned long Tmr = millis();
  
  int fullScroll = Disp.textWidth(text) + Disp.width() + 10;
  

  if((Tmr-lsRn)> Speed)
    { lsRn = Tmr;
      fType(0);
      if (x < fullScroll) { 
        ++x; 
        //Serial.println(String()+"x:" + x); 
        Disp.drawText(Disp.width() - x, 9, text);
      }
      else {  
        x=0; 
        return;
      }
    }
}

void runningInfoMode2(){
  static unsigned int x;
  if (reset_x !=0) {x=0;reset_x = 0;}
  if(adzan) return;
  
  int Speed = speedText;
  
  static unsigned long lsRn;
  unsigned long Tmr = millis();
  
  int fullScroll = Disp.textWidth(text) + Disp.width() + 10;
  

  if((Tmr-lsRn)> Speed)
    { lsRn = Tmr;
      fType(2);
      if (x < fullScroll) { 
        ++x; 
        //Serial.println(String()+"x:" + x); 
        Disp.drawText(Disp.width() - x, 0 , text);
      }
      else {  
        x=0; 
        return;
      }
    }
}

void runAnimasi(){
  if(adzan) return;
  RtcDateTime now = Rtc.GetDateTime();
  static int    y=0;
  static bool    s; // 0=in, 1=out              
  static unsigned long   lsRn;
  unsigned long          Tmr = millis();
  int dot    = now.Second();
  char buff_jam[20];
  
  if(dot%2){sprintf(buff_jam,"%02d:%02d",now.Hour(),now.Minute());}
  else{sprintf(buff_jam,"%02d %02d",now.Hour(),now.Minute());}
  
  if((Tmr-lsRn)>75) 
      { 
        if(s==0 and y<9){lsRn=Tmr;y++; }
        if(s==1 and y>0){lsRn=Tmr;y--; if(y == 1){ Disp.drawText(0,0, "          "); }}
      }
   if((Tmr-lsRn)>10000 and y ==9) {s=1;}

   if (y == 0 and s==1) { s=0; show = ANIM_DATE;}
  
  fType(0); 
  dwCtr(0,y-9, buff_jam); 

}

void runAnimasiJam(){
  if(adzan) return;
  RtcDateTime now = Rtc.GetDateTime();
  static int    y=0;
  static bool    s; // 0=in, 1=out              
  static unsigned long   lsRn;
  unsigned long          Tmr = millis();
  int dot    = now.Second();
  char buff_jam[20];
  
  if(dot%2){sprintf(buff_jam,"%02d:%02d",now.Hour(),now.Minute());}
  else{sprintf(buff_jam,"%02d %02d",now.Hour(),now.Minute());}
  
  if((Tmr-lsRn)>75) 
      { 
        if(s==0 and y<9){lsRn=Tmr;y++; }
        if(s==1 and y>0){lsRn=Tmr;y--; if(y == 1){ Disp.drawText(0,0, "          "); }}
      }
   if((Tmr-lsRn)>10000 and y ==9) {s=1;}

   if (y == 0 and s==1) { s=0; show = ANIM_DATE;}
  
  fType(0); 
  dwCtr(0,y-9, buff_jam); 

}

void runAnimasiDate(){
  static unsigned int x;
  if (reset_x !=0) { x=0;reset_x = 0;}
  if(adzan) return;
  const char *pasar[]     ={"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
  const char *Hari[]      ={"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
  //const char *sholatt[]   ={"SUBUH","TERBIT","DZUHUR","ASHAR","TRBNM","MAGRIB","ISYA"};
  const char *bulanMasehi[]={"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
  const char *namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
  };
//"MUHARRAM","SHAFAR","RAB.AWAL","RAB.AKHIR","JUM.AWAL","JUM.AKHIR","RAJAB","SYA'BAN","RAMADHAN","SYAWAL","DZULQA'DAH","DZULHIJAH"

  RtcDateTime now = Rtc.GetDateTime();
  static unsigned long   lsRn;
  unsigned long          Tmr = millis();
   
  int Speed = speedDate;
  byte daynow   = now.DayOfWeek();    // load day Number
  
  char buff_date[50];

  sprintf(buff_date,"%s %s %02d-%02d-%04d %02d %s %02dH",Hari[daynow],pasar[jumlahhari()%5],now.Day(),now.Month(),now.Year(),tanggalHijriah.tanggal, namaBulanHijriah[tanggalHijriah.bulan - 1], tanggalHijriah.tahun);
 
  int fullScroll = Disp.textWidth(buff_date) + Disp.width() ;
  //Serial.println("scroll:" + String(fullScroll));
    if((Tmr-lsRn)> Speed)
    { 
      lsRn = Tmr;
      if (x < fullScroll) { 
        ++x; 
        fType(0); 
        Disp.drawText(Disp.width() - x,0, buff_date);
      }
      else {  
        flag1=1;
        Disp.clear();
        x=0; 
        reset_x=1;
        show = ANIM_JAM;
        return;
      }
    }
}

void runAnimasiSholat(){
 
  if(adzan) return;
  RtcDateTime now = Rtc.GetDateTime();
  static int        y=0;
  static int        x=0;
  static byte       s=0; // 0=in, 1=out   
  static byte       s1=0;
  static byte lastList;

  if(list != lastList){s=0; s1=0; x=0; y=0;lastList = list; }

  static uint32_t   lsRn;
  uint32_t          Tmr = millis(); 
  
  const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
  int hours, minutes;
  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();
  char buff_jam[10];

  if((Tmr-lsRn)>55) 
  { 
    if(s1==0 and y<9){lsRn=Tmr; y++; }
    if(s==1 and x<33){lsRn=Tmr; x++; }
  }

  if((Tmr-lsRn)>4000 and y == 9) {s1=1; s=1;}

  if (x == 33 and s==1 and s1 == 1) { 
    s=0;
    s1=0;
    x=0;
    y=0;
     
    list++; 
    if(list==4){list=5;} 
    
    if(list==7){list=0; Disp.clear(); show=ANIM_JAM; flag1=1;}//else{list=list;}
  }

  String sholat = jadwal[list];

  get_float_time_parts(times[list], hours, minutes);

    minutes = minutes + ihtiSholat[list];

  if(minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
  }

  // if(list==5){ 
    
  // }

  sprintf(buff_jam,"%02d:%02d",hours,minutes);

  if(s1==0){
    fType(3);
    dwCtr(0,y-9, sholat);
    fType(0);
    dwCtr(0,18-y, buff_jam);
  }
  else{
    Disp.drawLine((list<6)?x-1:x,-1,(list<6)?x-1:x,16,1);
    Disp.drawLine((list<6)?x-2:x-1,-1,(list<6)?x-2:x-1,16,0);
  }
}
/*=======================================================================================================================================================*/
void drawAzzan()
  {
    const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
    String sholat = jadwal[sholatNow];
    // uint8_t           ct_limit =40;  //harus angka genap
    static uint8_t    ct;
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();
    uint8_t          limit = config.durasiadzan;
    //RtcDateTime now = Rtc.GetDateTime();
    //int detik = now.Second();
   
    if(Tmr - lsRn > 500  and ct <= limit)
      {
        lsRn = Tmr;
        if((ct%2)==0)
          { 
            fType(0); dwCtr(0,0,"ADZAN");
            fType(3); dwCtr(0,8,sholat);
            Buzzer(1);
          }
        else 
          { Buzzer(0); Disp.clear();}
        //DoSwap = true; 
        ct++;
        Serial.println("ct:" + String(ct));
      }
    if ((Tmr-lsRn)>1500 and (ct > limit))
      {
        sholatNow=-1;
        flag1=1;
        adzan = 0;
        (mode==1)?show = ANIM_JAM : show = ANIM_ZONK;
        Disp.clear();
        ct = 0;
        Buzzer(0);
        Serial.println("adzan end");
      }
  }
/*=====================================================================================================================================*/
/*========================================================================================================================================================*/
void fType(uint8_t x)
  {
    switch(x){
      case 0 :
        Disp.setFont(Font0);
      break;
      case 1 :
        Disp.setFont(Font1);
      break;
      case 2 :
        Disp.setFont(Font2);
      break;
      case 3 :
        Disp.setFont(Font3);
      break;
    };
  }

void dwCtr(int x, int y, String Msg){
   int   tw = Disp.textWidth(Msg);
    int   th = 16;
    int   c = int((DWidth-x-tw)/2);
    //Disp.drawFilledRect(x+c-1,y,x+tw+c,y+th,0);
    Disp.drawText(x+c,y,Msg);
}

const char msg[] PROGMEM = "MUSHOLLAH HIDAYATULLAH RT19/RW03,DODOKAN,TANJUNGSARI";
const char * const pasar[] PROGMEM = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
const char * const Hari[] PROGMEM = {"AHAD","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
const char msg1[] PROGMEM ="LURUSKAN DAN RAPATKAN SHAFF SHOLAT";
const char * const namaBulanHijriah[] PROGMEM = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};
 const char jadwal[][8] PROGMEM = {
    "SUBUH ", "TERBIT ", "DZUHUR ", "ASHAR ", 
    "TRBNM ", "MAGRIB ", "ISYA' "
  };


void drawTime()
{
    //if (adzan) return;
  
    RtcDateTime now = Rtc.GetDateTime();
    uint8_t detik = now.Second();
    uint8_t menit = now.Minute();
    uint8_t jam = now.Hour();
    char buff_jam[6];
    char buff_menit[6];
    static bool flag1 = false,flag2 = false;
    
    snprintf(buff_jam, sizeof(buff_jam), "%02d", jam);
    snprintf(buff_menit, sizeof(buff_menit), "%02d", menit);

    fType(4);
    Disp.drawText(16, 0, buff_jam); 
    Disp.drawText(34, 0, buff_menit); 

    // Blink colon (titik dua) setiap detik
    if (detik & 1)
    {
        Disp.drawCircle(31, 4, 1);   // Titik atas
        Disp.drawCircle(31, 11, 1);  // Titik bawah
    }
    else
    {
        Disp.drawCircle(31, 4, 1, 0);
        Disp.drawCircle(31, 11, 1, 0);
    }

    // Garis pemisah bawah
    Disp.drawLine(0, 16, 64, 16);

     if (jam >= 21  or jam < 3){
        Disp.setBrightness(20);  // Kecerahan 100% pada jam 02:00 - 10:00
//        flag1 = true;
//        flag2 = false;
        //Serial.println(F("malam"));
    } 
    else if(jam >= 3  or jam < 21 ){
        Disp.setBrightness(100);   // Kecerahan 50% pada jam 22:00 - 02:00
//        flag1 = false;
//        flag2 = true;
        //Serial.println(F("pagi"));
    }
    
     if (jam == 0 && menit == 0 && detik == 0 || jam == 12 && menit == 0 && detik == 0) {  // Bunyikan hanya saat 11:00:00 - 11:00:03
        Buzzer(2);
     }
}

void drawDate(){
  static unsigned int x;
  if (reset_x !=0) { x=0;reset_x = 0;}
  static int fullScroll = 0;
  //if(adzan) return;
 
  RtcDateTime now = Rtc.GetDateTime();
  static unsigned long   lsRn;
  unsigned long          Tmr = millis();
   
  int Speed = speedDate;
  byte daynow   = now.DayOfWeek();    // load day Number
  
  //char buff_date[30];

  char buff_date[100]; // Pastikan ukuran buffer cukup besar
    snprintf(buff_date, sizeof(buff_date), "%s %s %02d-%02d-%04d %02d %s %04dH",
    Hari[daynow], pasar[jumlahhari() % 5], now.Day(), now.Month(), now.Year(),
    tanggalHijriah.tanggal, namaBulanHijriah[tanggalHijriah.bulan - 1], tanggalHijriah.tahun);

  
  if (fullScroll == 0) { // Hitung hanya sekali
  fullScroll = Disp.textWidth(buff_date) + Disp.width() + 150;
  }

 if (Tmr - lsRn > Speed) { 
  lsRn = Tmr;
  fType(0); 
  Disp.drawText(Disp.width() - (x++), 25, buff_date);
  
  if (x >= fullScroll) {
    x = 0;
    show = ANIM_TEXT;
  }
}

}

void runningTextInfo1() {
  static unsigned int x = 0;
  static unsigned long lsRn;
  unsigned long Tmr = millis();
  int Speed = speedText1;

  // Simpan teks di Flash (PROGMEM)
  
  char msg_buffer[50]; // Pastikan cukup besar untuk teks
  strcpy_P(msg_buffer, msg); // Ambil teks dari Flash

  // Hitung panjang teks hanya sekali
  static int fullScroll = 0;
  if (fullScroll == 0) { 
    fullScroll = Disp.textWidth(msg_buffer) + Disp.width() + 250;
  }

  // Jalankan animasi scrolling berdasarkan millis()
  if ((Tmr - lsRn) > Speed) { 
    lsRn = Tmr;
    fType(0);
    
    int posX = Disp.width() - x;
    if (posX < -Disp.textWidth(msg_buffer)) { // Cegah teks keluar layar
      x = 0;
      show = ANIM_INFO;
      return;
    }

    Disp.drawText(posX, 25, msg_buffer);
    x++; // Geser teks ke kiri
  }
}

void runningTextInfo2() {
  static unsigned int x = 0;
  static unsigned long lsRn;
  unsigned long Tmr = millis();
  int Speed = speedText2;

  // Simpan teks di Flash (PROGMEM)
  
  char msg_buffer[50]; // Pastikan cukup besar untuk teks
  strcpy_P(msg_buffer, msg1); // Ambil teks dari Flash

  // Hitung panjang teks hanya sekali
  static int fullScroll = 0;
  if (fullScroll == 0) { 
    fullScroll = Disp.textWidth(msg_buffer) + Disp.width() + 250;
  }

  // Jalankan animasi scrolling berdasarkan millis()
  if ((Tmr - lsRn) > Speed) { 
    lsRn = Tmr;
    fType(0);
    
    int posX = Disp.width() - x;
    if (posX < -Disp.textWidth(msg_buffer)) { // Cegah teks keluar layar
      x = 0;
      show = ANIM_JAM;
      return;
    }

    Disp.drawText(posX, 25, msg_buffer);
    x++; // Geser teks ke kiri
  }
}

void jadwalSholat(){
 
if (adzan) return;

  RtcDateTime now = Rtc.GetDateTime();
  static int x = 0;
  static byte s = 0; // 0 = masuk, 1 = keluar
  static uint32_t   lsRn;
  uint32_t          Tmr = millis(); 

  //int hours, minutes;
  char buff_jam[6]; // Format HH:MM hanya butuh 6 karakter
  char sholat[8];   // Buffer untuk menyimpan nama sholat dari PROGMEM

  // Ambil nama sholat dari Flash
  strcpy_P(sholat, jadwal[list]);
  
  int hours, minutes;
  uint16_t tahun = now.Year();
  uint8_t bulan = now.Month();
  uint8_t tanggal = now.Day();

    if((Tmr-lsRn)>55) 
      { 
        if(s==0 and x<33){lsRn=Tmr; x++; }
        if(s==1 and x>0){lsRn=Tmr;x--; }
      }

   if((Tmr-lsRn)>1500 and x ==33) {s=1;}

   if (x == 0 && s == 1) { 
    s = 0;
    list++; 
    if (list == 4) list = 5;  
    if (list == 7) list = 0;
    if(list == 0){ JadwalSholat(); }
  }

  // Ambil nama sholat dari Flash
  strcpy_P(sholat, jadwal[list]);

  get_float_time_parts(times[list], hours, minutes);

  minutes = minutes + dataIhty[list];

  if (minutes >= 60) { minutes -= 60; hours++; }

  // Format HH:MM
  sprintf(buff_jam, "%02d:%02d", hours, minutes);

  // Tampilkan teks dengan animasi
  fType(3);
  Disp.drawText(-33 + x, 17, sholat);
  Disp.drawRect(-33 + x + 29, 17, -33 + x + 29, 23, 0);

  fType(0);
  Disp.drawText(67 - x, 17, buff_jam);
  Disp.drawRect(67 - x - 1, 17, 67 - x - 1, 23, 0);
}

  
/*=============================================================================================*/

void drawAzzan()
{
    static const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
    const char *sholat = jadwal[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = 40; // config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
            fType(0);
            dwCtr(0, 8, "ADZAN");
            fType(0);
            dwCtr(0, 16, sholat);
            Buzzer(1);
        }
        else
        {
            Buzzer(0);
            Disp.clear();
        }
        ct++;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_IQOMAH;
        Disp.clear();
        ct = 0;
        Buzzer(0);
    }
}

void drawIqomah()  // Countdown Iqomah (9 menit)
{  
    static uint32_t lsRn = 0;
    static int ct = 0;  // Mulai dari 0 untuk menghindari error
    static int mnt, scd;
    char locBuff[10];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    int cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    sprintf(locBuff, "-%02d:%02d", mnt, scd);

    if ((ct & 1) == 0) {  // Gunakan bitwise untuk optimasi modulo 2
        fType(0);
        dwCtr(0, 8, "IQOMAH");
    }

    fType(0);
    dwCtr(0, 16, locBuff);

    if (now - lsRn > 1000) 
    {   
        lsRn = now;
        ct++;

        Serial.println(F("test run"));  // Gunakan F() untuk hemat RAM

        if (ct > (cn_l - 5)) {
            Buzzer(ct & 1);  // Gunakan bitwise untuk menggantikan modulo 2
        }
    }

    if (ct >= cn_l)  // Pakai >= untuk memastikan countdown selesai dengan benar
    {
        ct = 0;
        Buzzer(0);
        Disp.clear();
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static int ct = 0;
    const int ct_l = displayBlink[sholatNow] * 60;  // Durasi countdown
    uint32_t now = millis();  // Simpan millis()

    // Ambil waktu dari RTC
    RtcDateTime rtcNow = Rtc.GetDateTime();

    // Hitung countdown
    int mnt = (ct_l - ct) / 60;
    int scd = (ct_l - ct) % 60;

//    char locBuff[6];
//    sprintf(locBuff, " %d:%02d ", mnt, scd);

//    fType(2);
//    Disp.drawText(10, 8, locBuff);

    // Tampilkan jam besar
    char timeBuff[9];
    sprintf(timeBuff, "%02d:%02d:%02d", rtcNow.Hour(), rtcNow.Minute(),rtcNow.Second());
    
    fType(3);
    dwCtr(0, 16, timeBuff);

    // Update countdown setiap detik
    if (now - lsRn > 1000)
    {
        lsRn = now;
        Serial.print(F("ct:"));
        Serial.println(ct);
        ct++;
    }

    // Reset jika countdown selesai
    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        Disp.clear();
        show = ANIM_JAM;
        logo1(48);
        logo2(0);
    }
}



void logo1 (uint8_t x){
  if (adzan) return;
  static const uint8_t logo1[] PROGMEM = {
    16,16,
  0x00, 0x00, 0x0c, 0xc0, 0x0d, 0xc0, 0x19, 0xc1, 0x00, 0x03, 0x04, 0x0b, 0x4c, 0xdb, 0x9c, 0xdb, 0xbc, 0xdb, 0xfc, 0xdb, 0x6c, 0xdb, 0x0c, 0xdb, 0x0c, 0xdb, 0x0f, 0xfb, 0x07, 0x32, 0x00, 0x00

    //0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
  };
  Disp.drawBitmap(x,0,logo1);
}

void logo2 (uint8_t x){
  if (adzan) return;
  static const uint8_t logo2[] PROGMEM = {
    16,16,
    0x00, 0x00, 0x13, 0x00, 0x1b, 0x00, 0x18, 0x38, 0x08, 0x2c, 0x0c, 0x78, 0x0d, 0xf0, 0x07, 0x00, 0x07, 0xff, 0x0c, 0x7c, 0x1d, 0xe0, 0x77, 0x80, 0xe3, 0x80, 0x83, 0x80, 0x01, 0x80, 0x00, 0x00
};
  Disp.drawBitmap(x,0,logo2);
}
/*======================================================================================*/

  void fType(int x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
  //  else Disp.se
  }

  void dwCtr(int x, int y, String Msg){
   int   tw = Disp.textWidth(Msg);
    int   th = 16;
    int   c = int((DWidth-x-tw)/2);
    //Disp.drawFilledRect(x+c-1,y,x+tw+c,y+th,0);
    Disp.drawText(x+c,y,Msg);
}

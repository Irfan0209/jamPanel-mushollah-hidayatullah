/*===============================

#feature
-baris atas jam & tulisan berjalan hari nasional+hari jawa,tanggal bulan tahun hijriah dan nasional
-barisan bawah tulisan berjalan "TPQ"

Program V1.0

==================================*/
/*------------------------------------------------------------------------------------
http://192.168.2.1/setPanel?jam=12:00:00&tanggal=01-01-2024&text=Hello&speedDate=60&speedText=70&brightness=20&newPassword=newpassword123&mode=1

noEEPROM    value
0           brightness
4           speed date
8           speed text
12          mode
16          chijir
20          durasi adzan
24          ihti
28          latitude
38          longitude
48          zona waktu
52          stateBuzzer
56          password
66          text
-------------------------------------------------------------------------------------*/

/*
 * SUBMENU       |        FUNGSI        |            KODE
 * --------------|----------------------|---------------------------------------------
 * waktu         |        time          |           Tm=00:00-00:00:0000
 * --------------|----------------------|----------------------------------------------
 * lokasi        |        lokasi        |           Lk=0.0000000-000.000000000-7
 * --------------|----------------------|----------------------------------------------
 * iqomah        |        iqomah        |           Iq=subuh-1
 *               |        display off   |           Dy=subuh-1
 * --------------|----------------------|----------------------------------------------
 * koreksi       |        koreksi       |           Kr=subuh-0
 * --------------|----------------------|---------------------------------------------
 * pesan         |        pesan         |           Tx=1-"pesan"
 * --------------|----------------------|---------------------------------------------
 * display       |        brightness    |           Br=100
 *               |        speed text    |           Spd=1 
 *               |        buzzer        |           Bzr=1
 * --------------|----------------------|-----------------------------------------------
 * 
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>

#include <DMDESP.h>

#include <Wire.h>
#include <RtcDS3231.h>
#include <ESP_EEPROM.h>

#include "PrayerTimes.h"


#include <fonts/SystemFont5x7.h>
#include <fonts/Font4x6.h>
#include <fonts/System4x7.h>
#include <fonts/SmallCap4x6.h>
#include <fonts/EMSans6x16.h>
//#include <fonts/Font4x6.h>

//SETUP DMD
#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 2

#define BUZZ  D4 // PIN BUZZER
//#define LED   2 // LED Internal

#define Font0 SystemFont5x7
#define Font1 Font4x6
#define Font2 System4x7 
#define Font3 SmallCap4x6
#define Font4 EMSans6x16
//////////hijriyah
#define epochHijriah          1948439.5f //math.harvard.edu
#define tambahKurangHijriah   0

// Ukuran EEPROM (pastikan cukup untuk semua data)
#define EEPROM_SIZE 200
//#include "icon.h"

//create object
RtcDS3231<TwoWire> Rtc(Wire);
DMDESP  Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)
RtcDateTime now;
ESP8266WebServer server(80);
double times[sizeof(TimeName)/sizeof(char*)];

int maxday[]            = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int ihtiSholat[]        = {0,0,0,0,0};
uint8_t iqomah[] = {1,1,1,1,1,1,1};
uint8_t displayBlink[] = {1,1,1,1,1,1,1};
uint8_t dataIhty[] = {0,0,0,0,0,0,0};

// Durasi waktu iqomah
struct Config {
  int chijir;
  int durasiadzan;
  int ihti; // Koreksi Waktu Menit Jadwal Sholat
  float latitude = -7.364057;
  float longitude = 112.646222;
  int zonawaktu = 7;
};


struct TanggalDanWaktu
{
  uint8_t detik;
  uint8_t menit;
  uint8_t jam;
  uint8_t hari;
  uint8_t tanggal;
  uint8_t bulan;
  uint8_t tahun;
};

struct Tanggal
{
  uint8_t tanggal;
  uint8_t bulan;
  uint16_t tahun;
};

struct TanggalJawa
{
  uint8_t pasaran;
  uint8_t wuku;
  uint8_t tanggal;
  uint8_t bulan;
  uint16_t tahun;
};

struct JamDanMenit
{
  uint8_t jam;
  uint8_t menit;
};

TanggalDanWaktu tanggalMasehi;
Tanggal tanggalHijriah;
TanggalJawa tanggalJawa;
JamDanMenit waktuMagrib;
Config config;

// Pengaturan hotspot WiFi dari ESP8266
 char ssid[20]     = "JAM_PANEL_MUSHOLLAH";
 char password[20] = "00000000";

// Variabel untuk waktu, tanggal, teks berjalan, tampilan ,dan kecerahan
String setJam        = "00:00:00";
String setTanggal    = "01-01-2024";
String setText       = "Selamat Datang!";
String info1[150];
String info2[150];
int    brightness    = 50;
char   text[200] ;
bool   adzan         = 0;
bool   stateBuzzer;
int    trigJam       = 17;
int    trigMenit     = 30;
int    DWidth        = Disp.width();
int    DHeight       = Disp.height();
byte   sholatNow     = -1;
byte   reset_x       = 0; 

/*======library tambahan=======*/
//byte   tampilan      = 1;
byte   mode          = 1;
byte   list          = 0; 
//bool   flag1         = 1;//variabel untuk menyimpan status animasi running text  
int    speedDate     = 70; // Kecepatan default date
int    speedText     = 60; // Kecepatan default text  
float dataFloat[10];
int   dataInteger[10];
int indexText;
/*==============================*/

enum Show{
  ANIM_ZONK,
  ANIM_JAM,
  ANIM_TEXT,
  ANIM_SHOLAT,
  ANIM_ADZAN,
  ANIM_IQOMAH,
  ANIM_BLINK
};

Show show = ANIM_JAM;

IPAddress local_IP(192, 168, 2, 1);      // IP Address untuk AP
IPAddress gateway(192, 168, 2, 1);       // Gateway
IPAddress subnet(255, 255, 255, 0);      // Subnet mask

//----------------------------------------------------------------------
// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void ICACHE_RAM_ATTR refresh() { 

  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);  

}

void Disp_init() {

  Disp.start();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(refresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  Disp.clear();
  Disp.setBrightness(brightness);
  Serial.println("Setup dmd selesai");

}

void AP_init(){
  // Konfigurasi hotspot WiFi dari ESP8266
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Atur server untuk menerima permintaan set waktu, tanggal, teks, dan kecerahan
  server.on("/setPanel", handleSetTime);
  server.begin();
  Serial.println("Server dimulai.");  
}
  
// Fungsi untuk mengatur jam, tanggal, running text, dan kecerahan
void handleSetTime(){
  Serial.println("hansle run");
  static int flag = 0;
  Buzzer(1);
  
  if (server.hasArg("status")) {//status
    server.send(200, "text/plain", "CONNECTED");
  }
  
  if (server.hasArg("Tm")) {//jam
    setJam = server.arg("Tm"); 
    //Serial.println("setJam:"+String(setJam));
    RtcDateTime now = Rtc.GetDateTime();
    
    // Memisahkan jam dan menit
    int separatorIndex = setJam.indexOf(':');  // Mencari posisi ":"
    int dashIndex = setJam.indexOf('-');       // Mencari posisi "-"

    int jam = setJam.substring(0, separatorIndex).toInt();
    int menit = setJam.substring(separatorIndex + 1, dashIndex).toInt();

    // Memisahkan tanggal, bulan, dan tahun
    int firstColonIndex = dashIndex + 1;
    int secondColonIndex = setJam.indexOf(':', firstColonIndex);
    int lastColonIndex = setJam.indexOf(':', secondColonIndex + 1);

    int tanggal = setJam.substring(firstColonIndex, secondColonIndex).toInt();
    int bulan = setJam.substring(secondColonIndex + 1, lastColonIndex).toInt();
    int tahun = setJam.substring(lastColonIndex + 1).toInt();
    
    Rtc.SetDateTime(RtcDateTime(tanggal, bulan, tahun, jam, menit, now.Second()));
    // Menampilkan hasil di Serial Monitor
//    Serial.print("Jam   : "); Serial.println(jam);
//    Serial.print("Menit : "); Serial.println(menit);
//    Serial.print("Tanggal : "); Serial.println(tanggal);
//    Serial.print("Bulan   : "); Serial.println(bulan);
//    Serial.print("Tahun   : "); Serial.println(tahun);
    flag=1;
    server.send(200, "text/plain", "Settingan jam berhasil diupdate");
  } 

  if (server.hasArg("text")) {
    setText = server.arg("text"); 

    // Mencari posisi tanda "-"
    int separatorIndex = setText.indexOf('-');

    // Memisahkan angka (parameter tetap)
    indexText = setText.substring(0, separatorIndex).toInt();

    // Memisahkan teks dinamis
    String dynamicText = setText.substring(separatorIndex + 1);

    dynamicText.toCharArray(text,dynamicText.length()+1);
    
    Serial.println(String()+"setText:"+setText);
    Serial.println(String()+"text   :"+text);
    
    //saveStringToEEPROM(66, dynamicText);
    server.send(200, "text/plain", "Settingan nama berhasil diupdate");
  }
  
  if (server.hasArg("Br")) {
    int input  = server.arg("Br").toInt(); 
    brightness = map(input,0,100,10,255);
   
    //Serial.println(String()+"brightness:"+brightness);
    Disp.setBrightness(brightness);
    //EEPROM.put(0, brightness);
    server.send(200, "text/plain", "Kecerahan berhasil diupdate");
  }
  
  if (server.hasArg("Sptx")) {
    int input = server.arg("Sptx").toInt(); // Atur kecepatan text
    speedText =  map(input,0,100,10,80);
  //  Serial.println(String()+"speedText:"+speedText);
    //EEPROM.put(8, speedText);
    server.send(200, "text/plain", "Kecepatan nama berhasil diupdate");
  }

  if (server.hasArg("Spdt")) {
    int input = server.arg("Spdt").toInt(); // Atur kecepatan text
    speedDate =  map(input,0,100,10,80);
  //  Serial.println(String()+"speedDate:"+speedDate);
    //EEPROM.put(8, speedDate);
    server.send(200, "text/plain", "Kecepatan tanggal berhasil diupdate");
  }

  if (server.hasArg("Lk")) {
    String data = server.arg("Lk"); 
    //Serial.print(String()+"data:"+data);
    parsingData(data);
    flag=1;
  }

  if (server.hasArg("Iq")) { //iqomah
    String data = server.arg("Iq"); 
     // Mencari posisi tanda "-"
    int separatorIndex = data.indexOf('-');

    // Memisahkan angka pertama
    int indexSholat = data.substring(0, separatorIndex).toInt();

    // Memisahkan angka kedua
    int indexKoreksi = data.substring(separatorIndex + 1).toInt();  
    iqomah[indexSholat]=indexKoreksi;
    Serial.println(String()+"iqomah:"+iqomah[indexSholat]=indexKoreksi);
  }           

  if (server.hasArg("Dy")) { //display off
    String data = server.arg("Dy"); 
     // Mencari posisi tanda "-"
    int separatorIndex = data.indexOf('-');

    // Memisahkan angka pertama
    int indexSholat = data.substring(0, separatorIndex).toInt();

    // Memisahkan angka kedua
    int indexKoreksi = data.substring(separatorIndex + 1).toInt();  
    displayBlink[indexSholat]=indexKoreksi;
    Serial.println(String()+"dislayoff:"+displayBlink[indexSholat]=indexKoreksi);
  }        

  if (server.hasArg("Kr")) { //koreksi jadwal
    String data = server.arg("Kr"); 
     // Mencari posisi tanda "-"
    int separatorIndex = data.indexOf('-');

    // Memisahkan angka pertama
    int indexSholat = data.substring(0, separatorIndex).toInt();

    // Memisahkan angka kedua
    int indexKoreksi = data.substring(separatorIndex + 1).toInt();  
    dataIhty[indexSholat]=indexKoreksi;
    Serial.println(String()+"koreksijadwal:"+dataIhty[indexSholat]=indexKoreksi);
  }        

  
  
  if (server.hasArg("Bzr")) {
    stateBuzzer = server.arg("Bzr").toInt(); // Atur status buzzer
    //Serial.println(String()+"stateBuzzer:"+stateBuzzer);
    //EEPROM.put(52, stateBuzzer);
    server.send(200, "text/plain", (stateBuzzer)?"Suara Diaktifkan":"Suara Dimatikan");
  }
  if (server.hasArg("status")) {
    server.send(200, "text/plain", "CONNECTED");
  }
/////////////////
//  if (server.hasArg("newPassword")) {
//    String newPassword = server.arg("newPassword");
//    if(newPassword.length()==8){
//      Serial.println(String()+"newPassword:"+newPassword);
//      newPassword.toCharArray(password, newPassword.length() + 1); // Set password baru
//      //saveStringToEEPROM(56, password); // Simpan password AP
//      server.send(200, "text/plain", "Password WiFi diupdate");
//    }else{ Buzzer(2); Serial.println("panjang password melebihi 8 karakter"); }
  //} 
   // write the data to EEPROM
  //boolean ok1 = EEPROM.commit();
  //Serial.println((ok1) ? "First commit OK" : "Commit failed");
  if(flag){JadwalSholat(); flag=0;}
  //delay(50);
  (stateBuzzer==1)?Buzzer(0) : digitalWrite(BUZZ,HIGH);
  
  //server.send(200, "text/plain", "Pengaturan berhasil diupdate dan disimpan ke EEPROM!");
}

// Fungsi untuk menyimpan nilai float ke EEPROM di alamat yang ditentukan
void tulisFloatEEPROM(int address, float nilai) {
  byte* p = (byte*)(void*)&nilai;
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM.write(address + i, *(p + i));
  }
  EEPROM.commit();  // Simpan perubahan ke EEPROM
}

// Fungsi untuk membaca nilai float dari EEPROM di alamat yang ditentukan
float bacaFloatEEPROM(int address) {
  float nilai = 0.0;
  byte* p = (byte*)(void*)&nilai;
  for (int i = 0; i < sizeof(float); i++) {
    *(p + i) = EEPROM.read(address + i);
  }
  return nilai;
}

// Fungsi untuk menyimpan string ke EEPROM
void saveStringToEEPROM(int startAddr, const String &data) {
  int len = data.length();
  for (int i = 0; i < len; i++) {
    EEPROM.put(startAddr + i, data[i]);
  }
  EEPROM.put(startAddr + len, '\0'); // Null terminator
}

// Fungsi untuk membaca string dari EEPROM
String readStringFromEEPROM(int startAddr) {
  char data[100]; // Buffer untuk string yang akan dibaca
  int len = 0;
  unsigned char k;
  k = EEPROM.read(startAddr);
  while (k != '\0' && len < 100) { // Membaca hingga null terminator
    data[len] = k;
    len++;
    k = EEPROM.read(startAddr + len);
  }
  data[len] = '\0';
  return String(data);
}

// Membaca semua data dari EEPROM
void loadFromEEPROM() {

  setText   = readStringFromEEPROM(66);
  brightness= EEPROM.read(0);
  speedDate = EEPROM.read(4);
  speedText = EEPROM.read(8);
  mode      = EEPROM.read(12);
  config.chijir      = EEPROM.read(16);
  config.durasiadzan = EEPROM.read(20);
  config.ihti        = EEPROM.read(24);
  config.latitude    = bacaFloatEEPROM(28);
  config.longitude   = bacaFloatEEPROM(38);
  config.zonawaktu   = EEPROM.read(48);
  stateBuzzer        = EEPROM.read(52);
  String loadedPassword = readStringFromEEPROM(56); // Baca password dari EEPROM
  //String loadedPassword="00000000";
  loadedPassword.toCharArray(password, loadedPassword.length() + 1); // Set password AP
  setText.toCharArray(text,setText.length()+1);
  (mode==1)? show = ANIM_JAM:show = ANIM_ZONK;
  
  Serial.println("");
  Serial.println(String()+"setText       :"+setText);
  Serial.println(String()+"brightness    :"+brightness);
  Serial.println(String()+"speedDate     :"+speedDate);
  Serial.println(String()+"speedText     :"+speedText);
  Serial.println(String()+"mode          :"+mode);
  Serial.println(String()+"zona waktu    :"+config.zonawaktu);
  Serial.println(String()+"chijir        :"+config.chijir);
  Serial.println(String()+"durasi adzan  :"+config.durasiadzan);
  Serial.println(String()+"ihti          :"+config.ihti);
  Serial.println(String()+"latitude      :"+config.latitude);
  Serial.println(String()+"longitude     :"+config.longitude);
  Serial.println(String()+"loadedPassword:"+loadedPassword);
  Serial.println(String()+"stateBuzzer   :"+stateBuzzer);
}
// now = Rtc.GetDateTime();
void setup() {
  Serial.begin(115200);
  pinMode(BUZZ, OUTPUT); 
  //pinMode(LED, OUTPUT);
  EEPROM.begin(EEPROM_SIZE); // Inisialisasi EEPROM dengan ukuran yang ditentukan
  digitalWrite(BUZZ,LOW);
  delay(100);
  digitalWrite(BUZZ,HIGH);
  // Load data dari EEPROM
  //loadFromEEPROM();

  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
    if (rtn != 0) {
      Serial.println(F("I2C bus error. Could not clear"));
      if (rtn == 1) {
        Serial.println(F("SCL clock line held low"));
      } else if (rtn == 2) {
        Serial.println(F("SCL clock line held low by slave clock stretch"));
      } else if (rtn == 3) {
        Serial.println(F("SDA data line held low"));
      }
    } 
    else { // bus clear, re-enable Wire, now can start Wire Arduino master
      Wire.begin();
    }
  
  Rtc.Begin();
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
  for(int i = 0; i < 4; i++){
      Buzzer(1);
      delay(80);
      Buzzer(0);
      delay(80);
  }
  AP_init();   //Inisialisasi Access Pointt
  Disp_init(); //Inisialisasi display
  
  //JadwalSholat();
}

void loop() {

  //server.handleClient(); // Menangani permintaan dari MIT App Inventor
  check();
  islam();
  //checkConnectedDevices();
  
  switch(show){
    case ANIM_JAM :
       drawTime();
       logo1(47);
       logo2(0);
       drawDate();
      
    break;
    case ANIM_TEXT :
       drawTime();
       logo1(47);
       logo2(0);
       runningText();
       //jadwalSholat();
    break;
    case ANIM_ADZAN :
       drawAzzan();
    break;
    case ANIM_IQOMAH :
       drawIqomah();
    break;
    case ANIM_BLINK :
       blinkBlock();
    break;
  };
   jadwalSholat();
  // Serial.println(String()+"adzan          :"+adzan);
  // Serial.println(String()+"reset_x        :"+reset_x);
}

// Cek jumlah perangkat yang terhubung ke AP
void checkConnectedDevices() {
  int connectedDevices = WiFi.softAPgetStationNum(); // Mendapatkan jumlah perangkat terhubung
//  Serial.print("Perangkat terhubung: ");
//  Serial.println(connectedDevices);
 fType(0);
  if (connectedDevices > 0) {
    Disp.drawText(0, 0, "HP Tersambung!");
  } else {
    Disp.drawText(0, 0, "Menunggu koneksi...");
  }
}

void logo1 (uint8_t x){
  static const uint8_t logo1[] PROGMEM = {
    16,16,
    0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
  };
  Disp.drawBitmap(x,0,logo1);
}

void logo2 (uint8_t x){
  static const uint8_t logo2[] PROGMEM = {
    16,16,
    0x00, 0x00, 0x13, 0x00, 0x1b, 0x00, 0x18, 0x38, 0x08, 0x2c, 0x0c, 0x78, 0x0d, 0xf0, 0x07, 0x00, 0x07, 0xff, 0x0c, 0x7c, 0x1d, 0xe0, 0x77, 0x80, 0xe3, 0x80, 0x83, 0x80, 0x01, 0x80, 0x00, 0x00
};
  Disp.drawBitmap(x,0,logo2);
}

void Buzzer(uint8_t state)
  {
    //if(!stateBuzzer) return;
    switch(state){
      case 0 :
        digitalWrite(BUZZ,HIGH);
      break;
      case 1 :
        digitalWrite(BUZZ,LOW);
      break;
      case 2 :
        for(int i = 0; i < 5; i++){ digitalWrite(BUZZ,LOW); delay(80); digitalWrite(BUZZ,HIGH); delay(80); }
      break;
    };
  }



// PARAMETER PENGHITUNGAN JADWAL SHOLAT

void JadwalSholat() {
  
  RtcDateTime now = Rtc.GetDateTime();

  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();

  Serial.println("calcualat run");
  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);

  get_prayer_times(tahun, bulan, tanggal, config.latitude, config.longitude, config.zonawaktu, times);

}

 //----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}

void parsingData(String data){
  // Data string
  //String data = "0.1234-111.2345-7";
  
  char charData[20]; 
  data.toCharArray(charData, sizeof(charData));

  // Buffer untuk strtok_r()
  char *token;
  char *savePtr;

  // Array penyimpanan angka
  float angkaFloat[10];
  int angkaInt[10];
  int indexFloat = 0, indexInt = 0;

  // Mulai parsing pertama
  token = strtok_r(charData, "-", &savePtr);

  while (token != NULL) { // Menggunakan while karena hanya dijalankan sekali
    Serial.print("Nilai ditemukan: ");
    Serial.println(token);

    // Cek apakah token mengandung titik (float) atau tidak (int)
    if (strchr(token, '.') != NULL) {
      dataFloat[indexFloat] = atof(token);
//      Serial.print("Disimpan sebagai float: ");
//      Serial.println(dataFloat[indexFloat], 5);
      config.latitude = dataFloat[0];
      config.longitude = dataFloat[1];
      indexFloat++;
    } else {
      dataInteger[indexInt] = atoi(token);
//      Serial.print("Disimpan sebagai int: ");
//      Serial.println(dataInteger[indexInt]);
      config.zonawaktu = dataInteger[0];
      indexInt++;
    }

    // Ambil nilai berikutnya
    token = strtok_r(NULL, "-", &savePtr);
  }

  //Serial.println("\nParsing selesai di proses()");
}

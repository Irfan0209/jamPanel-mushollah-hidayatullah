//////hijiriyah voidku/////////////////////////////////////////////////
void islam() {
  //
  waktuMagrib = {trigJam, trigMenit};
 
  // Serial.println("trigJam  :" + String(trigJam));
  // Serial.println("trigMenit:" + String(trigMenit));

  RtcDateTime now = Rtc.GetDateTime();
  tanggalMasehi.tanggal = now.Day();
  tanggalMasehi.bulan = now.Month();
  tanggalMasehi.tahun = now.Year() - 2000;

  uint32_t jumlahHari;
  double jumlahHariMatahari;

  tanggalMasehi.hari = hariDariTanggal(tanggalMasehi);
  masehiKeHijriah(tanggalMasehi, waktuMagrib, tambahKurangHijriah,
                  tanggalHijriah, tanggalJawa);
  printKonversi(tanggalMasehi, tanggalHijriah, tanggalJawa);

  //while (1);
}

double get_julian_date(Tanggal tanggal)
{
  if (tanggal.bulan <= 2)
  {
    tanggal.tahun -= 1;
    tanggal.bulan += 12;
  }

  double a = floor(tanggal.tahun / 100.0);
  double b = 2 - a + floor(a / 4.0);

  if (tanggal.tahun < 1583)
    b = 0;
  if (tanggal.tahun == 1582) {
    if (tanggal.bulan > 10)
      b = -10;
    if (tanggal.bulan == 10) {
      b = 0;
      if (tanggal.tanggal > 4)
        b = -10;
    }
  }

  return floor(365.25 * (tanggal.tahun + 4716)) + floor(30.6001 *
         (tanggal.bulan + 1)) + tanggal.tanggal + b - 1524.5;
}

double konversiTanggalHijriahKeJulianDate(uint16_t tahun, uint8_t
    bulan, uint8_t tanggal)
{
  return (epochHijriah + tanggal + ceil(29.5 * (bulan - 1)) +
          (354L * (tahun - 1)) + floor((3 + (11 * tahun)) / 30)) - 1;
}

void masehiKeHijriah(TanggalDanWaktu masehi, JamDanMenit
                     waktuSholatMagrib, int8_t koreksiHijriah, Tanggal & hijriah,
                     TanggalJawa & jawa)
{
  uint16_t sisaHari;
  double julianDate = get_julian_date({masehi.tanggal,
                                       masehi.bulan, masehi.tahun + 2000
                                      });

  uint16_t menitMagrib = waktuSholatMagrib.jam * 60  +
                         waktuSholatMagrib.menit;
  uint16_t menitSekarang = masehi.jam * 60 + masehi.menit;

  if (menitSekarang >= menitMagrib)
  {
    julianDate++;//Pergantian hari hijrah pada magrib
  }

  julianDate = floor(julianDate) + 0.5;

  Tanggal tanggalHijriah;

  hijriah.tahun = floor(((30 * (julianDate - epochHijriah)) +
                         10646) / 10631);
  hijriah.bulan = min(12.0, ceil((julianDate - (29 +
                                  konversiTanggalHijriahKeJulianDate(hijriah.tahun, 1, 1))) / 29.5)
                      + 1);
  hijriah.tanggal = (julianDate -
                     konversiTanggalHijriahKeJulianDate(hijriah.tahun, hijriah.bulan,
                         1)) + 1+config.chijir;

  long julianLong = (long)julianDate;
  jawa.pasaran = ((julianLong + 1) % 5); //0 = legi
  jawa.wuku = (((julianLong + 65) % 210) / 7); //0 = Shinta
  jawa.tanggal = hijriah.tanggal;
  jawa.bulan = hijriah.bulan;
  jawa.tahun = hijriah.tahun + 512;

}
uint8_t hariDariTanggal(TanggalDanWaktu tanggalDanWaktu) {
  uint16_t jumlahHari = tanggalDanWaktu.tanggal;
  for (uint8_t i = 1; i < tanggalDanWaktu.bulan; ++i)
    jumlahHari += maxday[i - 1];
  if (tanggalDanWaktu.bulan > 2 && tanggalDanWaktu.tahun % 4 == 0)
    ++jumlahHari;
  jumlahHari += (365 * tanggalDanWaktu.tahun) +
                ((tanggalDanWaktu.tahun + 3) / 4) - 1;

  return ((jumlahHari + 6) % 7) + 1; // 1 Januari 2000 hari sabtu= 7

}
void printKonversi(TanggalDanWaktu tanggalMasehi, Tanggal
                   tanggalHijriah, TanggalJawa tanggalJawa)
{



  ///end hijir voidku///////////////////////////////////////////////////
}

// digunakan untuk menghitung hari pasaran
  int jumlahhari(){ 
  RtcDateTime now = Rtc.GetDateTime();
  int d= now.Day();
  int m= now.Month();
  int y= now.Year();
  int hb[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
  int ht = (y - 1970) * 365 - 1;
  int hs = hb[m - 1] + d;
  int kab = 0;
  int i;
  if(y % 4 == 0) {
    if(m > 2) {
    hs++;
    }
  }
  for(i = 1970; i < y; i++) {
    if(i % 4 == 0) {
    kab++;
    }
  }
  return (ht + hs + kab); 
}

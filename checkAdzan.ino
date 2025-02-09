/*void check(){
  RtcDateTime now = Rtc.GetDateTime();
  uint8_t Hari = now.DayOfWeek();
  uint8_t jam = now.Hour();
  uint8_t menit = now.Minute();
  uint8_t detik = now.Second();
  uint8_t hours, minutes;
  static uint8_t counter=0;
  static uint32_t lsTmr;
  uint32_t tmr = millis();
 
  if(tmr - lsTmr > 100){
    lsTmr = tmr;
    
    get_float_time_parts(times[counter], hours, minutes);
    minutes = minutes + config.ihti;
    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }

    if(counter!=1 and counter!=4 ){
      if(jam == hours && menit == minutes && detik < 1){
      Disp.clear();
      sholatNow = counter;
      adzan = 1;
      reset_x = 1;
      show=ANIM_ADZAN;
      list    = 0;
      
      }
    }

    if(counter == 5){
      if(hours != trigJam || minutes != trigMenit){ trigJam  = hours; trigMenit=minutes;
      Serial.println("trigJam  :" + String(trigJam));
      Serial.println("trigMenit:" + String(trigMenit));} 
    }
  
    counter++;
    if(counter==7){counter=0;}
    
  }
  //Serial.println("sholatNow:" + String(sholatNow));
  // Serial.println("trigJam  :" + String(trigJam));
  // Serial.println("trigMenit:" + String(trigMenit));
}*/

void check() {
    RtcDateTime now = Rtc.GetDateTime();
    uint8_t jam = now.Hour();
    uint8_t menit = now.Minute();
    uint8_t detik = now.Second();
    int hours, minutes;
    static uint8_t counter = 0;
    static uint32_t lsTmr;
    static bool adzanFlag[7] = {false, false, false, false, false, false, false};
    uint32_t tmr = millis();

    if (tmr - lsTmr > 100) {
        lsTmr = tmr;
        yield();  // Mencegah reset

        get_float_time_parts(times[counter], hours, minutes);
        minutes += dataIhty[counter];
        if (minutes >= 60) {
            minutes -= 60;
            hours++;
        }

        if (counter != 1 && counter != 4 && !adzanFlag[counter]) {
            if (jam == hours && menit == minutes && detik == 0) {
                Disp.clear();
                sholatNow = counter;
                adzan = 1;
                reset_x = 1;
                show = ANIM_ADZAN;
                list = 0;
                adzanFlag[counter] = true;
            }
        }

        if (jam != hours || menit != minutes) {
            adzanFlag[counter] = false;
        }

        if (counter == 5 && (hours != trigJam || minutes != trigMenit)) {
            trigJam = hours;
            trigMenit = minutes;
            Serial.println("trigJam  :" + String(trigJam));
            Serial.println("trigMenit:" + String(trigMenit));
        }
        

        counter = (counter + 1) % 7;
    }
}

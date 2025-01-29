void check(){
  RtcDateTime now = Rtc.GetDateTime();
  int Hari = now.DayOfWeek();
  int jam = now.Hour();
  int menit = now.Minute();
  int detik = now.Second();
  int hours, minutes;
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
}


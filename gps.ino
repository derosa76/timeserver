
#include <TinyGPS.h>

TinyGPS gps;

unsigned long gps_show_last_time=0;

static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

void gps_setup(){
  // Serial.print("TinyGPS library v. "); Serial.println(TinyGPS::library_version());
   Serial.println(" by Mikal Hart"); //https://github.com/mikalhart/TinyGPS
  // Serial.println("Sats HDOP Latitude  Longitude  Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum");
  // Serial.println("          (deg)     (deg)      Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail");
  // Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");
}

void gps_run(unsigned long dt){
  
  // DEBUG MIGLIORATO - controlla la validità dei dati
  static unsigned long last_debug_report = 0;
  static int bytes_count = 0;
  static char last_bytes[100] = {0};  // buffer per vedere gli ultimi byte
  static int last_bytes_idx = 0;
  
  while (Serial2.available()) {
    char c = Serial2.read();
    
    // Salva gli ultimi byte per debug
    last_bytes[last_bytes_idx] = c;
    last_bytes_idx = (last_bytes_idx + 1) % 100;
    
    // Prova a decodificare
    gps.encode(c);
    bytes_count++;
  }
  
  // Ogni 10 secondi logga statistiche complete
  if (millis() - last_debug_report > 10000) {
    // Ottieni statistiche da TinyGPS - TIPI CORRETTI!
    unsigned long chars_processed;
    unsigned short good_sentences, failed_checksum;
    gps.stats(&chars_processed, &good_sentences, &failed_checksum);
    
    otalog("GPS: " + String(bytes_count) + " bytes, " + 
           String(good_sentences) + " sentence OK, " + 
           String(failed_checksum) + " checksum FAIL");
    
    // sample degli ultimi byte ricevuti
    String ultimi_bytes="GPS ultimi bytes: ";
    for(int i = 0; i < 100; i++) {
      int idx = (last_bytes_idx + i) % 100;
      if (last_bytes[idx] >= 32 && last_bytes[idx] <= 126) {
        ultimi_bytes+=last_bytes[idx];
      } else {
        ultimi_bytes+="[0x" + String(last_bytes[idx], HEX) + "]";
      }
    }
    otalog(ultimi_bytes);
    
    // Verifica se abbiamo satelliti
    int sats = gps.satellites();
    if (sats != TinyGPS::GPS_INVALID_SATELLITES) {
      otalog("GPS: " + String(sats) + " satelliti");
    } else {
      otalog("GPS: NESSUN satellite visibile!");
    }
    
    // Verifica fix
    unsigned long fix_age;
    int year;
    byte month, day, hour, minute, second, hundredths;
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
    if (fix_age == TinyGPS::GPS_INVALID_AGE) {
      otalog("GPS: NO FIX - nessun dato temporale valido");
    } else {
      otalog("GPS: FIX OK - age=" + String(fix_age) + "ms");
    }
    bytes_count = 0;
    last_debug_report = millis();
  }
  // FINE BLOCCO DEBUG
  
  if(millis()>=gps_show_last_time+dt){
    gps_show_last_time=millis();
    set_date_time();
    if (gpsTimeIsReliable) gps_offset_italia = time_offset_italia(time_object_gps);
    Serial.println(gpsTimeStatus());
  }
}

// void set_date_time()
// {
//   int year;
//   byte month, day, hour, minute, second, hundredths;
//   unsigned long age;
//   gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
//   if (age == TinyGPS::GPS_INVALID_AGE){
//     gpsTimeIsSet=false;
//     gpsTimeIsReliable=false;
//   }
//   else
//   {
//     if (age<=1500){

//       // *** DEBUG: stampa i valori RAW dal GPS ***
//       otalog("GPS raw: Y=%d M=%d D=%d H=%d m=%d s=%d hundredths=%d\n", 
//                     year, month, day, hour, minute, second, hundredths);
      
//       // PRIMA di setTime, stampa l'epoch attuale
//       unsigned long epoch_before = time_object_gps.getEpoch();
//       otalog("GPS epoch BEFORE setTime: %lu\n", epoch_before);

//       // Conversione hundredths (centesimi) in microsecondi
//       int microseconds = hundredths * 10000;  // 1 centesimo = 10.000 microsecondi
//       // Passaggio anche dei microsecondi
//       time_object_gps.setTime(second, minute, hour, day, month, year, microseconds); //aggiorno l'oggetto ESP32Time solo se il dato ha una age piccola (quindi è fresco!)

//     // DOPO setTime, stampa l'epoch risultante
//       unsigned long epoch_after = time_object_gps.getEpoch();
//       otalog("GPS epoch AFTER setTime: %lu\n", epoch_after);

//       gpsTimeLastSetMillis=millis();
//       gpsTimeIsSet=true;
//       gpsTimeIsReliable=true;
//       ntp_server_mark_gps_sync();
//     }
//     if (millis()-gpsTimeLastSetMillis>2*60*60*1000) gpsTimeIsReliable=false; //se il dato non è fresco, non aggiorno l'oggetto ESP32Time, e se il dato è più vecchio diventa NON RELIABLE 
//     //perchè la frequenza del ll'oscillatore ha accuratezza dello 0.5% e quindi può perdere 18 secondi all'ora
//     //in realtà da un test fatto da me in data 10/01/2024 dopo 13 minuti abbiamo ancora allineamento perfetto all'NTP time... per cui lo tengo buono per 2 ore (non si sa mai)
//   }
// }

void set_date_time()
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE){
    gpsTimeIsSet=false;
    gpsTimeIsReliable=false;
  }
  else
  {
    if (age<=1500){

      // *** DEBUG: stampa i valori RAW dal GPS ***
      otalog("GPS raw: Y=" + String(year) + " M=" + String(month) + " D=" + String(day) + 
             " H=" + String(hour) + " m=" + String(minute) + " s=" + String(second) + 
             " hundredths=" + String(hundredths));
      
      // PRIMA di setTime, stampa l'epoch attuale
      unsigned long epoch_before = time_object_gps.getEpoch();
      otalog("GPS epoch BEFORE setTime: " + String(epoch_before));

      // Conversione hundredths (centesimi) in microsecondi
      int microseconds = hundredths * 10000;  // 1 centesimo = 10.000 microsecondi
      // Passaggio anche dei microsecondi
      time_object_gps.setTime(second, minute, hour, day, month, year, microseconds); //aggiorno l'oggetto ESP32Time solo se il dato ha una age piccola (quindi è fresco!)

      // DOPO setTime, stampa l'epoch risultante
      unsigned long epoch_after = time_object_gps.getEpoch();
      otalog("GPS epoch AFTER setTime: " + String(epoch_after));

      gpsTimeLastSetMillis=millis();
      gpsTimeIsSet=true;
      gpsTimeIsReliable=true;
      ntp_server_mark_gps_sync();
    }
    if (millis()-gpsTimeLastSetMillis>2*60*60*1000) gpsTimeIsReliable=false;
  }
}

String gpsTimeStatus(){
  if (!gpsTimeIsSet) return "GPS:N/A";
  String res="GPS:";
  if (gpsTimeIsSet) res="GPS:"+String(time_object_gps.getEpoch())+"(+"+String(gps_offset_italia)+") - "+time_object_gps.getTime("%A, %B %d %Y %H:%M:%S");
  if (gpsTimeIsReliable) res=res+" (RELIABLE! Updated "+String((millis()-gpsTimeLastSetMillis)*1.0/60000,2)+" minutes ago)";
  if (!gpsTimeIsReliable) res=res+" (UN-RELIABLE :o\ Updated "+String((millis()-gpsTimeLastSetMillis)*1.0/60000,2)+" minutes ago)";
  return res;
}





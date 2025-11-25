//https://www.esp32.com/viewtopic.php?t=9600





//#include <SoftwareSerial.h>
#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

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
  while (Serial2.available()) gps.encode(Serial2.read());
  if(millis()>=gps_show_last_time+dt){
    gps_show_last_time=millis();
    set_date_time();

    if (gpsTimeIsReliable){
      if (time_object_gps.getYear()>gps_year){
        Serial.print("GPS: gps_year è cambiato da "+String(gps_year)+" a ");
        gps_year=time_object_gps.getYear();
        Serial.println(String(gps_year));
        gps_inizio_ora_legale=inizio_ora_legale(gps_year);
        gps_fine_ora_legale=fine_ora_legale(gps_year);
        set_date_time();//questa è necessario aggiungerla perchè per qualche ragione se no l'ora fine ora legale viene travasata in time_object_gps.getEpoch()
        Serial.println("GPS: nuovi epoch time GMT di inizio e fine ora legale Italia "+String(gps_year)+": "+String(gps_inizio_ora_legale)+" - "+String(gps_fine_ora_legale));
      }
      gps_siamo_in_ora_legale = ((time_object_gps.getEpoch()>=gps_inizio_ora_legale) && (time_object_gps.getEpoch()<gps_fine_ora_legale)) ? true : false; //ternary operator https://www.w3schools.com/cpp/cpp_conditions_shorthand.asp
      gps_offset_italia = gps_siamo_in_ora_legale ? 7200 : 3600; //ternary operator
    }

    Serial.println(gpsTimeStatus());

    //qui per ottenere coordinate ecc..ecc....
    // float flat, flon;
    // unsigned long age, date, time, chars = 0;
    // unsigned short sentences = 0, failed = 0;
    // static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
    // print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    // print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);
    // gps.f_get_position(&flat, &flon, &age);
    // print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    // print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    // print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
    // print_date(gps);
    // print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
    // print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
    // print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
    // print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6);
    // print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9);
    // print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
    // print_str(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6);

    // gps.stats(&chars, &sentences, &failed);
    // print_int(chars, 0xFFFFFFFF, 6);
    // print_int(sentences, 0xFFFFFFFF, 10);
    // print_int(failed, 0xFFFFFFFF, 9);
    // Serial.println();

  }
}

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
      time_object_gps.setTime(second, minute, hour, day, month, year); //aggiorno l'oggetto ESP32Time solo se il dato ha una age poiccola (quindi è fresco!)
      gpsTimeLastSetMillis=millis();
      gpsTimeIsSet=true;
      gpsTimeIsReliable=true;
    }
    if (millis()-gpsTimeLastSetMillis>2*60*60*1000) gpsTimeIsReliable=false; //se il dato non è fresco, non aggiorno l'oggetto ESP32Time, e se il dato è più vecchio diventa NON RELIABLE 
    //perchè la frequenza del ll'oscillatore ha accuratezza dello 0.5% e quindi può perdere 18 secondi all'ora
    //in realtà da un test fatto da me in data 10/01/2024 dopo 13 minuti abbiamo ancora allineamento perfetto all'NTP time... per cui lo tengo buono per 2 ore (non si sa mai)
  }
}

String gpsTimeStatus(){
  if (!gpsTimeIsSet) return "GPS:N/A";
  String res="GPS:";
  if (gpsTimeIsSet) res="GPS:"+String(time_object_gps.getEpoch())+"(+"+String(gps_offset_italia)+") - "+time_object_gps.getTime("%A, %B %d %Y %H:%M:%S");
  if (gpsTimeIsReliable) res=res+" (RELIABLE! Updated "+String((millis()-gpsTimeLastSetMillis)*1.0/60000,2)+" minutes ago)";
  if (!gpsTimeIsReliable) res=res+" (UN-RELIABLE :o\ Updated "+String((millis()-gpsTimeLastSetMillis)*1.0/60000,2)+" minutes ago)";
  //res=res+"GPS: epoch time GMT di inizio e fine ora legale Italia "+String(gps_year)+": "+String(gps_inizio_ora_legale)+" - "+String(gps_fine_ora_legale);
  return res;
}






//serie di funzioni utili per coordinate ecc... nello sketch di esempio

// static void print_float(float val, float invalid, int len, int prec)
// {
//   if (val == invalid)
//   {
//     while (len-- > 1)
//       Serial.print('*');
//     Serial.print(' ');
//   }
//   else
//   {
//     Serial.print(val, prec);
//     int vi = abs((int)val);
//     int flen = prec + (val < 0.0 ? 2 : 1); // . and -
//     flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
//     for (int i=flen; i<len; ++i)
//       Serial.print(' ');
//   }
// }

// static void print_int(unsigned long val, unsigned long invalid, int len)
// {
//   char sz[32];
//   if (val == invalid)
//     strcpy(sz, "*******");
//   else
//     sprintf(sz, "%ld", val);
//   sz[len] = 0;
//   for (int i=strlen(sz); i<len; ++i)
//     sz[i] = ' ';
//   if (len > 0) 
//     sz[len-1] = ' ';
//   Serial.print(sz);
// }

// static void print_date(TinyGPS &gps)
// {
//   int year;
//   byte month, day, hour, minute, second, hundredths;
//   unsigned long age;
//   gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
//   if (age == TinyGPS::GPS_INVALID_AGE)
//     Serial.print("********** ******** ");
//   else
//   {
//     char sz[32];
//     sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
//         month, day, year, hour, minute, second);
//     Serial.print(sz);
//   }
//   print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
// }

// static void print_str(const char *str, int len)
// {
//   int slen = strlen(str);
//   for (int i=0; i<len; ++i)
//     Serial.print(i<slen ? str[i] : ' ');
// }








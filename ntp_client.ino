//NTP SERVER --> https://www.lloydm.net/Demos/GPS-NTP.html (per futura implementazione...)

#include <NTPClient.h>

//https://github.com/arduino-libraries/NTPClient
WiFiUDP ntpUDP;
NTPClient ntp_client(ntpUDP); // By default 'pool.ntp.org' is used with 60 seconds update interval and no offset


unsigned long ntp_show_last_time=0;

void ntp_cl_setup()
{
  ntp_client.begin();
}

void ntp_cl_run(unsigned long dt)
{
  ntp_client.update();//https://github.com/arduino-libraries/NTPClient/blob/master/NTPClient.h --> This should be called in the main loop of your application. By default an update from the NTP Server is only made every 60 seconds. This can be configured in the NTPClient constructor.
  if(millis()>=ntp_show_last_time+dt){
    ntp_show_last_time=millis();
    if (ntp_client.isTimeSet()) {
      time_object_ntp.setTime(ntp_client.getEpochTime());
      ntp_time_is_set=true;
      if (time_object_ntp.getYear()>ntp_year){
        Serial.print("NTP: ntp_year è cambiato da "+String(ntp_year)+" a ");
        ntp_year=time_object_ntp.getYear();
        Serial.println(String(ntp_year));
        ntp_inizio_ora_legale=inizio_ora_legale(ntp_year);
        ntp_fine_ora_legale=fine_ora_legale(ntp_year);
        time_object_ntp.setTime(ntp_client.getEpochTime());//questa è necessario aggiungerla perchè per qualche ragione se no l'ora fine ora legale viene travasata in time_object_ntp.getEpoch()
        Serial.println("NTP: nuovi epoch time GMT di inizio e fine ora legale Italia "+String(ntp_year)+": "+String(ntp_inizio_ora_legale)+" - "+String(ntp_fine_ora_legale));
      }
      ntp_siamo_in_ora_legale = ((time_object_ntp.getEpoch()>=ntp_inizio_ora_legale) && (time_object_ntp.getEpoch()<ntp_fine_ora_legale)) ? true : false; //ternary operator https://www.w3schools.com/cpp/cpp_conditions_shorthand.asp
      ntp_offset_italia = ntp_siamo_in_ora_legale ? 7200 : 3600; //ternary operator
      Serial.println("NTP:"+String(time_object_ntp.getEpoch())+"(+"+String(ntp_offset_italia)+") - "+time_object_ntp.getTime("%A, %B %d %Y %H:%M:%S"));
      //Serial.println("NTP: epoch time GMT di inizio e fine ora legale Italia "+String(ntp_year)+": "+String(ntp_inizio_ora_legale)+" - "+String(ntp_fine_ora_legale));
    }
    else {
      Serial.println("NTP:N/A");
      ntp_time_is_set=false;
    }
  }
}
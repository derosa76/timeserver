
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> // questo non si può spostare nella scheda ota.ino ... altrimenti non va



//queste devono essere definite qui ed in questo ordine, se no fanno casino (errori)
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> 
AsyncWebServer server(80);

#include <ESP32Time.h>
ESP32Time time_object_ntp(0); // GMT
ESP32Time time_object_gps(0); // GMT




bool reboot=false;

bool gpsTimeIsSet=false;
bool gpsTimeIsReliable=false;
unsigned int gpsTimeLastSetMillis;

bool ntp_time_is_set=false;

// int           gps_year=0;
//unsigned long gps_inizio_ora_legale=0;
//unsigned long gps_fine_ora_legale=0;
//bool          gps_siamo_in_ora_legale=false;
int           gps_offset_italia=0;

// int           ntp_year=0;
// unsigned long ntp_inizio_ora_legale=0;
// unsigned long ntp_fine_ora_legale=0;
// bool          ntp_siamo_in_ora_legale=false;
int           ntp_offset_italia=0;


void setup(void)
{
  // Inizializza con un epoch ragionevole prima di fare qualsiasi cosa
  time_object_gps.setTime(1735689600);  // 1 gen 2025 00:00:00 GMT
  time_object_ntp.setTime(1735689600);

  wifi_watchdog_timer_setup(60); // questa è la funzione da chiamare in setup x forza. Questa innesca la chiamata a funzione ISR (interrupt ... Routine) ogni secondo
  status_led_setup();
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); //Serial2.begin(9600); // RX=16,TX=17 on ESP32.
  wifi_manager_setup();
  ota_setup();
  dns_setup(); // questo va messo per forza dopo OTA setup, se no non funziona!!
  wserver_setup();


  ntp_cl_setup();
  gps_setup();

  ntpd_setup();
  
  //Serial.println(stringa_ora_legale(2024));//per debug

}


void loop()
{
  wifi_watchdog_timer_run(1000); //obbligatoria per azzerare il counter se c'è Wifi
  wifi_watchdog_timer_print_info(10000); // questa è la funzione opzionale, che fornisce un po' di debug se necessario
  status_led_run();
  ArduinoOTA.handle();
  gps_run(10000);
  ntp_cl_run(10000);
  ntpd_run();
  if(reboot) {delay(5000);ESP.restart();}
}
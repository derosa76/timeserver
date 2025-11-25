
//scelgo i pin secondo https://www.studiopieters.nl/esp32-pinout/

int red_led_gpio=15; //POWER ON
int blue_led_gpio=18; //GPS TIME RELIABLE
int green_led_gpio=19; // WIFI CONNECTED
int yellow_led_gpio=21; // NTP CLIENT OK CONNECTED (INTERNET IS OK)

void status_led_setup(){
  pinMode(red_led_gpio, OUTPUT); /*POWER ON */ digitalWrite(red_led_gpio, HIGH); //sempre alto
  pinMode(blue_led_gpio, OUTPUT); //GPS TIME RELIABLE
  pinMode(green_led_gpio, OUTPUT); // WIFI CONNECTED
  pinMode(yellow_led_gpio, OUTPUT); // NTP CLIENT OK CONNECTED (INTERNET IS OK)
}


void status_led_run(){
  if (gpsTimeIsReliable) digitalWrite(blue_led_gpio, HIGH); else digitalWrite(blue_led_gpio, LOW); //GPS 
  if (WiFi.status() == WL_CONNECTED) digitalWrite(green_led_gpio, HIGH); else digitalWrite(green_led_gpio, LOW); // WIFI
  if (ntp_time_is_set) digitalWrite(yellow_led_gpio, HIGH); else digitalWrite(yellow_led_gpio, LOW); // NTP
}
#include <ESPmDNS.h>

void dns_setup(){
  //SET DNS
  //recupero il dns_name in memoria flash
  //se non c'è, non lo configuro --> per configurarlo bisogna farlo tramite http://192.168.XX.XX/cmd?cmd=dns;dns_name
  //ping esp32.local //esempio ping che deve funzionare
  //preferences_set_string("dns_name", "temperature");
  String dns_stored=preferences_get_string("dns_name");
  Serial.println("dns_name in Preferences:"+dns_stored);
  if (dns_stored.length()>0){
    if (!MDNS.begin(dns_stored))  {Serial.println("Error starting mDNS:"); Serial.println(dns_stored);}
    else {Serial.print("mDNS ok! dns_name:"); Serial.println(dns_stored);}
  }
  else Serial.println("mDNS non settato :(");
}
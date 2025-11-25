String WS_helpMessage(){
  String msg="";
  msg=msg+"IP:"+WiFi.localIP().toString()+"\r\n";
  msg=msg+"DNS name = Hostname ='"+preferences_get_string("dns_name")+"'\r\n";
  msg=msg+"available commands:\r\n";
  msg=msg+preferences_get_string("dns_name")+".local/?cmd=setdns;nuovo_nome_dns --> impone un nuovo dns name (e l'hostname viene impostato ugale)\r\n";
  msg=msg+preferences_get_string("dns_name")+".local/?cmd=getepochtime  --> restituisce epoch time GMT (+OFFSET ITALIA - incl. ora legale): from GPS; from NTP\r\n";
  return msg;
}

String WS_cmd_interpreter(String cmd){
  String s0=getValue(cmd,';',0);
  if (s0.equals("getepochtime")){
    String s_gps="0";
    if (gpsTimeIsReliable) s_gps=String(time_object_gps.getEpoch())+"(+"+String(gps_offset_italia)+")";
    String s_ntp="0";
    if (ntp_client.isTimeSet()) s_ntp=String(time_object_ntp.getEpoch())+"(+"+String(ntp_offset_italia)+")";
    return s_gps+";"+s_ntp;
  } 

  if (s0.equals("setdns")){
    Serial.println("settaggio DNS name"); 
    String DNS_name=getValue(cmd,';',1);
    if(preferences_set_string("dns_name", DNS_name)) {
      Serial.print("new DNS:"); 
      Serial.println (preferences_get_string("dns_name"));
      reboot=true;
      return ("cmd: "+cmd+" executed \r\n ESP reboot...");
    }
    else Serial.println ("set dns fallito....:(");
  }
 
  return "cmd not recognized";

}
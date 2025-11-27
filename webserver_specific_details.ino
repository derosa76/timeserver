String WS_helpMessage(){
  String msg="";
  msg=msg+"IP:"+WiFi.localIP().toString()+"\r\n";
  msg=msg+"DNS name = Hostname ='"+preferences_get_string("dns_name")+"'\r\n";
  msg=msg+"available commands:\r\n";
  msg=msg+preferences_get_string("dns_name")+".local/?cmd=setdns;nuovo_nome_dns --> impone un nuovo dns name (e l'hostname viene impostato ugale)\r\n";
  msg=msg+preferences_get_string("dns_name")+".local/?cmd=getepochtime  --> restituisce epoch time GMT (+OFFSET ITALIA - incl. ora legale): from GPS; from NTP\r\n";
  msg=msg+preferences_get_string("dns_name")+".local/?cmd=status --> mostra stato completo GPS e NTP\r\n";  // <--- AGGIUNGI QUESTA RIGA
  return msg;
}

String WS_cmd_interpreter(String cmd){
  String s0=getValue(cmd,';',0);
  
  if (s0.equals("getepochtime")){
    String s_gps="0";
    if (gpsTimeIsReliable) s_gps=String(time_object_gps.getEpoch())+"(+"+String(gps_offset_italia)+")";
    String s_ntp="0";
    if (ntp_time_is_set) s_ntp=String(time_object_ntp.getEpoch())+"(+"+String(ntp_offset_italia)+")";
    return s_gps+";"+s_ntp;
  }
  
  // NUOVO COMANDO PER DIAGNOSTICA GPS/NTP
  if (s0.equals("status")){
    String msg="=== STATUS REPORT ===\r\n\r\n";
    
    // GPS
    msg+="[GPS]\r\n";
    msg+="gpsTimeIsSet: "+String(gpsTimeIsSet?"YES":"NO")+"\r\n";
    msg+="gpsTimeIsReliable: "+String(gpsTimeIsReliable?"YES":"NO")+"\r\n";
    if (gpsTimeIsSet){
      msg+="GPS Epoch: "+String(time_object_gps.getEpoch())+"\r\n";
      msg+="GPS Time: "+time_object_gps.getTime("%Y-%m-%d %H:%M:%S")+"\r\n";
      msg+="Last GPS sync: "+String((millis()-gpsTimeLastSetMillis)/1000)+" sec ago\r\n";
    }
    msg+="GPS Year: "+String(time_object_gps.getYear())+"\r\n";
    msg+="GPS Offset Italia: "+String(gps_offset_italia)+"\r\n";
    msg+="\r\n";
    
    // NTP
    msg+="[NTP]\r\n";
    msg+="ntp_time_is_set: "+String(ntp_time_is_set?"YES":"NO")+"\r\n";
    if (ntp_time_is_set){
      msg+="NTP Epoch: "+String(time_object_ntp.getEpoch())+"\r\n";
      msg+="NTP Time: "+time_object_ntp.getTime("%Y-%m-%d %H:%M:%S")+"\r\n";
    }
    msg+="NTP Year: "+String(time_object_ntp.getYear())+"\r\n";
    msg+="NTP Offset Italia: "+String(ntp_offset_italia)+"\r\n";
    msg+="\r\n";
    
    // NTP SERVER
    msg+="[NTP SERVER]\r\n";
    msg+="Last GPS sync mark: "+String((millis()-last_gps_sync_millis)/1000)+" sec ago\r\n";
    msg+="Last NTP sync mark: "+String((millis()-last_ntp_sync_millis)/1000)+" sec ago\r\n";

    msg+="ntp ts:"+timestamp(&time_object_ntp)+"\r\n";
    msg+="gps ts:"+timestamp(&time_object_gps)+"\r\n";
    
    return msg;
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
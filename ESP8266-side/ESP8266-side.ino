// Proof of Concept - ESP8266 (ESP-01) sending HTTP requests for 
// a Web Server ('IP Webcam' app running on an Android Phone)
//
// Developed by Thiago Lages
// www.github.com/thiagolages
//
// Reference: https://arduino-esp8266.readthedocs.io/en/2.7.2/esp8266wifi/client-examples.html

#include <ESP8266WiFi.h>

int setupWifi();
int takePicture();
char strContains(char*, char*);
char* to_c_str(String);

String data(' ');

const char* ssid = "YOUR_WIFI_SSID_HERE";
const char* password = "YOUR_WIFI_PASSWORD_HERE";

// ip addr of web server running on Android phone, using IP Webcam app (e.g.: 192.168.0.X, without http:// or the port)
const char* host = "YOUR_PHONE'S_IP_ADDRESS_HERE"; // web server running on Android phone, using IP Webcam app
const int port = 8080;

WiFiClient client;

void setup()
{
  Serial.begin(115200);
//  Serial.println("ESP8266 started.");
}

void loop()
{

  while( !(Serial.available()>0) ){}
  
  data = Serial.readStringUntil('\n');
//  Serial.print("ESP received :");
//  Serial.println(data);
  
  if (strContains(to_c_str(data), "setupWifi")){ // using data.c_str() directly doesnt seem to work
//    Serial.println("setpWf STRING CMP (ESP) WORKED");
    int setupWifiOK = !setupWifi(); // setupWifi() returns 0 if everything is OK, we have to invert it
//    Serial.print("setpWf() ESP returned");
//    Serial.print(setupWifiOK);
    if (setupWifiOK){
      Serial.println("setupWifiOK");
    }
  }
  else if(strContains(to_c_str(data), "takePicture")){ // using data.c_str() directly doesnt seem to work
//    Serial.println("takPict STRING CMP (ESP) WORKED");
    int takePictureOK = !takePicture(); // takePicture() returns 0 if everything is OK, we have to invert it
//    Serial.print("tkPicf() ESP returned");
//    Serial.print(takePictureOK);
    if (takePictureOK){
      Serial.println("takePictureOK");
    }
    else{
      // send "repeat" command to Arduino
      /* to be implemented*/
    }
  } // end if setupWifi/takePicture
} // end loop

int setupWifi(){

//  Serial.println("stpWfi()-ESP running");
  
//  Serial.printf("Connecting to %s ", ssid);
//  Serial.println();
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
//    Serial.print(".");
//    delay(1000);
      delay(100);
  }
  
//  Serial.print("Connected, IP address: ");
//  Serial.println(WiFi.localIP());

  return 0;
  
}

int takePicture(){

//  Serial.println("tkPic()-ESP running");
  
  if (client.connect(host, port)){  
    
    client.print(String("GET /photo_save_only.jpg HTTP/1.1\r\n") +
    "Host: "+host+":"+port+"\r\n" +
    "User-Agent: python-requests/3.6.9\r\n" + // same user-agent as if we were using python's 'requests' lib
    "Accept-Encoding: gzip, deflate\r\n" +
    "Accept: */*\r\n" +
    "Connection: keep-alive\r\n" +
    "\r\n"
    );
   
//    delay(1000); //delay to make sure IP Webcam app can take the picture and save it correctly, otherwise its too fast (we're doing it using option 2 below)

    
    /* we can check if we were succesful here, on later versions*/
    String line;
//    Option 1: didnt work well, has to improve
//    while (client.available()){
//      line = client.readStringUntil('\n');
//      if (line.endsWith("</result>")){
//        if (strContains(to_c_str(line), "Ok")){ // line should be equal to "<result>Ok</result>" (thats what the server responds)
//          return 0;// everything worked great
//        }
//        else{
//          return 1;// something went wrong
//        }
//      }
//    }   

//    Option 2: working well
    while (client.connected() || client.available()){
      if (client.available()){
        line = client.readStringUntil('\n');
//        Serial.println(line);
      } 
    }
    client.stop(); // Disconnect from the server
    return 0; // everything worked great
  }
  else
  {
//    Serial.println("connection failed!]");
    client.stop();
    return 1; // failed
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char strContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {return 0;}
    
    while (index < len) {
      if (str[index] == sfind[found]) {
        found++;
        if (strlen(sfind) == found) {return 1;}
      }
      else {found = 0;}
      index++;
    }
    return 0;
}


// dont forget to send '\n' as well !
char* to_c_str(String str){ //using this instead of String.c_str() when sending it to strContains()
  
  char *c_str = (char*)malloc(str.length() + 1);
  
  strcpy(c_str,str.c_str());
  
  return c_str;
}

//char* to_c_str(String str){
//  
//  int len = str.length(); // doesnt include null char
//  Serial.print("Len = ");
//  Serial.println(len);
//  
//  char* c_str[len+2];      // +2 in case theres no '\n'(line ending), and we have to include it, together with '\0' (null char)
//  int idx = 0;
//
//  // prevents checking str.chatAt() at an invalid index. (len-1) because length returns N and we go from 0 to N-1
//  while( idx <= (len-1) ){
//    if(str.charAt(idx) != '\n'){
//      *c_str[idx] = str.charAt(idx);
//      idx++;
//    }
//  }
//
//  if(*c_str[idx-1] != '\n'){ // if the last char is not '\n'
//      *c_str[idx] = '\n';
//      *c_str[idx] = '\0';
//    }
//  
//  return *c_str; 
//}

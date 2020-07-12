/*
//########################################################################
//#  ESP8266 MQTT Sprinkler controller                                   #
//#  Code refactored 12/27/2016 to work with the new Arduinio IDE 1.6.13 #
//#  Blog post, videos, and code can  be found at www.electronhacks.com  #
//########################################################################
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Timer.h" // Timer library
Timer t;

//Variables
//Update these with values suitable for your network.
const char* ssid = "wifissid";
const char* password = "wifipassword";
const char* mqtt_server = "mqttipaddress";
const char* mqtt_topic = "HomeAutomation/Sprinkler/Command";
const char* mqtt_status = "HomeAutomation/Sprinkler/Status";
const char* mqtt_user = "mqtt";
const char* mqtt_password = "mqtt";

char vInp13 = 0;
String rx;
int  rxLength = 0;
int timer1Setting = 600;
int timer2Setting = 600;
int timer3Setting = 300;
int timer4Setting = 300;
int timer1 = 0;
int timer2 = 0;
int timer3 = 0;
int timer4 = 0;
int run1 = 0;
int run2 = 0;
int run3 = 0;
int run4 = 0;
int run1Stat = 0;
int run2Stat = 0;
int run3Stat = 0;
int run4Stat = 0;
int firstOut  = 0;


//Analog input variables
int vA0 = 0;
int iA0 = 0;
int Counter = 0;
int watchDog = 0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  t.every(1000, takeReading);
  t.every(10000, outputs);
  
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  digitalWrite(D2, 1);  
  digitalWrite(D3, 1); 
  digitalWrite(D4, 1); 
  digitalWrite(D5, 1); 
  
  pinMode(13, INPUT_PULLUP);

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


////////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length) 
{\
  Serial.print("RX: ");
  //Convert and clean up the MQTT payload messsage into a String
  rx = String((char *)payload);                    //Payload is a Byte Array, convert to char to load it into the "String" object 
  rxLength = rx.length();                          //Figure out how long the resulting String object is 
  for (int i = length; i <= rxLength; i++)         //Loop through setting extra characters to null as garbage may be there
  {
    rx.setCharAt(i, ' ');
  }
  rx.trim();                                       //Use the Trim function to finish cleaning up the string   
  Serial.print(rx);                                //Print the recieved message to serial
  Serial.println();

  //Evaulate the recieved message to do stuff

  if (rx == "Run1On"){
    run1 = 1;
  }  
  if (rx == "Run1Off"){
    run1 = 0;
  }   
  
  if (rx == "Run2On"){
    run2 = 1;
  }  
  if (rx == "Run2Off"){
    run2 = 0;
  }   
  
  if (rx == "Run3On"){
    run3 = 1;
  }  
  if (rx == "Run3Off"){
    run3 = 0;
  }   
    
  if (rx == "Run4On"){
    run4 = 1;
  }  
  if (rx == "Run4Off"){
    run4 = 0;
  }  
  outputs();
}


/////////////////////////////////////////////////
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_topic,mqtt_user,mqtt_password)) {
    //if (client.connect(mqtt_topic)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_status, "Online");
      // ... and resubscribe
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


///////////////////////////////////////////
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }

/*
  ///// Evaluate input 13 and send a message if the value changes 
  if (digitalRead(13) != vInp13)
  {
   vInp13 = digitalRead(13);
   if (vInp13 == LOW)
   {    
     client.publish(mqtt_topic, "DoorOpened");
     Serial.println("TX: DoorOpened");
   }
   else
   {
     client.publish(mqtt_topic, "DoorClosed");
     Serial.println("TX: DoorClosed");
   }
  }
  /*
 //Evaluate ADC
 /* iA0 = analogRead(A0); // Read the analog value
  if (Counter >= 150)   // Counter reduces how often we update to help prevent broker spam
  {
    Counter = 0;
    //if (1==1)   //For testing and scaling,
    if (((iA0 * 1.02) < vA0) or ((iA0 * 0.98) > vA0))   //(Deadband)Only Update if the raw value is +- 10% 
    {
     vA0 = iA0;                               //Set the variables equal to eachother for next time comparison
     iA0 = ((iA0/9.76)+12);                  //Scale and offset the value to Degrees F https://www.youtube.com/watch?v=ub20R9WnH-g
     String str = (String)iA0;                //PubSubClient requires data to be a char* (char array) so we have to convert the int
     int str_len = str.length() +1;           //Length + null terminator
     char char_array[str_len];                //Prepare the character array (buffer)
     str.toCharArray(char_array, str_len);    // Copy it over 
     client.publish(mqtt_topic, char_array); // Publish to MQTT
     Serial.print("TX: RAW: ");                    // Print to Seral window  
     Serial.print(vA0);    
     Serial.print("   Scaled: ");                    // Print to Seral window  
     Serial.println(iA0);        
    }  
  }  
  else
  {
    Counter++;
  }  */
  
  client.loop();
  t.update();
  delay(10);  
}


//////////////////////////////////////////////////////////////
void takeReading(void)
{
  if (timer1 > 0){
    if ((run1Stat == 1) and (timer1 >= timer1Setting)){ 
      //Serial.print("timer1: ");
      //Serial.println(timer1);
      timer1 = 0;
      run1 = 0;
      outputs();
  
    }   
    else{
      timer1 ++;
      //Serial.print("timer1: ");
      //Serial.println(timer1);
    }
  }

  if (timer2 > 0){
    if ((run2Stat == 1) and (timer2 >= timer2Setting)){ 
      timer2 = 0;
      run2 = 0;
      outputs();
    }   
    else{
      timer2 ++;
    }
   }

  if (timer3 > 0){
    if ((run3Stat == 1) and (timer3 >= timer3Setting)){ 
      timer3 = 0;
      run3 = 0;
      outputs();
    }   
    else{
      timer3 ++;
    }
  }

  if (timer4 > 0){
    if ((run4Stat == 1) and (timer4 >= timer4Setting)){ 
      timer4 = 0;
      run4 = 0;
      outputs();
    }   
    else{
      timer4 ++;
    }
  }
}


/////////////////////////////////////////////////////////
void outputs()
{
  // Watchdog section to know via MQTT if the system is running and online
  if (watchDog > 10000){
    watchDog = 0;
  }
  else{
    watchDog = watchDog + 1;
  }
  String str = (String)watchDog;           //PubSubClient requires data to be a char* (char array) so we have to convert the int
  int str_len = str.length() +1;           //Length + null terminator
  char char_array[str_len];                //Prepare the character array (buffer)
  str.toCharArray(char_array, str_len);    // Copy it over 
  client.publish("HomeAutomation/Sprinkler/WatchDog", char_array);  // Publish to MQTT
 
  firstOutSub();
  if (run1 == 1 and firstOut == 0){
    digitalWrite(D2, 0); //run
    client.publish("HomeAutomation/Sprinkler/Run1", "1");
    run1Stat = 1;
    timer1 = 1;
    firstOutSub();
  }
  else if (run1 == 0 and run1Stat == 1){
    digitalWrite(D2, 1); //stop 
    client.publish("HomeAutomation/Sprinkler/Run1", "0");
    run1Stat = 0;
    timer1 = 0;
    firstOutSub();
  }

  if (run2 == 1 and firstOut == 0){
    digitalWrite(D3, 0); //run 
    client.publish("HomeAutomation/Sprinkler/Run2", "1");
    run2Stat = 1;
    timer2 = 1;
    firstOutSub();
  }
  else if (run2 == 0 and run2Stat == 1){
    digitalWrite(D3, 1); //stop 
    client.publish("HomeAutomation/Sprinkler/Run2", "0");
    run2Stat = 0;
    timer2 = 0;
    firstOutSub();
  }

  if (run3 == 1 and firstOut == 0){
    digitalWrite(D4, 0); //run 
    client.publish("HomeAutomation/Sprinkler/Run3", "1");
    run3Stat = 1;
    timer3 = 1;
    firstOutSub();
  }
  else if (run3 == 0 and run3Stat == 1){
    digitalWrite(D4, 1); //stop 
    client.publish("HomeAutomation/Sprinkler/Run3", "0");
    run3Stat = 0;
    timer3 = 0;
    firstOutSub();
  }

  if (run4 == 1 and firstOut == 0){
    digitalWrite(D5, 0); //run 
    client.publish("HomeAutomation/Sprinkler/Run4", "1");
    run4Stat = 1;
    timer4 = 1;
    firstOutSub();    
  }
  else if (run4 == 0 and run4Stat == 1){
    digitalWrite(D5, 1); //stop 
    client.publish("HomeAutomation/Sprinkler/Run4", "0");  
    run4Stat = 0;
    timer4 = 0;
    firstOutSub();    
  }
}


void firstOutSub(){
  if (run1Stat == 1 or run2Stat == 1 or run3Stat == 1 or run4Stat == 1){
    firstOut = 1;
  }
  else if (run1Stat == 0 and run2Stat == 0 and run3Stat == 0 and run4Stat == 0){
    firstOut = 0;
  }
}

# Arduino Based MQTT Sprinkler Controller
A DIY Sprinkler controller using Node-Red and ESP8266 Arduino.

In my setup the hardware is an Arduino WeMos D1  (with the esp8266 chip for the microcontroller ) with a four relay arduino board.

![alt text](https://github.com/jhottell/Arduino_MQTT_SprinklerController_/blob/master/images/SprinklerController.jpg)

There are four sprinkler runs.

The arduino listens on HomeAutomation/Sprinkler/Command for a command to turn on and off runs. You can turn on all four at once and the arduino will run the runs one at a time for 20 minutes each.

The arduino publishes the status of the runs off or on as well as a Watch dog counter so that what ever you are using for a dashboard can tell if the arduino is connected and running.

The Node-RED-flows.json can be imported to your own NodeRED for a user interface. Node-RED also can implement a schedule to run the sprinklers at a specified time.

![alt text](https://github.com/jhottell/Arduino_MQTT_SprinklerController_/blob/master/images/Node-REDUI.png)

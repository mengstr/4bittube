# 4bittube

## ESP8266 code for driving (multiple) "4-bit LED Tube" modules

This is a small example project for the ESP8266 NONOS SDK driving the cheap 4-digit LED display modules.

The modules have two 74HC595 on the back and runs just fine powered by the 3.3V output from the NodeMCU.

They are controlled by three GPIOs from the ESP8266. The GPIO's used and  how many modules that are connected in series are specified in the initializer funcion.

``` 
uint8_t *disp; 
disp=Init_4bittube(count, dataGPIO, clockGPIO, latchGPIO);
```
The function returns a pointer to an array that holds the display buffer. One byte per digit.



**This is the QIFEI module**

![The modules](/images/frontback.png?raw=true "The modules from QIFEI")

**Three modules in series connected to a NodeMCU**

![Running modules](/images/3modules.png?raw=true "The modules in action connected to a NodeMCU")

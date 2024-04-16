
# IoT_LED-Matrix-64x64_p
LED-Matrix Display driven by Raspi PICOW  Comchain: openHAB - mqtt - wifi - display develoed with platformio
**IoT RGB-Matrixdisplay to visualize temperatures from air ant HOT Tub via WiFi over MQTT to openHAB**
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/Screenshot%202024-04-04%20214645.png)
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/photo_2024-04-16_13-58-31.jpg)
# Goal of this project
To use a 64x64 RGB matrix display for visualisation of temperature, time and systemstate of my HotTub (whrilpool).
The picture should be absolute flickerree.

The project consits of three parts:

## Part 1: 
Hardwaredesign controller RP 2040 PICO W and powershifting 3.3 V to 5 V 
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/Screenshot%202024-04-16%20175854.png)
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/Screenshot%202024-04-16%20175725.png)
## Part 2: 
Softwaredevelopment matrix control, connection to Wifi, connection to mqtt 

## Part 3: 3D Case design
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/photo_2024-04-16_13-59-08.jpg)
![](https://github.com/ene9ba/IoT_LED-Matrix-64x64_p/blob/main/Doc/Screenshot%202024-04-04%20214609.png)
## Info:
The LED-Panel is integrated in a self designed case with integrated power supply. Communication to SMART-Home runs via WiFi and MQTT to opnHAB.
The contoller I'm using is Raspberry Pi PICO RP2040 because it meets the requriements for WiFi and control the LED-panel **flickerfree** using the realtine state-machine on the chip.

The pcb will be pluged directly to the pins on the panel, there is no need for additional cabling.

Software ist developed with PlatformIO IDE.

I'm using displaytype RGB_LEF Joy-It. Most pinouts from other manaufactorers are the same, but have a look to the pcb pinout to be shure pining fits.

### Designfiles for the Case you can find on [thingeverese]() or [printables]().

### Credits and Acknowledgments
This project builds upon the work of the following contributors:

**![pitschu](https://github.com/pitschu/RP2040matrix-v2):** For providing the initial groundwork and hardware interfacing for driving HUB75 LED Matrix panels with Raspberry Pi Pico's PIO

**Bodmer/Adafruit-GFX-Library:** For the core graphics library, enabling compatibility with Adafruit's GFX library and enhancing the display manipulation capabilities of the project.

**![ClemensElflein](https://github.com/ClemensElflein/RP2040Matrix):** For adapt the software to run on pico w without FREERTOS (works now with Arduino WifFi libraries) 


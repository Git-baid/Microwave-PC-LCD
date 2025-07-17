# Microwave PC LCD
I [built a computer inside a microwave](https://imgur.com/a/diy-gaming-microwave-nef2UVq) and replaced the standard segmented display with an LCD screen controlled by an ESP32 microcontroller so I can display whatever I want. Normally it is just set to emulate a regular microwave display with a segmented font displaying the actual time.
When the display first turns on, it connects to my Wifi, asks an [NTP server](https://www.ntppool.org/en/) what time it is, then closes the wifi connection and keep track of the time from there. This way I dont need batteries to keep the time even when turned off, but does require a brief internet connection.
![Microwave PC](https://github.com/Git-baid/Microwave-PC-LCD/blob/main/Screenshot%202025-07-17%20143044.png)
The screen can also display the current state of the microwave door whether "opened" or "closed". Not sure what I want to do with this yet but the framework is in place.
It can also display the current temperatures of my CPU and GPU components of my computer and interface with [baidbot](https://github.com/Git-baid/baidbotDiscord) to allow users with access to display whatever image they want on my screen at anytime anonymously.

<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/20250717_143116.jpg" width = 200>
<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/20250717_143217.jpg" width = 200>

I've integrated with the standard microwave control panel buttons in order to control certain aspects of the screen. 
- "Meat" will return to the standard clock display after displaying an image.
- "Power" restarts the display
- "Defrost" puts the ESP32 Microcontroller into boot mode (allows updating firmware)

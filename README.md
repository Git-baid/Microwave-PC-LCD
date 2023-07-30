# Microwave PC LCD
**Anything related to the LCD on the control panel of baid's [custom microwave PC build](https://imgur.com/a/Ty57pLM)**

<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/20230725_234915.jpg" width="600" align="center">
<img src="https://cdn.discordapp.com/attachments/434752033552203793/1135023352122179715/20230729_173715.jpg" width="500" align="left">



The microwave display is an LCD TFT screen connected to an ESP32 microcontroller on the inside of the PC.
This uses a modified seven-segment font, internal ESP32 RTC, and the [NTP Pool Project](https://www.pool.ntp.org/zone/north-america) to simulate a traditional microwave clock with accurate time.

It also displays current GPU and CPU temperatures of my computer updating every few seconds using a small server hosted on the ESP32. When my PC is off, the display continues to show the time. The temperature displays disappear automatically.


Another functionality I added just for fun is the ability for anyone with access to my [baidbot discord bot](https://github.com/Git-baid/baidbotDiscord) to send a command with an attached image to display it on my microwave. Pressing the clock button clears this image and returns to the clock display.
For example, baidbot's current icon looks like-

<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/baidbot800x.png" width="320" align="left">


<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/20230725_235203.jpg" width="290" align="left">

<img src="https://github.com/Git-baid/Microwave-PC-LCD/blob/main/20230725_235213.jpg" width="290" align="left">


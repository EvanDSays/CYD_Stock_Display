# CYD_Stock_Display
Display two stocks on your Cheap Yelow Display (CYD)
![Picture of the CYD running the CYD_Stock_Display program](https://i.imgur.com/KCgmdfg.jpeg)

# Dependencies
[TimeLib](https://github.com/PaulStoffregen/Time.git)

[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)

[ArduinoJson](https://github.com/bblanchon/ArduinoJson)

# Instructions
1. Get an API key from [Finnhub.io](https://finnhub.io/)
2. Override the TFT_eSPI User_Setup.h file with the one I provided
3. Configure the secrets and stocks at the top of the main.cpp file I provided
4. Upload to your CYD

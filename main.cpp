#include <TFT_eSPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;
#define FONT_STYLE 4
#define FONT_SIZE 26
#define SPACING 4
#define HLINE_THICKNESS 1
#define TRIANGLE_HEIGHT 30
#define UPDATE_INTERVAL 60000UL  // 1 minute

// Secrets
const String finnhubAPIKey = "YOUR_API_KEY_HERE";
const String ssid = "YOUR_WIFI_SSID_NAME_HERE"; 
const String pass = "YOUR_WIFI_PASSWORD_HERE";

// Pick the two stocks
const String stock1 = "AAPL";
const String stock1Name = "Apple";
const String stock2 = "SPY";
const String stock2Name = "S&P 500";

TFT_eSPI tft = TFT_eSPI();

// Declare functions
void fetch_and_update_stock();
void checkStock(String, String, int);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Black out the screen
  tft.fillScreen(TFT_BLACK);

  // Connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
        delay(100);
  }

  // Start the tft display
  tft.init();
  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);

  // Get stock info
  fetch_and_update_stock();
}

void loop() {
  delay(UPDATE_INTERVAL);
  fetch_and_update_stock();
}

void fetch_and_update_stock() {

  // Only update stock if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) return;

  // Set the font style
  tft.setTextFont(FONT_STYLE);

  // Get info for stock 1 and put it in the top half of the screen
  checkStock(stock1, stock1Name, 0);

  // Draw a line in the middle of the screen
  tft.drawFastHLine(0,centerY,SCREEN_WIDTH,TFT_WHITE);

  // Get the info for stock 2 and put it in the second half of the screen
  checkStock(stock2, stock2Name, centerY + HLINE_THICKNESS);
}

void checkStock(String ticker, String stockName, int yCoord){

  // Calculate where the text will start
  int startingTextYCoord = yCoord + SPACING + (FONT_SIZE/2);

  String url = String("https://finnhub.io/api/v1/quote?symbol=" + ticker + "&token=" + finnhubAPIKey);

  HTTPClient https;
  https.begin(url);

  if (https.GET() == HTTP_CODE_OK) {
    String payload = https.getString();
    JsonDocument doc;

    if (deserializeJson(doc, payload) == DeserializationError::Ok) {

      // Collect the information from the API
      float price = doc["c"];
      float percentageChange = doc["dp"];
      unsigned long asOf = doc["t"];
      
      String asOfStr = "";
      String weekdayStr = "";
      String percentageChangeStr = "";
      int weekdayInt = weekday(asOf);

      // Determine day of week from timestamp
      switch(weekdayInt){
        case 1:
          weekdayStr = "Sun";
          break;
        case 2:
          weekdayStr = "Mon";
          break;
        case 3:
          weekdayStr = "Tues";
          break;
        case 4:
          weekdayStr = "Wed";
          break;
        case 5:
          weekdayStr = "Thur";
          break;
        case 6:
          weekdayStr = "Fri";
          break;
        case 7:
          weekdayStr = "Sat";
      }      

      // Create the timestamp string for the screen
      asOfStr = "As of " + weekdayStr + " " + String(month(asOf)) + "/" + String(day(asOf)) + " " + String(hour(asOf)-5) + ":";
      // Add a zero to the minute if it is less than 10
      if(minute(asOf) < 10){
        asOfStr = asOfStr + "0";
      }
      asOfStr = asOfStr + String(minute(asOf));

      //Clear previous numbers
      tft.fillRect(0,yCoord,SCREEN_WIDTH,yCoord+(SPACING*4)+(FONT_SIZE*4),TFT_BLACK);
      
      // Text is centered
      tft.setTextDatum(CC_DATUM);

      // Add stock ticker to screen in white text
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(stockName + " (" + ticker + ")", centerX, startingTextYCoord);

      // Set numbers and arrow to red if stock is down or green if up
      if(percentageChange < 0){
        percentageChangeStr = String(percentageChange) + "%";
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.fillTriangle(SPACING + (TRIANGLE_HEIGHT/2), yCoord + SPACING+TRIANGLE_HEIGHT,
                        SPACING,                        yCoord + SPACING,
                        SPACING + TRIANGLE_HEIGHT,      yCoord + SPACING,
                        TFT_RED);
      }
      else{
        percentageChangeStr = "+" + String(percentageChange) + "%";
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.fillTriangle(SPACING + (TRIANGLE_HEIGHT/2), yCoord + SPACING,
                        SPACING,                        yCoord + SPACING + TRIANGLE_HEIGHT,
                        SPACING + TRIANGLE_HEIGHT,      yCoord + SPACING + TRIANGLE_HEIGHT,
                        TFT_GREEN);
      }
      
      // Add percentage change, price, and timestamp to the screen
      tft.drawString(percentageChangeStr,
                    centerX, startingTextYCoord+(SPACING*1)+(FONT_SIZE*1));
      tft.drawString("$" + String(price),
                    centerX, startingTextYCoord+(SPACING*2)+(FONT_SIZE*2));
      tft.drawString(asOfStr,
                    centerX, startingTextYCoord+(SPACING*3)+(FONT_SIZE*3));

    } else {
      Serial.print("JSON parse failed\n");
    }
  } else {
    Serial.println("HTTP GET failed");
  }
  https.end();
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1  // Reset pin not used for this OLED module (change if needed)
#define SCREEN_ADDRESS 0x3C  // I2C address for the OLED module (check your module's datasheet)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//---------------------------------------------------SCREEN

#include <DHT.h>
#define DHTPIN 3        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Type of the DHT sensor (DHT11 or DHT22)

DHT dht(DHTPIN, DHTTYPE);
//---------------------------------------------------SENSOR

const int lowerPin = 0;  // Replace with the actual pin number for button 1
const int higherPin = 1;  // Replace with the actual pin number for button 2
const int okPin = 2;

const int outputValvePin = 6;
const int outputFanPin = 4;
//---------------------------------------------------BUTTONS

int humidity = 40;
int temperature = 50;
int hours = 12;
int speed = 5;

bool initializing = true;

int secondsPassed = 0;
int currentWantedTemp;

void updateVariable(int &variable, const char* prompt) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(prompt);
  display.print(variable);

  display.display();

  while (digitalRead(okPin) == HIGH) {
    if (digitalRead(higherPin) == LOW) {
      delay(200);
      variable++;
      Serial.println(variable);
    }
    if (digitalRead(lowerPin) == LOW) {
      delay(200);
      variable--;
      Serial.println(variable);
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(prompt);
    display.print(variable);
    display.display();
  }
}

void setup() {
  pinMode(lowerPin, INPUT_PULLUP);
  pinMode(higherPin, INPUT_PULLUP);
  pinMode(okPin, INPUT_PULLUP);
  pinMode(outputValvePin, OUTPUT);
  pinMode(outputFanPin, OUTPUT);

  dht.begin();  
  currentWantedTemp = dht.readTemperature() + 5;

  if(!display.begin(SCREEN_ADDRESS, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(2000);  // Pause for 2 seconds
  display.clearDisplay();  // Clear the display buffer
  display.setTextSize(1);  // Set text size
  display.setTextColor(SSD1306_WHITE);

}

// the loop function runs over and over again forever
void loop() {
  if (initializing) {
    updateVariable(humidity, "Humidity: ");
    updateVariable(temperature, "Temperature: ");
    updateVariable(hours, "Hours: ");
    updateVariable(speed, "Speed: ");

    if (digitalRead(okPin) == LOW) {
      initializing = false;
      Serial.println("Initialization complete!");
      delay(1000);
    }
  } 
  else 
  {
    // Your main program logic here
    // Read temperature and humidity every 20 seconds
      if(hours > 0)
      {
        float readTemperature = dht.readTemperature();
        float readHumidity = dht.readHumidity();

        // Control the valve based on temperature
        if (readTemperature < currentWantedTemp && readTemperature < temperature) {
          digitalWrite(outputValvePin, HIGH); // Open valve
          Serial.println("Valve opened");
        } else {
          digitalWrite(outputValvePin, LOW); // Close valve
          Serial.println("Valve closed");
        }

        // Control the fan based on humidity
        if (readHumidity > humidity) {
          digitalWrite(outputFanPin, HIGH); // Turn on fan
          Serial.println("Fan turned on");
        } else {
          digitalWrite(outputFanPin, LOW); // Turn off fan
          Serial.println("Fan turned off");
        }

        // Display the updated values on the OLED screen
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.print("Temp: ");
        display.print(readTemperature);
        display.print(" C  ");
        display.print("Humid: ");
        display.print(readHumidity);
        display.print(" %");
        display.display();

      // Your additional logic here
        delay(180000);  // Add a delay for better readability, adjust as needed
        secondsPassed += 180;
        if(secondsPassed == 3600)
        {
          hours--;
          secondsPassed = 0;
          currentWantedTemp += 5;
        } 
      }
      else
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.print("Program ended. Press ok for restart.");
        display.display();
        if (digitalRead(okPin) == LOW) 
        {
          initializing = true;
          humidity = 40;
          temperature = 50;
          hours = 12;
          speed = 5;
          Serial.println("restarting!");
          delay(1000);
        }
      }
  }
} 
  



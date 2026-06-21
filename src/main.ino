#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 21
#define LED_COUNT 16
#define CLK_PIN 1
#define DT_PIN 2
#define AMPSD_PIN 3

int brightness = 100; // Start at ~40% brightness
int lastCLK = HIGH;   
const int buttonPin = 16;
int buttonCounter = 0;

Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int currentRedValue = 0;
int currentGreenValue = 0;
int currentBlueValue = 0;

// Smooth non-blocking color interpolation
unsigned long lastColorMillis = 0;
const unsigned long colorInterval = 30; // ms between color updates; smoother interpolation
float curR, curG, curB; // floating accumulators for smooth transitions

int targetRedValue = random(0,255);
int targetGreenValue = random(0,255);
int targetBlueValue = random(0,255);

bool redRising = true;
bool greenRising = true;
bool blueRising = true;

int targetRed = 0;
int targetGreen = 0;
int targetBlue = 0;
unsigned long lastFlickerMillis = 0;
const unsigned long flameFlickerInterval = 70;
const unsigned long waterFlickerInterval = 90;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(AMPSD_PIN, OUTPUT);
  digitalWrite(AMPSD_PIN, LOW);
  ring.begin();
  ring.setBrightness(brightness);
  ring.clear();
  ring.show();
  // initialize float accumulators
  curR = currentRedValue;
  curG = currentGreenValue;
  curB = currentBlueValue;
}

void loop() {
  checkButtonPress();
  int currentCLK = digitalRead(CLK_PIN);

// Detect rotation only when CLK changes from HIGH to LOW
if (currentCLK == LOW && lastCLK == HIGH) {
  if (digitalRead(DT_PIN) != currentCLK) {
    brightness = min(brightness + 25, 255); // Turn Right: Increase
  } else {
    brightness = max(brightness - 25, 0);   // Turn Left: Decrease
  }
  ring.setBrightness(brightness);
  ring.show();
}

lastCLK = currentCLK;
  unsigned long now = millis();
  if (buttonCounter == 0) {
    if (now - lastColorMillis >= colorInterval) {
      lastColorMillis = now;
      const float step = 0.5f; // smaller = smoother; 0.5 gives nice interpolation at 35ms

      // red channel
      if (abs(curR - targetRedValue) < step) {
        curR = targetRedValue;
        targetRedValue = random(0, 255);  // always get new target when reached
      } else {
        curR += (curR < targetRedValue) ? step : -step;
      }

      // green channel
      if (abs(curG - targetGreenValue) < step) {
        curG = targetGreenValue;
        targetGreenValue = random(0, 255);  // always get new target when reached
      } else {
        curG += (curG < targetGreenValue) ? step : -step;
      }

      // blue channel
      if (abs(curB - targetBlueValue) < step) {
        curB = targetBlueValue;
        targetBlueValue = random(0, 255);  // always get new target when reached
      } else {
        curB += (curB < targetBlueValue) ? step : -step;
      }

      // apply and show
      ring.fill(ring.Color((uint8_t)curR, (uint8_t)curG, (uint8_t)curB));
      ring.show();
    }
  } else if (buttonCounter == 1) {
    ring.fill(ring.Color(62,62,62));
    ring.show();
  } else if (buttonCounter == 2) {
    ring.fill(ring.Color(125,125,125));
    ring.show();
  } else if (buttonCounter == 3) {
    ring.fill(ring.Color(255,255,255));
    ring.show();

  } else if (buttonCounter == 4) {  // Flame Animation
      if (now - lastFlickerMillis >= flameFlickerInterval) {
        lastFlickerMillis = now;
        for (int i = 0; i < LED_COUNT; i++) {
          // Randomly decide whether the current pixel should be on or off
          if (random(0, 2) == 0) {
            // Choose a random red value for flame effect (low red is more yellow, high is more red)
            targetRed = random(50, 255);

            // Set green proportional to red for a reddish-orange color
            targetGreen = random(0, (targetRed / 2));  // Ensure green is a bit lower than red

            // Set the current pixel to a flame color
            ring.setPixelColor(i, targetRed, targetGreen, 0); // Set RGB, no blue
          } else {
            // Turn off the pixel to simulate flickering
            ring.setPixelColor(i, 0, 0, 0);  // Turn off pixel
          }
        }
        ring.show();  // Update the ring with the new pixel colors
      }
  
  } else if (buttonCounter == 5) {     // water animation 
      if (now - lastFlickerMillis >= waterFlickerInterval) {
        lastFlickerMillis = now;
        for (int i = 0; i < LED_COUNT; i++) {
          // Randomly decide whether the current pixel should be on or off
          if (random(0, 2) == 0) {
            // Choose a random blue value for water effect
            targetBlue = random(70, 255);

            // Set green proportional to blue for a teal blue / green color
            targetGreen = random(0, targetBlue);  // Ensure green is a bit lower than red

            // Set the current pixel to a flame color
            ring.setPixelColor(i, 0, targetGreen, targetBlue); // Set RGB, no red
          } else {
            // Turn off the pixel to simulate flickering
            ring.setPixelColor(i, 0, 0, 0);  // Turn off pixel
          }
        }
        ring.show();  // Update the ring with the new pixel colors
      }
  
  } else if (buttonCounter == 6) {
    ring.fill(ring.Color(255,45,0));
    ring.show();
  } else if (buttonCounter == 7){
    ring.clear();
    ring.show();
  }
}

void checkButtonPress() {
    static bool lastButtonState = HIGH;
    bool currentButtonState = digitalRead(buttonPin);

    if (currentButtonState == LOW && lastButtonState == HIGH) {
        delay(30);  // Debounce delay

        if (digitalRead(buttonPin) == LOW) {
            buttonCounter++;
            ring.clear();
            ring.show();
            if (buttonCounter > 7) {
                buttonCounter = 0;
            }
        }
    }

    lastButtonState = currentButtonState;
}

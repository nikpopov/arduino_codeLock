// Code Lock PROJECT
/* Use Arduino NANO as microcontroller
Allow to use 6-digit secret code for access
Allow user change secret code using by password
All data display on LCD 16x2 (with I2C interface)

Components' needed:
ARDUINO NANO
3x4 Keypad
LCD 16x2 with I2C interface (can be used without I2C interface but needs to change LCD control code)
Buzzer
Electromagnetic Lock (can be used with electromotor inside)

Pin Connections
* ARDUINO NANO |  Keypad   |  LCD I2C 16x2  | Buzzer
*--------------|-----------|----------------|--------
*   +5V        |           |       VIN      |   VIN(+)
*    GND       |           |       GND      |
*    A5        |           |       SCL      |
*    A4        |           |       SDA      |
*    A0        |    R1     |                |
*    A1        |    R2     |                |
*    A2        |    R3     |                |
*    A3        |    R4     |                |
*    D9        |    C1     |                |
*    D8        |    C2     |                |
*    D7        |    C3     |                |
*    D10       |    C3     |                |   GND(-)
*/

#include <Wire.h>   // Include Wire library for I2C communication
#include <Adafruit_LiquidCrystal.h>   // Include Liquid Crystal Display 20x4 library
#include <Keypad.h>   // Include librarry for 3x4 keypad control

// Initialize the LCD library with the I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7};        // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String password = "1234";  // Default password
String code = "654321";    // Default code
String input = "";
String modePassword = "0000"; // Special password for entering edit mode
String currentPassword = "";
bool editMode = false;
enum EditState { NONE, EDIT_CODE, EDIT_PASSWORD, CONFIRM_PASSWORD };
EditState editState = NONE;

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.print("Enter Code:");
  pinMode(10, OUTPUT); // Buzzer pin
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '*') {
      input = "";
      lcd.clear();
      lcd.print("Enter Code:");
    } else if (key == '#') {
      if (!editMode) {
        if (input == code) {
          lcd.clear();
          lcd.print("Access Granted");
          delay(2000);
          lcd.clear();
          lcd.print("Enter Code:");
          input = "";
          tone(10, 1000, 200); // Buzzer sound
        } else if (input == modePassword) {
          lcd.clear();
          lcd.print("Enter Password:");
          input = "";
          editMode = true;
        } else {
          lcd.clear();
          lcd.print("Access Denied");
          delay(2000);
          lcd.clear();
          lcd.print("Enter Code:");
          input = "";
          tone(10, 2000, 200); // Buzzer sound
        }
      } else {
        switch (editState) {
          case NONE:
            if (input == password) {
              lcd.clear();
              lcd.print("1:Edit Code");
              lcd.setCursor(0, 1);
              lcd.print("2:Edit Password");
              editState = CONFIRM_PASSWORD;
            } else {
              lcd.clear();
              lcd.print("Wrong Password");
              delay(2000);
              lcd.clear();
              lcd.print("Enter Password:");
            }
            input = "";
            break;
          case CONFIRM_PASSWORD:
            if (input == "1") {
              lcd.clear();
              lcd.print("New 6-digit Code:");
              editState = EDIT_CODE;
            } else if (input == "2") {
              lcd.clear();
              lcd.print("New Password:");
              editState = EDIT_PASSWORD;
            }
            input = "";
            break;
          case EDIT_CODE:
            if (input.length() == 6) {
              code = input;
              lcd.clear();
              lcd.print("Code Updated");
              delay(2000);
              exitEditMode();
            }
            break;
          case EDIT_PASSWORD:
            if (input.length() == 4) {
              password = input;
              lcd.clear();
              lcd.print("Password Updated");
              delay(2000);
              exitEditMode();
            }
            break;
        }
      }
    } else {
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    }
  }
}

void exitEditMode() {
  lcd.clear();
  lcd.print("Enter Code:");
  input = "";
  editMode = false;
  editState = NONE;
}

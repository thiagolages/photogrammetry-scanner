#include <LiquidCrystal.h>
#include <Stepper.h>
#include <Servo.h>

//#define LOG_SERIAL // used when there's no LCD available 0 - dont print to Serial; 1 - print to Serial;

bool checkNeutral(int, char);
void initialSetupJoystick();
int invertReading(int); // inverts the direction of the joystick. If it ranges from 0-1023 in one direction, it will range from 1023-0 instead
int takePicture();
int setupWifi();
char strContains(char*, char*);

LiquidCrystal lcd(13, 2, 4, 5, 6, 7);  // Use these pins for the 1602 lcd

/********************************************************/
// These will all be defined inside initialSetupJoystick()
int X_RIGHT_THRESHOLD;
int X_LEFT_THRESHOLD;

int Y_UP_THRESHOLD;
int Y_DOWN_THRESHOLD;

int X_NEUTRAL_LOW_THRESHOLD;
int X_NEUTRAL_HIGH_THRESHOLD;

int Y_NEUTRAL_LOW_THRESHOLD;
int Y_NEUTRAL_HIGH_THRESHOLD;

/********************************************************/


const int SW_pin = 8; // digital pin connected to switch output
const int X_pin = A1; // analog pin connected to X output
const int Y_pin = A0; // analog pin connected to Y output

int MenuNr = 0;   // Menu number
int PhotoNr = 2;  // The amount of photos that have to be taken
bool Flag1 = 0;   // This flag is only active during 1 program cycle (prevents constantly adding/subtracting 1 to the menu number when the joystick is pushed to the side)
bool Flag2 = 0;   // This flag is only active during 1 program cycle (prevents constantly adding/subtracting 2 to the photo number when the joystick is pushed up or down)
bool Flag3 = 0;   // This flag is only active during 1 program cycle (prevents constantly adding/subtracting 1 to the RPM when the joystick is pushed up or down)
bool Flag4 = 0;   // This flag is only active during 1 program cycle (prevents constantly adding/subtracting 1 to the turn number when the joystick is pushed to the side)
bool Flag5 = 0;   // This flag is only active during 1 program cycle (prevents constantly adding/subtracting 1 to the RPM when the joystick is pushed up or down)
bool Flag6 = 0;   // This flag is only active during 1 program cycle to clear the lcd
int SwMenu = 0;   // Switch menu (Sub menu's in the main menu's)
bool BtnFlag = 0; // This flag is only active during 1 program cycle (prevents constantly adding of 1 to SwMenu when button is pressed)


const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
int FullRev = 14336;                  // 1 full revolution of the big gear -> Small-Big gear ratio is 7:1
int rolePerMinute = 15;               // Adjustable range of 28BYJ-48 stepper is 0~17 rpm
int PhotoTaken = 0;                   // Amount of photo's that have been taken
int StepPerPhoto;                     // Amount of steps per photo (calculated -> see MenuNr 0/SwMenu 2)
int TurnNr = 1;                       // Amount of turns
int CurrentTurn = 0;                  // Stores current turn number
int Steps = 0;                        // Amount of individual steps the stepper motor has to turn

Stepper myStepper(stepsPerRevolution, 9, 11, 10, 12);  // Use these pins for the stepper motor

Servo Servo1;  // Define Servo1 as a servo

void setup() {
  
  Serial.begin(115200);
  lcd.begin(16, 2);                   //Lcd setup

  pinMode(SW_pin, INPUT);             //Set pushbutton as input
  digitalWrite(SW_pin, HIGH);         //Set SW_pin High

  myStepper.setSpeed(rolePerMinute);  //Set RPM of steppermotor

  Servo1.attach(3);                   //Attach servo to pin 3
  Servo1.write(90);                   //Move servo to mid possition

  initialSetupJoystick();
    
    lcd.setCursor(4, 0);                //Startup screen start
    lcd.print("Welcome!");              //      """""      //
    lcd.setCursor(1, 1);                //      """""      //
    lcd.print("Software: V1.6");        //      """""      //
    delay(1000);                        //      """""      //
    lcd.clear();                        //      """""      //
    lcd.setCursor(0, 0);                //      """""      //
    lcd.print("Designed by");           //      """""      //
    lcd.setCursor(0, 1);                //      """""      //
    lcd.print("Brian Brocken");         //      """""      //
    delay(1000);                        //      """""      //



  /********************************************************/
    #ifdef LOG_SERIAL
//      Serial.println("Welcome!");              //      """""      //
//      delay(500);
//      Serial.println("Software: V1.6");        //      """""      //
//      delay(1000);                        //      """""      //
//      Serial.println("Designed by");           //      """""      //
//      Serial.println("Brian Brocken");         //      """""      //
//      delay(1000);                        //      """""      //
    #endif
  /********************************************************/

      lcd.clear();                        //Startup screen end
}

void loop() {

  //  int XValue = analogRead(X_pin);     // Read the analog value from The X-axis from the joystick
  //  int YValue = analogRead(Y_pin);     // Read the analog value from The Y-axis from the joystick
  //  int SwValue = digitalRead(SW_pin);  // Read the digital value from The Button from the joystick

  int XValue = analogRead(X_pin);                 // Read the analog value from The X-axis from the joystick
  int YValue = invertReading(analogRead(Y_pin)); // Read the analog value from The Y-axis from the joystick
  int SwValue = digitalRead(SW_pin);              // Read the digital value from The Button from the joystick

  char print_buffer[16]; // variable that will be used to print on LCD
  
  //  Serial.print("XValue = \t");
  //  Serial.print(XValue);
  //  Serial.print("\tYValue = \t");
  //  Serial.print(YValue);
  //  Serial.println("\t");

  if (MenuNr < -1) { //This sets the min number of menu's
    MenuNr = -1;
  }
  else if ( MenuNr > 2) { //This sets the max numbers of menu's
    MenuNr = 2;
  }

  if (XValue > X_RIGHT_THRESHOLD && Flag1 == 0 && SwMenu == 0) { //if the joystick is pushed to the right side and flag1 is 0 then 1 will be added to the menu number (purpose of the flag -> see comment Flags above)
    MenuNr = MenuNr + 1;
    Flag1 = 1;
    lcd.clear();
  }

  if (XValue < X_LEFT_THRESHOLD && Flag1 == 0 && SwMenu == 0) { //if the joystick is pushed to the left side and flag1 is 0 then 1 will be subtracted from the menu number (purpose of the flag -> see comment Flags above)
    MenuNr = MenuNr - 1;
    Flag1 = 1;
    lcd.clear();
  }

  if (checkNeutral(XValue, 'X') && Flag1 == 1) { //if joystick is at neutral possition, flag1 is set to 0 (purpose of the flag -> see comment Flags above)
    Flag1 = 0;
  }


  if (SwValue == 0 && BtnFlag == 0) { //if the button is pressed and the flag is 0 -> add 1 to menu
    SwMenu = SwMenu + 1;
    BtnFlag = 1;
    lcd.clear();
  }

  if (SwValue == 1 && BtnFlag == 1) { //if the button is not pressed and the flag is 0 -> Reset the flag (purpose of the flag -> see comment Flags above)
    BtnFlag = 0;
  }

  //***********************************************Menu -1***********************************************//
  if (MenuNr == -1) { //Menu -1 program

    if (SwMenu == 0){
      lcd.setCursor(0,0);
      lcd.print("Connect to Wi-Fi ");

    #ifdef LOG_SERIAL
      /********************************************************/
      Serial.println("Connect to Wi-Fi");
      /********************************************************/
    #endif
    }
    
    if (SwMenu == 1) { //Menu -1 selected
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  Connecting... ");

      // setupWifi() returns 0 if everything is ok, so we need to invert
      int connectionOK = !setupWifi();
      
      if(connectionOK){ //Send command to ESP so it can connect to Wifi
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   CONNECTED !  ");
        
        delay(2000);
        lcd.clear();
        // next time, go to Menu 0
        SwMenu = 0;
        MenuNr = 0; 
      }
    }
  } // end MenuNr -1
    
   


  //***********************************************Menu0***********************************************//

  if (MenuNr == 0) { //Menu0 program

    if (SwMenu == 0) { //Menu 0 selected
      lcd.setCursor(0, 0);
      lcd.print("Photogrametry");

    #ifdef LOG_SERIAL
      /********************************************************/
      Serial.println("Photogrametry");
      /********************************************************/
    #endif
    }

    if (SwMenu == 1) { //entered menu 0
      lcd.setCursor(0, 0);
      lcd.print("Nr. of photos:");
      lcd.setCursor(7, 1);
      lcd.print(PhotoNr);
      
      
      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.print("Nr. photos: ");
        Serial.println(PhotoNr);
        /********************************************************/
      #endif
      
      if (YValue > Y_UP_THRESHOLD && Flag2 == 0) { //joystick up -> Add 2 to number of photo's
        PhotoNr = PhotoNr + 2;
        Flag2 = 1;
        delay(100);
        lcd.clear();
        Flag2 = 0;
        
      }
      if (YValue < Y_DOWN_THRESHOLD && Flag2 == 0) { //joystick down -> Subtract 2 from number of photo's
        PhotoNr = PhotoNr - 2;
        Flag2 = 1;
        delay(100);
        lcd.clear();
        Flag2 = 0;

      }

      //      if (checkNeutral(YValue, 'Y') && Flag2 == 1){  //if the Y-axis is back at it's neutral possition -> Flag3 = 0 -> Prevents constant adding or subtracting of 2
      //        Flag2 = 0;
      //      }

      if (PhotoNr < 2) {   //Min allowable Nr of photo's
        PhotoNr = 2;
      }
      if (PhotoNr > 200) { //Max allowable Nr of photo's
        PhotoNr = 200;
      }
    }

    if (SwMenu == 2) { //Program started

      lcd.setCursor(0, 0);
      lcd.print("Program started");
      lcd.setCursor(0, 1);
      lcd.print("Photo Nr: ");
      
      lcd.setCursor(10,1);
      sprintf(print_buffer,"%2d/%2d",PhotoTaken, PhotoNr);
      lcd.print(print_buffer);
      

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.println("Program started");
        Serial.print("Photo Nr: ");
        Serial.println(PhotoTaken);
        /********************************************************/
      #endif
      
      StepPerPhoto = FullRev / PhotoNr;  //Calculate amount of steps per photo

      if (PhotoTaken < PhotoNr) {
        myStepper.setSpeed(rolePerMinute);  //Set motor speed
        myStepper.step(StepPerPhoto);       //move the calculated amount of steps
        PhotoTaken = PhotoTaken + 1;        //Add 1 to photos taken
        lcd.setCursor(0, 1);
        lcd.print("Photo Nr: ");            //Taking photo's
        
        lcd.setCursor(10,1);
        sprintf(print_buffer,"%2d/%2d",PhotoTaken, PhotoNr);
        lcd.print(print_buffer);

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.print("Photo Nr: ");
          Serial.println(PhotoTaken);
          /********************************************************/
        #endif

        // Using ESP
        // takePicture() returns 0 if everything is fine. 
        // We invert it to check if pictureOK is true
        int pictureOK = !takePicture();
        if(pictureOK){}
        
        // Using Servo
//        Servo1.write(30);
//        delay(300);
//        Servo1.write(90);
//        delay(1000);
      }

      if (PhotoTaken == PhotoNr) { //If the amount of photos taken is equal to the amount of photos that have to be taken -> Program finished
        lcd.setCursor(0, 0);
        lcd.print("Program finished");

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.println("Program finished");
          /********************************************************/
        #endif

        delay(3000);
        lcd.clear();  //Rest parameters
        PhotoTaken = 0;
        PhotoNr = 2;
        SwMenu = 0;
        Steps = 0;
      }
    }
  }


  //***********************************************Menu1***********************************************//

  if (MenuNr == 1) { //Menu1 program

    if (SwMenu == 0) { //Menu1 selected
      lcd.setCursor(0, 0);
      lcd.print("Cinematic");
      
      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.println("Cinematic");
        /********************************************************/
      #endif
    }

    if (SwMenu == 1) { //Entered menu1 - sub menu1
      lcd.setCursor(0, 0);
      lcd.print("Motor Speed");
      lcd.setCursor(7, 1);
      lcd.print(rolePerMinute);

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.print("Motor Speed: ");
        Serial.println(rolePerMinute);
        /********************************************************/
      #endif

      if (YValue > Y_UP_THRESHOLD && Flag3 == 0) { // joystick up -> Add 1 RPM
        rolePerMinute = rolePerMinute + 1;
        Flag3 = 1;
        lcd.clear();
      }
      if (YValue < Y_DOWN_THRESHOLD && Flag3 == 0) { // joystick down -> Subtract 1 RPM
        rolePerMinute = rolePerMinute - 1;
        Flag3 = 1;
        lcd.clear();
      }
      if (checkNeutral(YValue, 'Y') && Flag3 == 1) { //if the Y-axis is back at it's neutral possition -> Flag3 = 0 -> Prevents constant adding or subtracting of 1
        Flag3 = 0;
      }

      if (rolePerMinute < 1) { //Min allowable RPM
        rolePerMinute = 1;
      }
      if (rolePerMinute > 17) { //Max allowable RPM
        rolePerMinute = 17;
      }
    }

    if (SwMenu == 2) { //Entered menu1 - sub menu2
      lcd.setCursor(0, 0);
      lcd.print("Nr of turns");
      lcd.setCursor(7,1);
      lcd.print(TurnNr);
      

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.print("Nr of turns: ");
        Serial.println(TurnNr);
        /********************************************************/
      #endif

      if (YValue > Y_UP_THRESHOLD && Flag4 == 0) { // joystick up -> Add 1 turn
        TurnNr = TurnNr + 1;
        Flag4 = 1;
        lcd.clear();
      }
      if (YValue < Y_DOWN_THRESHOLD && Flag4 == 0) { // joystick down -> Subtract 1 turn
        TurnNr = TurnNr - 1;
        Flag4 = 1;
        lcd.clear();
      }
      if (checkNeutral(YValue, 'Y') && Flag4 == 1) { //if the Y-axis is back at it's neutral possition -> Flag3 = 0 -> Prevents constant adding or subtracting of 1
        Flag4 = 0;
      }

      if (TurnNr < 1) { //Min allowable amount of turns
        TurnNr = 1;
      }
      if (TurnNr > 200) { //Max allowable amount of turns
        TurnNr = 200;
      }
    }

    if (SwMenu == 3) { //Program started
      lcd.setCursor(0, 0);
      lcd.print("Program started");
      lcd.setCursor(0, 1);
      
      lcd.print("Turns done: ");
      lcd.setCursor(5,1);
      sprintf(print_buffer,"%2d/%2d",CurrentTurn, TurnNr);
      lcd.print(print_buffer);

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.println("Program started");
        Serial.print("Turns done: ");
        Serial.println(CurrentTurn);
        /********************************************************/
      #endif

      if (CurrentTurn < TurnNr) {
        myStepper.setSpeed(rolePerMinute);
        myStepper.step(FullRev);
        CurrentTurn = CurrentTurn + 1;
        lcd.setCursor(0, 1);
        lcd.print("Turns done: ");
        
        lcd.setCursor(5,1);
        sprintf(print_buffer,"%2d/%2d",CurrentTurn, TurnNr);
        lcd.print(print_buffer);

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.print("Turns done: ");
          Serial.println(CurrentTurn);
          /********************************************************/
        #endif
      }

      if (CurrentTurn == TurnNr) { //If the current turn is equal to the amount of thurns that need to be turned -> program finished
        lcd.setCursor(0, 0);
        lcd.print("Program finished");

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.println("Program finished");
          /********************************************************/
        #endif

        delay(3000);
        lcd.clear();  //Reset
        CurrentTurn = 0;
        PhotoNr = 1;
        rolePerMinute = 15;
        SwMenu = 0;
        Steps = 0;
      }

    }
  }

  //***********************************************Menu2***********************************************//

  if (MenuNr == 2) { //Menu2 selected

    if (SwMenu == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Manual control");

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.println("Manual control");
        /********************************************************/
      #endif
    }

    if (SwMenu == 1) { //Entered menu2
      lcd.setCursor(0, 0);
      lcd.print("Motor Speed");
      lcd.setCursor(7, 1);
      lcd.print(rolePerMinute);

      #ifdef LOG_SERIAL
        /********************************************************/
        Serial.print("Motor Speed: ");
        Serial.println(rolePerMinute);
        /********************************************************/
      #endif

      if (YValue > Y_UP_THRESHOLD && Flag5 == 0) { // joystick up -> Add 1 RPM
        rolePerMinute = rolePerMinute + 1;
        Flag5 = 1;
        lcd.clear();
      }
      if (YValue < Y_DOWN_THRESHOLD && Flag5 == 0) { // joystick down -> Subtract 1 RPM
        rolePerMinute = rolePerMinute - 1;
        Flag5 = 1;
        lcd.clear();
      }
      if (checkNeutral(YValue, 'Y') && Flag5 == 1) { //if the Y-axis is back at it's neutral possition -> Flag3 = 0 -> Prevents constant adding or subtracting of 1
        Flag5 = 0;
      }

      if (rolePerMinute < 1) { //Min allowable RPM
        rolePerMinute = 1;
      }
      if (rolePerMinute > 17) { //Max allowable RPM
        rolePerMinute = 17;
      }

      if (XValue > X_RIGHT_THRESHOLD ) { //if the joystick is pushed to the right side and the neutral flag is 0 then 1 will be added to the menu number (purpose of the flag -> see comment Flag1 above)
        Steps = Steps + 1;
        myStepper.setSpeed(rolePerMinute);
        myStepper.step(Steps);
        lcd.setCursor(14, 1);
        lcd.print("->");

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.println("->");
          /********************************************************/
        #endif

        Flag6 = 1;
      }

      if (XValue < X_LEFT_THRESHOLD ) { //if the joystick is pushed to the left side and the neutral flag is 0 then 1 will be subtracted from the menu number (purpose of the flag -> see comment Flag1 above)
        Steps = Steps + 1;
        myStepper.setSpeed(rolePerMinute);
        myStepper.step(-Steps);
        lcd.setCursor(0, 1);
        lcd.print("<-");

        #ifdef LOG_SERIAL
          /********************************************************/
          Serial.println("<-");
          /********************************************************/
        #endif

        Flag6 = 1;
      }

      if (checkNeutral(XValue, 'X')) { //if the Y-axis is back at it's neutral possition -> Flag3 = 0 -> Prevents constant adding or subtracting of 1
        Steps = 0;
        if (Flag6 == 1) {
          lcd.clear();
          Flag6 = 0;  //This flag is only active during 1 program cycle to clear the lcd
        }
      }
    }

    if (SwMenu == 2) { //Leave menu 2 when button is pressed
      lcd.clear();
      CurrentTurn = 0;
      PhotoNr = 1;
      rolePerMinute = 15;
      SwMenu = 0;
      Steps = 0;
    }
  }
}


bool checkNeutral(int value, char option) {

  if (option == 'x' || option == 'X') {
    return (value > X_NEUTRAL_LOW_THRESHOLD && value < X_NEUTRAL_HIGH_THRESHOLD);
  }

  else if (option == 'y' || option == 'Y') {
    return (value > Y_NEUTRAL_LOW_THRESHOLD && value < Y_NEUTRAL_HIGH_THRESHOLD);
  }

  else {
    return false;
  }
}

int invertReading(int value) {
  return (1023 - value);
}

void initialSetupJoystick() {

  int XValue = analogRead(X_pin);
  int YValue = invertReading(analogRead(X_pin));

  X_RIGHT_THRESHOLD = XValue * 1.4;
  X_LEFT_THRESHOLD = XValue * 0.6;

  Y_UP_THRESHOLD = YValue * 1.4;
  Y_DOWN_THRESHOLD = YValue * 0.6;

  X_NEUTRAL_LOW_THRESHOLD = XValue * 0.8;
  X_NEUTRAL_HIGH_THRESHOLD = XValue * 1.2;

  Y_NEUTRAL_LOW_THRESHOLD = YValue * 0.8;
  Y_NEUTRAL_HIGH_THRESHOLD = YValue * 1.2;

}

int setupWifi(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Connecting   ");
  lcd.setCursor(0,1);
  lcd.print("   to Wifi...   ");
  
  /********************************************************/
    #ifdef LOG_SERIAL
      Serial.println("Connecting to Wifi...");
    #endif
  /********************************************************/
  
  String data("");
  
  Serial.println("setupWifi");              // send a request for ESP to connect to Wifi
  
  do{
    while(!Serial.available()>0){}      // wait to receive something
    data = Serial.readStringUntil('\n');// read what has arrived
//    Serial.print("stpWfi()-Arduino received: ");
//    Serial.println(data);
//    Serial.println("setupWifi");              // send it again in case something goes wrong
  }while(!strContains(data.c_str(),"setupWifiOK")); // while we dont receive "setupWifiOK"
  
//  Serial.println("stpWifiOK STRING CMP (arduino) WORKED");
  
  return 0;                                 // zero means everything worked as expected
}

int takePicture(){

  /********************************************************/
    #ifdef LOG_SERIAL
      Serial.println("Taking Picture...");
    #endif
  /********************************************************/
  
  String data("");
  
  Serial.println("takePicture");              // send a request for ESP to take a picture
  
  do{
    while(!Serial.available()>0){}      // wait to receive something
    data = Serial.readStringUntil('\n');// read what has arrived
//    Serial.print("tkPic()-Arduino received: ");
//    Serial.println(data);
//    Serial.println("takePicture");              // send it again in case something goes wrong
  }while(!strContains(data.c_str(),"takePictureOK")); // while we dont receive "takePictureOK"
  
//  Serial.println("tkPicOK STRING CMP (arduino) WORKED");
  
  return 0;                                 // zero means everything worked as expected
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char strContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {return 0;}
    
    while (index < len) {
      if (str[index] == sfind[found]) {
        found++;
        if (strlen(sfind) == found) {return 1;}
      }
      else {found = 0;}
      index++;
    }
    return 0;
}

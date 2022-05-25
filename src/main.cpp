#include <Arduino.h>

/*******************************************************************
 *  Teleporter remote controll
 *
 *  Parts:
 *  ESP 32 dev board
 *  5 buttons
 *  1 switch
 *  3,7V Battery + charger
 *
 *  Written by Rundhall
 *******************************************************************/

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <BleKeyboard.h>
// Source:
// https://github.com/T-vK/ESP32-BLE-Keyboard

// General IO//
#define BUTTON_1_D14 21 // Button 1 MEDIA_PLAY_PAUSE start, stop teleporter
#define BUTTON_2_D18 23 // Button 2 VOLUME_MUTE Scroll up
#define BUTTON_3_D19 14 // Button 3 VOLUME_UP Scroll down
#define BUTTON_4_D21 19 // Button 4 VOLUME_DOWN Increase speed
#define BUTTON_5_D23 18 // Button 5 ESCAPE Decrease speed

//------------------------------------------------------------------------------------------------------
////////////////// SERIAL COM VALUES DECLARATION////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
const byte numChars = 64;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
int integerFromPC = 0;
boolean debug = false;
char recvChar;
char endMarker = 'a';

void parseData();
void recvWithEndMarker1();
void showNewData();

//------------------------------------------------------------------------------------------------------
////////////////// Button handel /////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
int buttonState[] = {0, 0, 0, 0, 0}; /// Button initial
bool buttonfirstpressed[] = {true, true, true, true, true};
int buttonPins[] = {BUTTON_1_D14, BUTTON_2_D18, BUTTON_3_D19, BUTTON_4_D21, BUTTON_5_D23};
// First param is name
// Second is manufacturer
// Third is initial batter level
BleKeyboard bleKeyboard("YakupPult", "Max", 100);

//------------------------------------------------------------------------------------------------------
////////////////// INITIAL SETUP  /////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  bleKeyboard.begin();
  pinMode(BUTTON_1_D14, INPUT_PULLUP);
  pinMode(BUTTON_2_D18, INPUT_PULLUP);
  pinMode(BUTTON_3_D19, INPUT_PULLUP);
  pinMode(BUTTON_4_D21, INPUT_PULLUP);
  pinMode(BUTTON_5_D23, INPUT_PULLUP);
}

//------------------------------------------------------------------------------------------------------
////////////////// MAIN LOOP  //////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
// continuously checks buttons, serial port for any commands
void loop()
{
  //*Serial handel*//
  recvWithEndMarker1(); // Checks serial port messages
  showNewData();

  //*Button handel*//
  for (int i = 0; i < 5; i++)
  {

    buttonState[i] = digitalRead(buttonPins[i]); // Read button.
    if (buttonState[i] == LOW)
    {
      if (buttonfirstpressed[i])
      {
        buttonfirstpressed[i] = false;

        switch (i)
        {
        case 0:
          bleKeyboard.print(" ");
          break;
        case 1:
          bleKeyboard.write(KEY_UP_ARROW);
          break;
        case 2:
          bleKeyboard.write(KEY_DOWN_ARROW);
          break;
        case 3:
          bleKeyboard.write(KEY_LEFT_ARROW);
          break;
        case 4:
          bleKeyboard.write(KEY_RIGHT_ARROW);
          break;
        } // End  switch (integerFromPC)
        delay(50);
        bleKeyboard.releaseAll(); // this releases the buttons
        Serial.println(i);
        delay(50);

      } // End  if(buttonfirstpressed[i])
    }   // End if (buttonState[i] ==LOW)
    else
    {
      buttonfirstpressed[i] = true;
    } // End Else if (buttonState[i] ==LOW)
  }   // End for(int i =0;i<5;i++)

} // End void loop()

//------------------------------------------------------------------------------------------------------
////////////////// SERIAL PORT HANDLE SAVES INCOMING DATA  /////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
void recvWithEndMarker1()
{
  static byte ndx = 0;
  char rc;
  while (Serial.available() > 0 && newData == false)
  { // If there is new data and package is handeled
    rc = Serial.read();
    // Serial.print(rc);
    if (rc != endMarker)
    { // If reads other then end marker it stores in a buffer else a new command has arrived.
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    } // End if (rc != endMarker)
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    } // End if (rc != endMarker) Else
  }   // End while (Serial.available() > 0 && newData == false)
} // End void recvWithEndMarker1() {

//------------------------------------------------------------------------------------------------------
////////////////// SERIAL INCOMING DATA PARSE AND ACT   ///////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
void showNewData()
{
  if (newData == true)
  {
    parseData();
    newData = false;
  }
} // End void showNewData() {

//------------------------------------------------------------------------------------------------------
////////////////// SERIAL PARSE INCOMING DATA   ///////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------
//* A keyboard command can be sent by sending serial command. Like: Firsd button: 1,a  Second button: 2,a etc.*//
void parseData()
{

  // split the data into its parts
  char *strtokIndx;                        // this is used by strtok() as an index
  strtokIndx = strtok(receivedChars, ","); // this continues where the previous call left off
  integerFromPC = atoi(strtokIndx);        // convert this part to an integer
  Serial.print(integerFromPC);             // feedback, that command has arrived.
  switch (integerFromPC)
  {
    // Read all value
  case 1:
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    break;
  case 2:
    bleKeyboard.write(KEY_MEDIA_MUTE);
    break;
  case 3:
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
    break;
  case 4:
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    break;
  case 5:
    bleKeyboard.press(KEY_ESC);
    break;

  } // End  switch (integerFromPC)
  delay(100);
  bleKeyboard.releaseAll(); // this releases the buttons
  Serial.println(":OK");    // feedback, that command has arrived.
} // End void parseData() {

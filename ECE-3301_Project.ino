#include <MagStripe.h>


// Visual feedback when the card is being read...
static const byte READ_LED = 13;
static const byte ERROR_LED = 12;

MagStripe card;

static const byte DATA_BUFFER_LEN = 108;
static char data[DATA_BUFFER_LEN];


void setup()
{
  pinMode(READ_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  
  // The card data will be sent over serial...
  Serial.begin(9600);
  
  // Initialize the library for reading track 2...
  card.begin(1);

  // Start with the feedback LEDs off...
  digitalWrite(READ_LED, LOW);
  digitalWrite(ERROR_LED, LOW);
}

 
void loop()
{
  // Don't do anything if there isn't a card present...
  if (!card.available()) {
    return;
  }
  
  // Show that a card is being read...
  digitalWrite(READ_LED, HIGH);
  
  // Read the card into the buffer "data" (as a null-terminated string)...
  short chars = card.read(data, DATA_BUFFER_LEN);
  
  // Show that the card has finished reading...
  digitalWrite(READ_LED, LOW);
  
  // If there was an error reading the card, blink the error LED...
  if (chars < 0) {
    digitalWrite(ERROR_LED, HIGH);
    delay(250);
    digitalWrite(ERROR_LED, LOW);
    
    Serial.println("ERROR");
    return;
  }

  // Send the data to the computer...
  Serial.println(data);
}



/*
 * Remove Example
 *
 * This sketch shows how to use remove() to delete
 * the file created by the SdFatAppend.pde example.
 */
#include <SdFat.h>
#include <SdFatUtil.h> // use functions to print strings from flash memory

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))
void error_P(const char *str)
{
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode()) {
    PgmPrint("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  while(1);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  PgmPrintln("Type any character to start");
  while (!Serial.available());
  
  // initialize the SD card
  if (!card.init()) error("card.init");
  
  // initialize a FAT volume
  if (!volume.init(card)) error("volume.init");

  // open the root directory
  if (!root.openRoot(volume)) error("openRoot");
  
  char name[] = "APPEND.TXT";
  if (!file.open(root, name, O_WRITE)) {
    PgmPrint("Can't open "); 
    Serial.println(name);
    PgmPrintln("Run the append example to create the file.");
    error("open");
  }
  if (!file.remove()) error("file.remove");
  Serial.print(name);
  PgmPrintln(" deleted.");
}

void loop(void) {}
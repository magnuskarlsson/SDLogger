/*
 * Rewrite Example
 *
 * This sketch shows how to rewrite part of a line in the middle 
 * of the file created by the SdFatAppend.pde example.
 * 
 * Check around line 30 of pass 50 of APPEND.TXT after running this sketch.
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
  // open for read and write
  if (!file.open(root, name, O_RDWR)) {
    PgmPrint("Can't open "); 
    Serial.println(name);
    PgmPrintln("Run the append example to create the file.");
    error("file.open");
  }
  // seek to middle of file
  if (!file.seekSet(file.fileSize()/2)) error("file.seekSet");
  // find end of line
  int16_t c;
  while ((c = file.read()) > 0 && c != '\n');
  if (c < 0) error("file.read");
  // clear write error flag
  file.writeError = false;
  // rewrite the begining of the line at the current position
  file.write("**rewrite**");
  if (file.writeError) error("file.write");
  if (!file.close()) error("file.close");
  Serial.print(name);
  PgmPrintln(" rewrite done.");
}

void loop(void) {}
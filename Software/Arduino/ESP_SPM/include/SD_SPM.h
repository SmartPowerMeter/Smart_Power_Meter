#ifndef SD_SPM
#define SD_SPM

#include "FS.h"
#include "SD.h"

typedef enum{
  SD_OK = 0,
  SD_ABSENT,
  SD_INIT_ERROR,
  SD_TYPE_ERROR,
  SD_SIZE_ERROR
} sd_status;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);


sd_status allCardChecks(fs::SDFS &card, uint8_t gpio_pin);
sd_status checkCardPrecense(uint8_t gpio_pin);
sd_status initCard(fs::SDFS &card);
sd_status checkCardType(fs::SDFS &card);
uint8_t getCardType(fs::SDFS &card);
uint64_t getCardSize(fs::SDFS &card);
String readFileUntilChar(fs::FS &fs, const char * path, char terminator);
bool csvCheckHeader(fs::FS &fs, const char * path);
String csvGetHeader(fs::FS &fs, const char * path);
bool csvRecAppend(File file, double volt, double amp, double power, double energy, double pf, double freq);
bool insertRecord(fs::FS &fs, const char* path, double volt, double amp, double power, double energy, double pf, double freq);

void handleErrorSD(sd_status status);

#endif
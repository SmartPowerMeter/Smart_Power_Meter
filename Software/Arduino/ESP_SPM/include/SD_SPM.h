#ifndef SD_SPM
#define SD_SPM

#include "FS.h"
#include "SD.h"
#include "pzem_004t_SPM.h"

typedef enum{
  SD_OK = 0,
  SD_ABSENT,
  SD_INIT_ERROR,
  SD_TYPE_ERROR,
  SD_SIZE_ERROR,
  SD_WRITE_NOT_ALLOWED,
  SD_FILE_CREATION_ERROR,
  SD_FILE_APPEND_OPEN_ERROR,
  SD_FILE_APPEND_FAILED,
  SD_STR_APPEND_ERROR,
  SD_FILE_WRITE_OPEN_ERROR,
  SD_FILE_WRITE_FAILED
} sd_status;

struct SD_event_flags
{
  uint8_t f_1min;
  uint8_t f_5min;
  uint8_t f_15min;
  uint8_t f_1hour;
  uint16_t cnt;
};

struct SD_1min_mean
{
  float voltage;
  float current;
  float power;
  float energy;
  float frequency;
  float pf;
  uint8_t cnt;
};

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
sd_status writeFile(fs::FS &fs, const char * path, const char * message);
sd_status appendFile(fs::FS &fs, const char * path, const char * message);
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
bool makeDirRecursive(fs::FS &fs, const char *dir);
void formatPzemValuesWithTime(PZEM004Tv30& pzem);
sd_status appendToStr();
void updateSDFlagCounter();
sd_status SDRoutineEverySec();

void handleErrorSD(sd_status status);

#endif
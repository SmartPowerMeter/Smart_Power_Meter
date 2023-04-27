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
  SD_FILE_WRITE_FAILED,
  SD_FILE_PATH_NOT_FOUND,
  SD_FILE_READ_OPEN_ERROR,
  SD_FILE_SEEK_ERROR,
  SD_FILE_READ_BYTES_ERROR,
  SD_RETRIEVE_STR_AT_POS_ERROR,
  SD_SLASH_NOT_FOUND_IN_PATH_ERROR,
  SD_PATH_INFERENCE_ERROR,
  SD_YMD_CONV_ERROR,
  SD_YMD_CONV_FUNC_ERROR,
  SD_STR_TO_VAL_CONV_ERROR,
  SD_NULL_PRT_ERROR,
  SD_FILE_CURSOR_OUT_OF_RANGE_ERROR
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
  struct meas meas;
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
sd_status getStrAtPos(File* file, int pos, char* str, uint8_t length);
sd_status meanLastValsBetween(const char *path, time_t start, time_t end, struct meas* meas, uint8_t maxRewind);
time_t getRecTime(const char *time_str, struct tm ymd);
sd_status getYearMonthDayFromPath(char path[], struct tm *ymd);
sd_status convRecValues(char* rec_str, struct meas* meas_vals);

void handleErrorSD(sd_status status);

#endif
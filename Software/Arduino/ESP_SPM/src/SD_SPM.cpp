#include "SD_SPM.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <time.h> 
#include <WiFi.h>
#include "pzem_004t_SPM.h"
#include "time_SPM.h"
#include "Hardware_SPM.h"

#include "vfs_api.h"



#define ONE_REC_MAX_LEN           60
#define ONE_REC_TRUE_LEN_WTIME    48  // based on format string with time
#define ONE_REC_TRUE_LEN_WOTIME   39  // based on format string with time

char header[] = "Timestamp,voltage,current,power,energy,frequency,pf\n\0";
const char formatStringWithTime[] = "%02d:%02d:%02d,%05.1f,%07.3f,%07.1f,%07.2f,%04.1f,%04.2f\n";
const char formatString[] = "%05.1f,%07.3f,%07.1f,%07.2f,%04.1f,%04.2f\n";
char append_str[3000] = "";

char path[50];
char full_path[50] = "";
time_t t_now;

extern PZEM004Tv30 pzem;
extern bool f_SD_fall_edge;
extern unsigned long SD_insert_start;

uint8_t SD_CARD_WRITE_ENABLE = 1;

// struct SD_event_flags SD_flags = {0};
struct SD_1min_mean meanVals_1min = {0};


// PZEM004Tv30 pzem(Serial2, 16, 17);

// #define RELAY       26

// const char* ssid     = "SHAKO";
// const char* password = "stereometria2001";

// long timezone = 3;
// byte daysavetime = 1;



// HardwareSerial SerialPort(2); // use UART2
// ModbusMaster node;

// struct pzem_data measurements;

sd_status initSDOnInterrupt(){
    if ((f_SD_fall_edge == 1) & (millis() - SD_insert_start >= 1000)){
        // delay(500); // debounce
        if (digitalRead(SD_PRECENCE) == 1) return SD_ABSENT;

        SD.end();
        SD = SDFS(FSImplPtr(new VFSImpl()));
        sd_status ret = initCard(SD);
        if (ret != SD_OK) {
            handleErrorSD(ret);
        };
        f_SD_fall_edge = 0;
    }
    return SD_OK;
}

/* Check following:
    SD Card is inserted
    Initialization successful
    SD card type is viable
    SD card size is in limits
*/
sd_status allCardChecks(fs::SDFS &card, uint8_t gpio_pin){
    if (checkCardPrecense(gpio_pin) != SD_OK) return SD_ABSENT;
    if (initCard(card) != SD_OK) return SD_INIT_ERROR;
    if (checkCardType(card) != SD_OK) return SD_TYPE_ERROR;
    if (getCardSize(card) > 32768) return SD_SIZE_ERROR;

    return SD_OK;
}


/* Check card precence status
*/
sd_status checkCardPrecense(uint8_t gpio_pin){
    pinMode(gpio_pin, INPUT);
    int lvl;
    lvl = digitalRead(gpio_pin);
    if (lvl == 1){
        return SD_ABSENT;
    }

    return SD_OK;
}


/* Initialize SD Card
*/
sd_status initCard(fs::SDFS &card){
    
    if(!card.begin(SD_CS, SPI, 4000000, "/sd", 5, false)){
        return SD_INIT_ERROR;
    }
    Serial.println("SD initialization successfull");
    return SD_OK;
}

/* Check card type
*/
sd_status checkCardType(fs::SDFS &card){
    uint8_t type = card.cardType();
    
    if ((type == CARD_NONE) | (type == CARD_UNKNOWN)){
        return SD_TYPE_ERROR;
    }
    return SD_OK;
}

/* Get card type*/
uint8_t getCardType(fs::SDFS &card){
    return card.cardType();
}

/* Get card size in MB*/
uint64_t getCardSize(fs::SDFS &card){
    return card.cardSize() / (1048576);  // 1048576 = 1024 * 1024
}

struct tm* getSysTime(){
  struct timeval tv_now;
  
  gettimeofday(&tv_now, NULL);
  return localtime(&tv_now.tv_sec);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

String readFileUntilChar(fs::FS &fs, const char * path, char terminator){
    // Serial.printf("Reading file: %s\n", path);
    String line;

    unsigned long startTime;
    unsigned long finishTime;

    startTime = millis();
    File file = fs.open(path);
    finishTime = millis();
    Serial.printf("Open took: %lu milliseconds\n", finishTime - startTime);

    if(!file){
        Serial.println("Failed to open file for reading");
        return "-1";
    }

    // Serial.print("Read from file: ");
    startTime = millis();
    line = file.readStringUntil(terminator);
    finishTime = millis();
    Serial.printf("Read took: %lu milliseconds\n", finishTime - startTime);

    startTime = millis();
    file.close();
    finishTime = millis();
    Serial.printf("Close took: %lu milliseconds\n", finishTime - startTime);

    return line;
}

bool csvCheckHeader(fs::FS &fs, const char * path){
  String curr_line;

  curr_line = readFileUntilChar(fs, path, '\n');
  Serial.println(curr_line);
  if (curr_line+"\n" == String(header)){
    return true;
  }else{
    return false;
  }
}

String csvGetHeader(fs::FS &fs, const char * path){
  return readFileUntilChar(fs, path, '\n');
}

// "Timestamp,voltage,current,power,energy,pf,frequency\n";
bool csvRecAppend(File file, double volt, double amp, double power, double energy, double pf, double freq){
  struct tm * timeinfo;
  timeinfo = getSysTime();

  return file.printf("%d-%02d-%02d %02d:%02d:%02d,%.1f,%.3f,%.1f,%.f,%.2f,%.1f\n", (timeinfo->tm_year)+1900, (timeinfo->tm_mon)+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, volt, amp, power, energy, pf, freq);
}


bool insertRecord(fs::FS &fs, const char* path, double volt, double amp, double power, double energy, double pf, double freq){
  File file = fs.open(path, FILE_APPEND);

  if(!file){
      Serial.println("Failed to open file for appending");
      return false;
  }
  // if (file.readStringUntil('\n')+"\n" != String(header)){
  //   Serial.println("Inconsistent header, should be overwritten before append");
  //   Serial.println(file.readStringUntil('\n'));
  //   return false;
  // }
  if(!csvRecAppend(file, volt, amp, power, energy, pf, freq)){
    Serial.println("Append failed");
    return false;
  }

  file.close();
  return true;
}

// bool readPzem(struct pzem_data* pzem_data){
//   // double volt_pzem, amp_pzem, power_pzem, energy_pzem, freq_pzem, pf_pzem;
//   // uint8_t result;

//   pzem_data->volt = pzem.voltage();
//   pzem_data->amp = pzem.current();
//   pzem_data->power = pzem.power();
//   pzem_data->energy = pzem.energy();
//   pzem_data->freq = pzem.frequency();
//   pzem_data->pf = pzem.pf();

//   return true;

//   // result = node.readInputRegisters(0x0000, 10);
//   // if (result == node.ku8MBSuccess) {
//   //   pzem_data->volt = (node.getResponseBuffer(0x00)/10.0f);
//   //   pzem_data->amp = (node.getResponseBuffer(0x01)/1000.000f);
//   //   pzem_data->power = (node.getResponseBuffer(0x03)/10.0f);
//   //   pzem_data->energy = (node.getResponseBuffer(0x05)/1000.0f);
//   //   pzem_data->freq = (node.getResponseBuffer(0x07)/10.0f);
//   //   pzem_data->pf = (node.getResponseBuffer(0x08)/100.0f);
//   //   // PR_alarm = (node.getResponseBuffer(0x09));
//   //   return true;
//   // }else{
//   //   return false;
//   // }
// }


sd_status writeFile(fs::FS &fs, const char * path, const char * message){

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        return SD_FILE_WRITE_OPEN_ERROR;
    }
    if(!file.print(message)){
        file.close();
        return SD_FILE_WRITE_FAILED;
    }
    file.close();
    return SD_OK;
}

sd_status appendFile(fs::FS &fs, const char * path, const char * message){

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        return SD_FILE_APPEND_OPEN_ERROR;
    }

    if(!file.print(message)){
        file.close();
        return SD_FILE_APPEND_FAILED;
    }
    file.close();
    return SD_OK;
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

bool makeDirRecursive(fs::FS &fs, const char *dir) {
  // split directory path into parent and child directories
  char parentDir[strlen(dir) + 1];
  char childDir[strlen(dir) + 1];
  const char *lastSlash = strrchr(dir, '/');
  if (lastSlash) {
    strncpy(parentDir, dir, lastSlash - dir);
    parentDir[lastSlash - dir] = '\0';
    strcpy(childDir, lastSlash + 1);
  } else {
    strcpy(parentDir, "/");
    strcpy(childDir, dir);
  }

  // create parent directory if it does not exist
  if (!fs.exists(parentDir)) {
    if (!makeDirRecursive(fs, parentDir)) {
      return false;
    }
  }

  // create child directory
//   Serial.println(dir);
  return fs.mkdir(dir);
}

/* This function will return string with current time and values
*/
void formatPzemValuesWithTime(PZEM004Tv30& pzem, char* output){
    sprintf(output, formatStringWithTime,
                        getESPHour24(),
                        getESPMin(),
                        getESPSec(),
                (float) pzem.voltage(),
                (float) pzem.current(),
                (float) pzem.power(),
                (float) pzem.energy(),
                (float) pzem.frequency(),
                (float) pzem.pf());
}

void formatMeasValuesWithTime(char* output, struct meas* meas, time_t time){
    struct tm dt; 
    localtime_r(&time, &dt);
    // Serial.printf("Pointer to dt: %d\n", dt);
    sprintf(output, formatStringWithTime,
                        dt.tm_hour,
                        dt.tm_min,
                        dt.tm_sec,
                        meas->voltage,
                        meas->current,
                        meas->power,
                        meas->energy,
                        meas->frequency,
                        meas->pf);
    // Serial.printf("\nvoltage: %f\n", meas->voltage);
    // Serial.printf("Current: %f\n", meas->current);
    // Serial.printf("power: %f\n", meas->power);
    // Serial.printf("energy: %f\n", meas->energy);
    // Serial.printf("frequency: %f\n", meas->frequency);
    // Serial.printf("pf: %f\n\n", meas->pf);
    // Serial.printf("Rec str: %s\n", output);
}

/* This function should be called after taking measurements
   It will append to the string in flash memory which then
   will be saved into file in every 1 minute
*/
sd_status appendToStr(){
    char curr_vals[ONE_REC_MAX_LEN] = "";
    formatPzemValuesWithTime(pzem, curr_vals);
    if(strcat(append_str, curr_vals) == nullptr){
        return SD_STR_APPEND_ERROR;
    };
    return SD_OK;
}

void accumulateMean_1min(){
    meanVals_1min.meas.voltage   = (meanVals_1min.meas.voltage   * meanVals_1min.cnt + (float)pzem.voltage())  /(meanVals_1min.cnt + 1);
    meanVals_1min.meas.current   = (meanVals_1min.meas.current   * meanVals_1min.cnt + (float)pzem.current())  /(meanVals_1min.cnt + 1);
    meanVals_1min.meas.power     = (meanVals_1min.meas.power     * meanVals_1min.cnt + (float)pzem.power())    /(meanVals_1min.cnt + 1);
    meanVals_1min.meas.energy    = (meanVals_1min.meas.energy    * meanVals_1min.cnt + (float)pzem.energy())   /(meanVals_1min.cnt + 1);
    meanVals_1min.meas.frequency = (meanVals_1min.meas.frequency * meanVals_1min.cnt + (float)pzem.frequency())/(meanVals_1min.cnt + 1);
    meanVals_1min.meas.pf        = (meanVals_1min.meas.pf        * meanVals_1min.cnt + (float)pzem.pf())       /(meanVals_1min.cnt + 1);
    meanVals_1min.cnt++;
}

/* This function handles writing to a files
   Every second it will append record to a string in flash memory
   If 1  minute has passed it will write to s.txt
   On multiples of    minute  it will write to m.txt
   On multiples of 5  minutes it will write to 5m.txt
   On multiples of 15 minutes it will write to 15m.txt
   On multiples of 1  hour    it will write to h.txt
*/
sd_status SDRoutineEverySec(){
    if (!SD_CARD_WRITE_ENABLE){
        return SD_WRITE_NOT_ALLOWED;
    }

    sd_status ret = appendToStr();
    if(ret != SD_OK) return ret;

    accumulateMean_1min();  // for m.txt
    // updateSDFlagCounter();
    
    // if(SD_flags.f_1min == 1){
    //     SD_flags.f_1min = 0;

    //     ret = checkCardPrecense(SD_PRECENCE);
    //     if (ret != SD_OK) {
    //         memset(append_str, 0, sizeof(append_str));
    //         return ret;
    //     }

    //     strcpy(full_path, path);
    //     strcat(full_path, "/s.txt");
    //     if(!SD.exists(full_path)){
    //         if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
    //     }

    //     ret = appendFile(SD, full_path, append_str);
    //     if(ret != SD_OK) return ret;
    //     // Serial.printf("Appended to s.txt\n%s\n", append_str);
    //     memset(append_str, 0, sizeof(append_str));

    //     return SD_OK;
    // }
    t_now = time(nullptr);
    struct tm tm_now;
    localtime_r(&t_now, &tm_now);

    // at the end of every minute
    if (tm_now.tm_sec == 59){
        ret = checkCardPrecense(SD_PRECENCE);
        if (ret != SD_OK) {
            memset(append_str, 0, sizeof(append_str));  // clear string if SD card is not inserted
            memset(&meanVals_1min, 0, sizeof(meanVals_1min)); // clear accumulate struct for m.txt
            return ret;
        }
        tm_now.tm_sec = 0; 
        t_now = mktime(&tm_now);
        tm_now.tm_year = tm_now.tm_year + 1900;
        tm_now.tm_mon = tm_now.tm_mon + 1;

        sprintf(path, "/SPM_DATA/%04d/%02d/%02d",
                                            tm_now.tm_year,
                                            tm_now.tm_mon,
                                            tm_now.tm_mday);

        // add to s.txt
        strcpy(full_path, path);
        strcat(full_path, "/s.txt");
        if(!SD.exists(full_path)){
            if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
        }

        ret = appendFile(SD, full_path, append_str);
        if(ret != SD_OK) return ret;
        // Serial.printf("Appended to s.txt\n%s\n", append_str);
        memset(append_str, 0, sizeof(append_str));


        // add to m.txt
        char data[ONE_REC_MAX_LEN] = "";
        formatMeasValuesWithTime(data,  &meanVals_1min.meas, t_now);
        strcpy(full_path, path);
        strcat(full_path, "/m.txt");
        if(!SD.exists(full_path)){
            if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
        }
        ret = appendFile(SD, full_path, data);
        if(ret != SD_OK) return ret;
        memset(&meanVals_1min, 0, sizeof(meanVals_1min));
        // Serial.printf("Appended to m.txt\n%s\n", data);


        // 5 min
        if ((tm_now.tm_min+1) % 5 == 0){
            // Serial.println("-------------------> Into 5 minute");
            // char data[ONE_REC_MAX_LEN] = "";
            // Serial.printf("1.tm_min = %d\n", tm_now.tm_min);
            time_t start, end;
            end = t_now;
            start = end - 240; // 4*60
            // Serial.printf("2.tm_min = %d\n", tm_now.tm_min);

            struct meas meas = {0};
            strcpy(full_path, path);
            strcat(full_path, "/m.txt");
            ret = meanLastValsBetween(full_path, start, end, &meas, 7);
            if(ret != SD_OK){
                handleErrorSD(ret);
                goto func_end;
            }
            // Serial.printf("3.tm_min = %d\n", tm_now.tm_min);
            // Serial.print("Before formatting:\n");
            // Serial.printf("voltage: %f\n", meas.voltage);
            // Serial.printf("current: %f\n", meas.current);
            // Serial.printf("power: %f\n", meas.power);
            // Serial.printf("energy: %f\n", meas.energy);
            // Serial.printf("frequency: %f\n", meas.frequency);
            // Serial.printf("pf: %f\n", meas.pf);
            // Serial.printf("Pointer to tm_now: %d\n", tm_now);
            formatMeasValuesWithTime(data,  &meas, start);
            // Serial.printf("4.tm_min = %d\n", tm_now.tm_min);
            // Serial.printf("formatted: %s\n", data);

            strcpy(full_path, path);
            strcat(full_path, "/5m.txt");
            if(!SD.exists(full_path)){
                if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
            }
            ret = appendFile(SD, full_path, data);
            if(ret != SD_OK) return ret;
            // Serial.printf("5m appended data: %s\n", data);
            // Serial.printf("Appended to 5m.txt\n%s\n", data);
            // Serial.printf("5.tm_min = %d\n", tm_now.tm_min);
            // 15 min
            if ((tm_now.tm_min+1) % 15 == 0){
                // Serial.println("-------------------> Into 15 minute");
                // char data[ONE_REC_MAX_LEN] = "";
                // time_t start, end;
                // end = time(nullptr);
                start = end - 840; // 60*14

                // struct meas meas;
                strcpy(full_path, path);
                strcat(full_path, "/5m.txt");
                ret = meanLastValsBetween(full_path, start, end, &meas, 5);
                if(ret != SD_OK){
                    handleErrorSD(ret);
                    goto func_end;
                }
                formatMeasValuesWithTime(data,  &meas, start);

                strcpy(full_path, path);
                strcat(full_path, "/15m.txt");
                if(!SD.exists(full_path)){
                    if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
                }
                ret = appendFile(SD, full_path, data);
                if(ret != SD_OK) return ret;
                // Serial.printf("Appended to 15m.txt\n%s\n", data);
                // Serial.printf("tm_min = %d\n", tm_now.tm_min);
                // 1 hour
                if (tm_now.tm_min == 59){
                    // Serial.println("-------------------> Into 1 hour");
                    // char data[ONE_REC_MAX_LEN] = "";
                    // time_t start, end;
                    // end = time(nullptr);
                    start = end - 3540; // 60*59

                    // struct meas meas;
                    strcpy(full_path, path);
                    strcat(full_path, "/15m.txt");
                    ret = meanLastValsBetween(full_path, start, end, &meas, 7);
                    if(ret != SD_OK){
                        handleErrorSD(ret);
                        goto func_end;
                    }
                    formatMeasValuesWithTime(data,  &meas, start);

                    strcpy(full_path, path);
                    strcat(full_path, "/h.txt");
                    if(!SD.exists(full_path)){
                        if(!makeDirRecursive(SD, path)) return SD_FILE_CREATION_ERROR;
                    }
                    ret = appendFile(SD, full_path, data);
                    if(ret != SD_OK) return ret;
                    // Serial.printf("Appended to h.txt\n%s\n", data);
                    // Serial.printf("tm_min = %d\n", tm_now.tm_min);
                }
            }
        }
    }

    func_end:
    return SD_OK;
}


/* Read fixed length string from opened file from specified cursor position
   Leaves coursor to the end of the string in file
*/
sd_status getStrAtPos(File* file, int pos, char* str, uint8_t length){

    if(!file->seek(pos)) return SD_FILE_SEEK_ERROR;
    file->read((uint8_t*)str, (size_t)length);
    // int len = 
    // Serial.printf("str from file: %s\n", str);
    // Serial.print("return length: ");
    // Serial.println(len);
    // Serial.print("argument length: ");
    // Serial.println(length);
    // if(len != (int)length) return SD_FILE_READ_BYTES_ERROR;
    str[length] = '\0';
    // if(!file->seek(pos)) return SD_FILE_SEEK_ERROR;

    return SD_OK;
}

/* Return mean values read from the end of file
   maxRewind is parameter for maximum number of records it goes from end to the begining of file
*/
sd_status meanLastValsBetween(const char *path, time_t start, time_t end, struct meas* meas, uint8_t maxRewind){
    
    if(!SD.exists(path)){
        return SD_FILE_PATH_NOT_FOUND;
    }

    File file = SD.open(path);
    if(!file){
        return SD_FILE_READ_OPEN_ERROR;
    }

    if(!file.seek(0, SeekEnd)) return SD_FILE_SEEK_ERROR;
    int cursor = file.position();

    struct tm ymd = {0};
    sd_status ret = getYearMonthDayFromPath((char *)path, &ymd);
    if(ret != SD_OK) return ret;

    char rec[ONE_REC_MAX_LEN] = "";
    // char rec_copy[ONE_REC_MAX_LEN] = "";
    char *p1, *p2;
    struct meas rec_vals = {0};
    struct meas mean_vals = {0};
    uint16_t rec_cnt = 0;
    time_t rec_t;

    for(int i = 0; i < maxRewind; i++){
        // Serial.println("begin for");
        cursor = cursor - ONE_REC_TRUE_LEN_WTIME - 1;
        if (cursor < 0) goto copy_mean_vals_and_exit;
        // Serial.printf("Cursor pos: %d\n", cursor);
        ret = getStrAtPos(&file, (cursor == 0) ? 0 : (cursor-1), rec, ONE_REC_TRUE_LEN_WTIME+1);
        if(ret != SD_OK) return ret;

        // strcpy(rec_copy, rec);
        if(cursor == 0) p1 = rec;
        else p1 = strchr(rec, '\n') + 1;

        // Serial.printf("============>> For i == %d\n", i);
        // Serial.printf("Rec: %s\n", rec);
        // Serial.printf("Recptr: %d\n", rec_copy);
        // Serial.printf("p1: %d\n", p1);
        if(p1 != nullptr){
            p2 = strchr(p1, ',');
            // Serial.printf("p2: %d\n", p2);
            if((p2 != nullptr) & (p2-p1==8)){
                *p2 = '\0';
                rec_t = getRecTime(p1, ymd);

                // Serial.printf("Record time: %d -> %s\n", rec_t, ctime(&rec_t));
                // Serial.printf("Start time:  %d -> %s\n", start, ctime(&start));
                // Serial.printf("End time:    %d -> %s\n", end, ctime(&end));

                if((difftime(start, rec_t) > 0) | (difftime(rec_t, end) > 0)) continue;
                // Serial.println(" ==================== > Time cond satisfied");

                *(p2+1+ONE_REC_TRUE_LEN_WOTIME) = '\0';
                // Serial.printf("meas_str: %s\n", p2+1);
                ret = convRecValues((p2+1), &rec_vals);
                if (ret != SD_OK) return ret;

                // Serial.println("Rec_vals: ");
                // Serial.printf("voltage: %f\n", rec_vals.voltage);
                // Serial.printf("current: %f\n", rec_vals.current);
                // Serial.printf("power: %f\n", rec_vals.power);
                // Serial.printf("energy: %f\n", rec_vals.energy);
                // Serial.printf("frequency: %f\n", rec_vals.frequency);
                // Serial.printf("pf: %f\n", rec_vals.pf);
                mean_vals.voltage   = ((mean_vals.voltage   * rec_cnt) + rec_vals.voltage)  /(rec_cnt+1);
                mean_vals.current   = ((mean_vals.current   * rec_cnt) + rec_vals.current)  /(rec_cnt+1);
                mean_vals.power     = ((mean_vals.power     * rec_cnt) + rec_vals.power)    /(rec_cnt+1);
                mean_vals.energy    = ((mean_vals.energy    * rec_cnt) + rec_vals.energy)   /(rec_cnt+1);
                mean_vals.frequency = ((mean_vals.frequency * rec_cnt) + rec_vals.frequency)/(rec_cnt+1);
                mean_vals.pf        = ((mean_vals.pf        * rec_cnt) + rec_vals.pf)       /(rec_cnt+1);
                rec_cnt = rec_cnt + 1;
            }else return SD_NULL_PRT_ERROR;
        }else return SD_NULL_PRT_ERROR;

        // Serial.printf("voltage: %f\n", mean_vals.voltage);
        // Serial.printf("current: %f\n", mean_vals.current);
        // Serial.printf("power: %f\n", mean_vals.power);
        // Serial.printf("energy: %f\n", mean_vals.energy);
        // Serial.printf("frequency: %f\n", mean_vals.frequency);
        // Serial.printf("pf: %f\n", mean_vals.pf);

        // Serial.println("End For");
    }

    copy_mean_vals_and_exit:
    meas->voltage   = mean_vals.voltage;
    meas->current   = mean_vals.current;
    meas->power     = mean_vals.power;
    meas->energy    = mean_vals.energy;
    meas->frequency = mean_vals.frequency;
    meas->pf        = mean_vals.pf;

    return SD_OK;
}


sd_status convRecValues(char* rec_str, struct meas* meas_vals){
    uint8_t n = sscanf(rec_str, "%f,%f,%f,%f,%f,%f\n", &meas_vals->voltage,
                                                       &meas_vals->current,
                                                       &meas_vals->power,
                                                       &meas_vals->energy,
                                                       &meas_vals->frequency,
                                                       &meas_vals->pf);
    if (n != 6) return SD_STR_TO_VAL_CONV_ERROR;

    return SD_OK;
}

/* Get time_t value of record
   time_str is HH:MI:SS type string
   ymd is struct tm object in which year, month and day should be present
*/
time_t getRecTime(const char *time_str, struct tm ymd){
    char tmp[3] = "";

    // Serial.println("In getRecTime()");

    strncpy(tmp, time_str, 2); tmp[2] = '\0';
    ymd.tm_hour = atoi(tmp);
    strncpy(tmp, time_str+3, 2); tmp[2] = '\0';
    ymd.tm_min = atoi(tmp);
    strncpy(tmp, time_str+6, 2); tmp[2] = '\0';
    ymd.tm_sec = atoi(tmp);

    // Serial.printf("Year: %d\n", ymd.tm_year);
    // Serial.printf("Month: %d\n", ymd.tm_mon);
    // Serial.printf("Day: %d\n", ymd.tm_mday);
    // Serial.printf("Hour: %d\n", ymd.tm_hour);
    // Serial.printf("Min:  %d\n", ymd.tm_min);
    // Serial.printf("Sec:  %d\n", ymd.tm_sec);

    time_t t1 = mktime(&ymd);
    // Serial.print("Record time: ");
    // Serial.println(ctime(&t1));
    return t1;
}

sd_status getYearMonthDayFromPath(char path[], struct tm *ymd){
    char *p1, *end;
    p1 = strchr(path, '/');
    if(p1 == nullptr) return SD_SLASH_NOT_FOUND_IN_PATH_ERROR;

    end = path + strlen(path);
    p1 = end;
    uint8_t tmp_cnt = 0;
    while(tmp_cnt != 4){
        if(end - p1 > 30) return SD_PATH_INFERENCE_ERROR;
        if(*p1 == '/') tmp_cnt++;
        p1 = p1 - 1;
    }
    p1 = p1 + 2;
    char year[5], month[3], day[3];
    strncpy(year, p1, 4); year[4] = '\0';
    p1 = p1 + 5;
    strncpy(month, p1, 2); month[2] = '\0';
    p1 = p1 + 3;
    strncpy(day, p1, 2); day[2] = '\0';
    
    // Serial.printf("year: %s, month: %s, day: %s\n", year, month, day);
    ymd->tm_year = atoi(year);
    ymd->tm_mon  = atoi(month);
    ymd->tm_mday = atoi(day);
    // if((ymd->tm_year == 0) | (ymd->tm_mon == 0) | (ymd->tm_mday == 0)) return SD_YMD_CONV_ERROR;

    ymd->tm_year = ymd->tm_year - 1900;
    ymd->tm_mon = ymd->tm_mon - 1;

    return SD_OK;
}


// void updateSDFlagCounter(){
//     if(SD_flags.cnt >= 3600){
//         SD_flags.cnt = 0;
//         SD_flags.f_1hour = 1;
//     }else{
//         SD_flags.cnt++;
//         if(SD_flags.cnt % 60 == 0)   SD_flags.f_1min = 1;
//         if(SD_flags.cnt % 300 == 0)  SD_flags.f_5min = 1;
//         if(SD_flags.cnt % 900 == 0)  SD_flags.f_15min = 1;
//     }
// }

void handleErrorSD(sd_status status){
    Serial.print("!!!!!!!!! Error: ");
    if(status == SD_ABSENT){
        Serial.println("SD Card is not inserted");
    }else if (status == SD_INIT_ERROR){
        Serial.println("SD Card initialization failed");
    }else if (status == SD_TYPE_ERROR){
        Serial.println("SD Card type not supported");
    }else if (status == SD_SIZE_ERROR)
    {
        Serial.println("SD Card size error");
    }else if (status == SD_FILE_CREATION_ERROR)
    {
        Serial.println("Could not create file");
    }else if (status == SD_FILE_APPEND_OPEN_ERROR)
    {
        Serial.println("Could not open file for append");
    }else if (status == SD_FILE_APPEND_FAILED)
    {
        Serial.println("Could not append to a file");
    }else if (status == SD_STR_APPEND_ERROR)
    {
        Serial.println("Could not append to a string in flash memory");
    }else if (status == SD_FILE_WRITE_OPEN_ERROR)
    {
        Serial.println("Could not open file for write");
    }else if (status == SD_FILE_WRITE_FAILED)
    {
        Serial.println("Could not write to a file");
    }else if (status == SD_FILE_PATH_NOT_FOUND)
    {
        Serial.println("File on specified path was not found");
    }else if (status == SD_FILE_READ_OPEN_ERROR)
    {
        Serial.println("Could not open file for read");
    }else if (status == SD_FILE_SEEK_ERROR)
    {
        Serial.println("Could not position cursor to a specified location");
    }else if (status == SD_FILE_READ_BYTES_ERROR)
    {
        Serial.println("Error in reading bytes from file");
    }else if (status == SD_RETRIEVE_STR_AT_POS_ERROR)
    {
        Serial.println("Error retrieving record from file");
    }else if (status == SD_SLASH_NOT_FOUND_IN_PATH_ERROR)
    {
        Serial.println("Slash not found in path");
    }else if (status == SD_PATH_INFERENCE_ERROR)
    {
        Serial.println("Path could not be inferred");
    }else if (status == SD_YMD_CONV_ERROR)
    {
        Serial.println("Error occured during year, month or day conversion from string to int");
    }else if (status == SD_YMD_CONV_FUNC_ERROR)
    {
        Serial.println("getYearMonthDayFromPath() function errored");
    }else if (status == SD_STR_TO_VAL_CONV_ERROR)
    {
        Serial.println("Error in getting 6 measurement values from string");
    }else if (status == SD_NULL_PRT_ERROR)
    {
        Serial.println("Got nullptr where was not expected");
    }else if (status == SD_FILE_CURSOR_OUT_OF_RANGE_ERROR)
    {
        Serial.println("Crusor out of range when reading from file");
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}

// void setup(){
//   // delay(10000);
//   pinMode(RELAY, OUTPUT);
//   digitalWrite(RELAY, LOW);

//   unsigned long time, time1;
//   String curr_line;

//     Serial.begin(115200);
//     if(!SD.begin()){
//         Serial.println("Card Mount Failed");
//         return;
//     }
//     uint8_t cardType = SD.cardType();

//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }

//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     // SerialPort.begin(9600, SERIAL_8N1, 16, 17);
//     // node.begin(1, SerialPort);

//     configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
//     struct tm tmstruct;
//     tmstruct.tm_year = 0;
//     getLocalTime(&tmstruct, 10000);
//     time = micros();
// 	  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
//     time1 = micros();
//     Serial.printf("Time took: %ul\n", time1-time);

//     // createDir(SD, "/data");
//     // writeFile(SD, "/data/log_27_Mar_2023.csv", header);

//     // char* file_path = "/data/log_27_Mar_2023.csv";
//     // if (!csvCheckHeader(file_path)){
//     //   writeFile(SD, file_path, header);
//     // }

//     // struct timeval tv_now;
//     // gettimeofday(&tv_now, NULL);
//     // Serial.printf("\nSeconds: %ul\n", tv_now.tv_sec);



// // "Timestamp,voltage,current,power,energy,pf,frequency\n";
    


//     // File file = SD.open("/data/log_27_Mar_2023.csv");
//     // if(!file){
//     //     Serial.println("Failed to open file for reading");
//     //     return;
//     // }

//     // Serial.print("Read from file: ");
//     // // while(file.available()){
//     //   String line = file.readStringUntil('\n');
//     //   Serial.println(line);
//     // // }
  
//     // file.close();


//     // Serial.print("SD Card Type: ");
//     // if(cardType == CARD_MMC){
//     //     Serial.println("MMC");
//     // } else if(cardType == CARD_SD){
//     //     Serial.println("SDSC");
//     // } else if(cardType == CARD_SDHC){
//     //     Serial.println("SDHC");
//     // } else {
//     //     Serial.println("UNKNOWN");
//     // }

//     // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//     // Serial.printf("SD Card Size: %lluMB\n", cardSize);

//     // listDir(SD, "/", 0);
//     // createDir(SD, "/mydir");
//     // listDir(SD, "/", 0);
//     // removeDir(SD, "/mydir");
//     // listDir(SD, "/", 2);
//     // writeFile(SD, "/hello.txt", "Hello ");
//     // appendFile(SD, "/hello.txt", "World!\n");
//     // readFile(SD, "/hello.txt");
//     // deleteFile(SD, "/foo.txt");
//     // renameFile(SD, "/hello.txt", "/foo.txt");
//     // readFile(SD, "/foo.txt");
//     // testFileIO(SD, "/test.txt");
//     // Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
//     // Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
// }

// void loop(){
//   // readPzem(&measurements);
//   if (readPzem(&measurements)){
//     insertRecord("/data/log_27_Mar_2023.csv", measurements.volt, measurements.amp, measurements.power, measurements.energy, measurements.pf, measurements.freq);
//     Serial.printf("%.1f,%.3f,%.1f,%.f,%.2f,%.1f\n", measurements.volt, measurements.amp, measurements.power, measurements.energy, measurements.pf, measurements.freq);
//   }

//   delay(1000);
// }

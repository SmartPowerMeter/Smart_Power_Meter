#include "SD_SPM.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <time.h> 
#include <WiFi.h>
#include <PZEM004Tv30.h>

char header[] = "Timestamp,voltage,current,power,energy,pf,frequency\n\0";

// PZEM004Tv30 pzem(Serial2, 16, 17);

// #define RELAY       26

// const char* ssid     = "SHAKO";
// const char* password = "stereometria2001";

// long timezone = 3;
// byte daysavetime = 1;



// HardwareSerial SerialPort(2); // use UART2
// ModbusMaster node;

// struct pzem_data measurements;

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
    if(!card.begin()){
        return SD_INIT_ERROR;
    }
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


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
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

void handleErrorSD(sd_status status){
    if(status == SD_ABSENT){
        Serial.println("SD Card is not inserted");
    }else if (status == SD_INIT_ERROR){
        Serial.println("SD Card initialization failed");
    }else if (status == SD_TYPE_ERROR){
        Serial.println("SD Card type not supported");
    }else if (status == SD_SIZE_ERROR)
    {
        Serial.println("SD Card size error");
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

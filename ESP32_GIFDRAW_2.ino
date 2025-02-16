#define NORMAL_SPEED
#define SERIALDEBUG false

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>

#define SD_CS_PIN 5

TFT_eSPI tft = TFT_eSPI();
AnimatedGIF gif;

String gifFiles[] = {"/1.gif"};
int gifIndex = 0;

void setup() {
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  if (!SD.begin(SD_CS_PIN, tft.getSPIinstance())) {
    Serial.println("Error: Unable to initialize SD card!");
    while (true);
  }

  if (SERIALDEBUG) {
    Serial.println("SD card initialized successfully.");
  }

  gif.begin(BIG_ENDIAN_PIXELS);
}

void loop() {

  if (gif.open(gifFiles[gifIndex].c_str(), fileOpen, fileClose, fileRead, fileSeek, GIFDraw)) {
    if (SERIALDEBUG) {
      Serial.print("SUCCESS gif.open: ");
      Serial.println(gifFiles[gifIndex].c_str());
      Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    }

    while (gif.playFrame(true, NULL)) {
      yield();
    }
    gif.close();
  } else {
    Serial.print("FAIL gif.open: ");
    Serial.println(gifFiles[gifIndex].c_str());
    while(true);
  }
}

void* fileOpen(const char* filename, int32_t* pFileSize) {
  File* f = new File();
  *f = SD.open(filename, FILE_READ);
  if (*f) {
    *pFileSize = f->size();
    
    if (SERIALDEBUG) {
      Serial.print("SUCCESS - fileopen: ");
      Serial.println(filename);
    }

    return (void*)f;
  } else {
    Serial.print("FAIL - fileopen: ");
    Serial.println(filename);
    delete f;
    return NULL;
  }
}

void fileClose(void* pHandle) {
  if (SERIALDEBUG) {
    Serial.println("ENTER - fileClose");
  }

  File* f = static_cast<File*>(pHandle);
  if (f != NULL) {
    f->close();
    delete f;
  }
}

int32_t fileRead(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen) {
  if (SERIALDEBUG) {
    Serial.println("ENTER - fileRead");
  }

  File* f = static_cast<File*>(pFile->fHandle);
  if (f == NULL) {
    Serial.println("f == NULL");
    return 0;
  }
  if (SERIALDEBUG) {
    Serial.print("File* f= ");
    Serial.print(reinterpret_cast<uintptr_t> (f));
    Serial.print("\nCalling f->read");
    Serial.print("\npBuf: ");
    Serial.print(reinterpret_cast<uintptr_t> (pBuf));
    Serial.print("\niLen: ");
    Serial.print(iLen, HEX);
  }

  int32_t iBytesRead = f->read(pBuf, iLen);
  pFile->iPos = f->position();
  
  if (SERIALDEBUG) {
    Serial.print("iBytesRead = ");
    Serial.println(iBytesRead);
    Serial.print("iPos = ");
    Serial.println(pFile->iPos);
  }

  return iBytesRead;
}

int32_t fileSeek(GIFFILE* pFile, int32_t iPosition) {
  if (SERIALDEBUG) {
    Serial.println("ENTER - fileSeek");
  }
  File* f = static_cast<File*>(pFile->fHandle);
  if (f == NULL) return 0;
  f->seek(iPosition);
  pFile->iPos = f->position();

  if (SERIALDEBUG) {
    Serial.print("iPos = ");
    Serial.println(pFile->iPos);
  }

  return pFile->iPos;
}

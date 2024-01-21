#ifndef ISPRITEDTTEXTOBSERVER_H
#define ISPRITEDTTEXTOBSERVER_H

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

class ISpritedTextObserver 
{
 public:
  virtual ~ISpritedTextObserver(){};

  virtual void PrintText() = 0;
  virtual void ForcePrintText() = 0;
  virtual void CreateSprite() = 0;
  virtual void setSpriteBackground(void* ScreenPtr) = 0;

};

#endif
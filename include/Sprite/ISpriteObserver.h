#ifndef ISPRITEOBSERVER_H
#define ISPRITEOBSERVER_H

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

class ISpriteObserver 
{
 public:
  virtual ~ISpriteObserver(){};

  virtual void PrintSprite()                          = 0;
  virtual void ForcePrintSprite()                     = 0;
  virtual void CreateSprite()                         = 0;
  virtual void setSpriteBackground(TFT_eSPI* pScreen) = 0;

};

#endif
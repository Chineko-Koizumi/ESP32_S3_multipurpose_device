#ifndef ISPRITESUBJECT_H
#define ISPRITESUBJECT_H

#include "ISpriteObserver.h"

class ISpriteSubject 
{
 public:
  virtual ~ISpriteSubject(){};
  virtual void Attach(ISpriteObserver *observer) = 0;
  virtual void Detach(ISpriteObserver *observer) = 0;

  virtual void NotifyPrintSprite() = 0;
  virtual void NotifyForcePrintSprite() = 0;
  virtual void NotifyCreateSprite() = 0;
  virtual void NotifysetSpriteBackground(TFT_eSPI* pScreen) = 0;
};

#endif
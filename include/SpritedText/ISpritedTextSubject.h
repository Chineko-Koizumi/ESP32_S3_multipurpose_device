#ifndef ISPRITEDTTEXTSUBJECT_H
#define ISPRITEDTTEXTSUBJECT_H

#include "ISpritedTextObserver.h"

class ISpritedTextSubject 
{
 public:
  virtual ~ISpritedTextSubject(){};
  virtual void Attach(ISpritedTextObserver *observer) = 0;
  virtual void Detach(ISpritedTextObserver *observer) = 0;

  virtual void NotifyPrintText() = 0;
  virtual void NotifyForcePrintText() = 0;
  virtual void NotifyCreateSprite() = 0;
  virtual void NotifysetSpriteBackground(void* ScreenPtr) = 0;
};

#endif
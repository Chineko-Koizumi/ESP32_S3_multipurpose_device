#ifndef SPRITESUBJECT_H
#define SPRITESUBJECT_H

#include <list>

#include "ISpriteSubject.h"


class SpriteSubject : public ISpriteSubject 
{
 public:
  virtual ~SpriteSubject();

  /**
   * The subscription management methods.
   */
  void Attach(ISpriteObserver *observer) override;
  void Detach(ISpriteObserver *observer) override;

  void NotifyPrintSprite()                          override;
  void NotifyForcePrintSprite()                     override;
  void NotifyCreateSprite()                         override;
  void NotifysetSpriteBackground(TFT_eSPI* pScreen) override;

 private:
  std::list<ISpriteObserver *> list_observer_;
};

#endif
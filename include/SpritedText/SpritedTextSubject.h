#include <list>

#include "ISpritedTextSubject.h"


class SpritedTextSubject : public ISpritedTextSubject 
{
 public:
  virtual ~SpritedTextSubject();

  /**
   * The subscription management methods.
   */
  void Attach(ISpritedTextObserver *observer) override;
  void Detach(ISpritedTextObserver *observer) override;

  void NotifyPrintText()                              override;
  void NotifyForcePrintText()                         override;
  void NotifyCreateSprite()                           override;
  void NotifysetSpriteBackground(void* ScreenPtr)     override;

 private:
  std::list<ISpritedTextObserver *> list_observer_;
};
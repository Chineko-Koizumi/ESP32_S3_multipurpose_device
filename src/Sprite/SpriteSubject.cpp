
#include "Sprite/SpriteSubject.h"

 SpriteSubject::~SpriteSubject() 
  {
  }

  void SpriteSubject::Attach(ISpriteObserver *observer)
  {
    list_observer_.push_back(observer);
  }

  void SpriteSubject::Detach(ISpriteObserver *observer) 
  {
    list_observer_.remove(observer);
  }

  void SpriteSubject::NotifyPrintSprite()
  {
    std::list<ISpriteObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->PrintSprite();
      ++iterator;
    }
  }

  void SpriteSubject::NotifyForcePrintSprite()
  {
    std::list<ISpriteObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->ForcePrintSprite();
      ++iterator;
    }
  }

  void SpriteSubject::NotifyCreateSprite()
  {
    std::list<ISpriteObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->CreateSprite();
      ++iterator;
    }
  }

  void SpriteSubject::NotifysetSpriteBackground( TFT_eSPI* pScreen )
  {
    std::list<ISpriteObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->setSpriteBackground(pScreen);
      ++iterator;
    }
  }



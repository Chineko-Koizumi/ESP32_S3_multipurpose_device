
#include "SpritedTextSubject.h"

 SpritedTextSubject::~SpritedTextSubject() 
  {
  }

  void SpritedTextSubject::Attach(ISpritedTextObserver *observer)
  {
    list_observer_.push_back(observer);
  }

  void SpritedTextSubject::Detach(ISpritedTextObserver *observer) 
  {
    list_observer_.remove(observer);
  }

  void SpritedTextSubject::NotifyPrintText()
  {
    std::list<ISpritedTextObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->PrintText();
      ++iterator;
    }
  }

  void SpritedTextSubject::NotifyForcePrintText()
  {
    std::list<ISpritedTextObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->ForcePrintText();
      ++iterator;
    }
  }

  void SpritedTextSubject::NotifyCreateSprite()
  {
    std::list<ISpritedTextObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->CreateSprite();
      ++iterator;
    }
  }

  void SpritedTextSubject::NotifysetSpriteBackground(TFT_eSPI* ScreenPtr)
  {
    std::list<ISpritedTextObserver *>::iterator iterator = list_observer_.begin();
    while (iterator != list_observer_.end()) 
    {
      (*iterator)->setSpriteBackground(ScreenPtr);
      ++iterator;
    }
  }



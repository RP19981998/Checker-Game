
#ifndef RPlayerr_hpp
#define RPlayer_hpp

#include <stdio.h>
#include "Game.hpp"

namespace ECE141 {
  
  class RPlayer : public PlayerInterface {
  public:
    RPlayer();
    ~RPlayer(){}
    virtual bool takeTurn(Game &aGame,PieceColor asColor,std::ostream &aLog);

  };
   
}

#endif /* RPlayer_hpp */

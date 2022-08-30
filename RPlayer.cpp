
#include "RPlayer.hpp"

namespace ECE141 {
   
    size_t scale=8;
    Location tempLoc;
    struct Score {
        size_t index[12];
        size_t value[12];
        size_t Capacity=0;
    };
    Score theScore;
   
  Rui784Player::Rui784Player() : PlayerInterface() {}
  //begin my code here

  bool canJumpTo(Game* aGame, const Piece aPiece, int aDeltaX, int aDeltaY) {
      Location thePLoc = aPiece.getLocation();
      Location theStep(thePLoc.row + aDeltaY, thePLoc.col + aDeltaX);
      auto theColor = aPiece.getColor();
      if (auto theTile = aGame->getTileAt(theStep)) {
          if (auto thePiece = theTile->getPiece()) {
              if (thePiece && theColor != thePiece->getColor()) {
                  Location theJump(theStep.row + aDeltaY, theStep.col + aDeltaX);
                  if (auto theJumpTile = aGame->getTileAt(theJump)) {
                      if (theJumpTile->isEmpty()) {
                          tempLoc = theJump;
                          return true;
                      }
                  }
              }
          }
      }
      return false;
  }
  bool canMoveTo(Game* aGame, const Piece aPiece, int aDeltaX, int aDeltaY) {
      Location thePLoc = aPiece.getLocation();
      Location theStep(thePLoc.row + aDeltaY, thePLoc.col + aDeltaX);
      if (auto theTile = aGame->getTileAt(theStep)) {
          if (theTile->isEmpty()) {
              tempLoc = theStep;
              return true;
          }
      }
      
      return false;
  }
  //get the pieces now can move
  int moreY(PieceColor aColor) { return PieceColor::blue == aColor ? -1 : 1; }//to the opponent
  int lessY(PieceColor aColor) { return PieceColor::blue == aColor ? 1 : -1; }
  int moreX(PieceColor aColor) { return PieceColor::blue == aColor ? 1 : -1; }//to the right
  int lessX(PieceColor aColor) { return PieceColor::blue == aColor ? -1 : 1; }
  
  bool CanJump(Game* aGame, Piece aPiece, PieceColor aColor) {
      if (canJumpTo(aGame, aPiece, lessX(aColor), moreY(aColor))) {
          return true;
      }
    
      if (canJumpTo(aGame, aPiece, moreX(aColor), moreY(aColor))) {
          return true;
      }

      if (PieceKind::king == aPiece.getKind()) {
          if (canJumpTo(aGame, aPiece, lessX(aColor), lessY(aColor))) {
              return true;
          }

          if (canJumpTo(aGame, aPiece, moreX(aColor), lessY(aColor))) {
              return true;
          }
      }
      return false; }

  size_t CanMove(Game* aGame, Piece aPiece, PieceColor aColor) {
      if (PieceKind::king == aPiece.getKind()) {
          if (canMoveTo(aGame, aPiece, moreX(aColor), lessY(aColor))) {
              return 2;
          }
          if (canMoveTo(aGame, aPiece, lessX(aColor), lessY(aColor))) {
              return 2;
          }

          
      }
      
      
      if (canMoveTo(aGame,aPiece, lessX(aColor), moreY(aColor))) {
              return 1;
      }

      if (canMoveTo(aGame, aPiece, moreX(aColor), moreY(aColor))) {
              return 1;
      }

          
      return 0; }
 
  bool MoveViable(Game* aGame,Piece aPiece, PieceColor aColor) {
      if (CanJump(aGame,aPiece, aColor)) return true;
      else if (CanMove(aGame,aPiece, aColor)) return true;
      else return false;
  }
  
 
  //rank the moveviable pieces and find the best move action
  
  bool isKingable(const Piece aPiece) {
      Location aDest = aPiece.getLocation();
      if (PieceKind::pawn == aPiece.getKind()) {
          return PieceColor::blue == aPiece.getColor()
              ? 0 == aDest.row : (int)(scale - 1) == aDest.row;
      }
      return false;
  }

  bool hasOpponent(Game* aGame, const Piece aPiece, const Location aDest) {
      if (auto theTile = aGame->getTileAt(aDest)) {
          if (auto theOPiece = theTile->getPiece()) {
              if (!(aPiece.getColor() == theOPiece->getColor()))
                      return true;
          }
      }
      return false;
  }

  bool noSafe(Game* aGame, const Piece aPiece,PieceColor aColor) {
      int theRow = aPiece.getLocation().row + moreY(aColor);
      int theCol = aPiece.getLocation().col + lessX(aColor);
    
      Location theDestLoc(theRow, theCol);
      if (hasOpponent(aGame, aPiece, theDestLoc)) return true;

      theDestLoc.row = aPiece.getLocation().row + moreY(aColor); 
      theDestLoc .col= aPiece.getLocation().col + moreX(aColor);
      if (hasOpponent(aGame, aPiece, theDestLoc)) return true;

      theDestLoc.row = aPiece.getLocation().row + lessY(aColor); 
      theDestLoc.col = aPiece.getLocation().col + lessX(aColor);
      if (hasOpponent(aGame, aPiece, theDestLoc)) return true;

      theDestLoc.row = aPiece.getLocation().row + lessY(aColor); 
      theDestLoc.col = aPiece.getLocation().col + moreX(aColor);
      if (hasOpponent(aGame, aPiece, theDestLoc)) return true;
      
      return false;
  }

  bool DoRank(Game* aGame, Piece aPiece, PieceColor aColor,size_t j) { 
  /*
    (+4)become king with safe   
    value7:jumps 
    (-1)non-safe jumps
    value5:jump   
    (-1)non-safe jump   
    value2:move   
    */
      theScore.value[j] = 0;
      int count=-2;
      if (CanJump(aGame, aPiece, aColor)) {
          count = 1;
          while (CanJump(aGame, aPiece, aColor)) {
              aPiece.setLocation(tempLoc);
              count++;
          }
      }
      else if (PieceKind::king == aPiece.getKind()) {
          if (CanMove(aGame, aPiece, aColor) == 2) {
              aPiece.setLocation(tempLoc);
              count = 0;
          }
          else if (CanMove(aGame, aPiece, aColor) == 1){       
              aPiece.setLocation(tempLoc);
              count = -1;
           }     
      }
      else if (CanMove(aGame, aPiece, aColor)) {
          count = 1;
          aPiece.setLocation(tempLoc);
      }
      //put in the value
      if (count == -2) return true;
      else if (count == -1) {
          theScore.value[j] += 1;
          if (noSafe(aGame, aPiece, aColor)) theScore.value[j] += 2;
          return true;
      }
      else if (count == 0) {
          theScore.value[j] += 2;
          if (noSafe(aGame, aPiece, aColor)) theScore.value[j] += 2;
          return true;
      }
      else if (count == 1) {
          theScore.value[j] += 2;
          if (noSafe(aGame, aPiece, aColor));
          else if (isKingable(aPiece)) theScore.value[j] += 4;
          return true;
      } 
      else if (count == 2) theScore.value[j] += 5;
      else if (count > 2) theScore.value[j] += 7;

      if (noSafe(aGame, aPiece, aColor))theScore.value[j] -= 1;
      else if (isKingable(aPiece)) theScore.value[j] += 4;
      
      return true;
  }
  
 
  //revie all the pireces now have 
  bool DoReview(Game* aGame, PieceColor aColor) {
      size_t count = aGame->countAvailablePieces(aColor);
      for (size_t i = 0; i < count; i++) {
          if (auto aPiece = aGame->getAvailablePiece(aColor, i)) {
              if (MoveViable(aGame, *aPiece, aColor)) {
                  theScore.index[theScore.Capacity] = i;
                  DoRank(aGame, *aPiece, aColor, theScore.Capacity);
                  theScore.Capacity++;
                  
              }
          }
      }

      return true;
  }

  size_t FindBestMove() {
      size_t i = theScore.Capacity;
      size_t index=0;
      size_t maxValue = 0;
      for (size_t j = 0; j < i; j++) {
          if (theScore.value[j] > maxValue) {
              index = j;
              maxValue = theScore.value[j];
          }
      }
          
      return index;
  }

  //move action
  bool DoMove(Game* aGame, PieceColor aColor) {
      size_t index = FindBestMove();
      Piece aPiece = *aGame->getAvailablePiece(aColor, theScore.index[index]);
      if (CanJump(aGame, aPiece, aColor)) {
          while (CanJump(aGame, aPiece, aColor)) {  
              aGame->movePieceTo(aPiece, tempLoc);
              aPiece.setLocation(tempLoc);
          }
      }
      else if (CanMove(aGame, aPiece, aColor)) 
          aGame->movePieceTo(aPiece, tempLoc);
      return true;
  }
  
  bool Rui784Player::takeTurn(Game &aGame,PieceColor asColor,std::ostream &aLog) {
     
      DoReview(&aGame,asColor);
      DoMove(&aGame,asColor);

      for (size_t i = 0; i < theScore.Capacity; i++) {
          theScore.index[i] = 0;
          theScore.value[i] = 0;
      }
      theScore.Capacity = 0;

      return true;
  }
}

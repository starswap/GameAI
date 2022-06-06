//AI Hex Game inspired by C++ for C Programmers MOOC, though the code is all my own rather than using the assignment code provided.
//Hamish Starling 2021.

#include "graph.h"
#include <string>
#include <queue>
#include <fstream>
#include <ctime>
using namespace std;
const int modifiersI[] = {-1,-1,0,0,1,1};
const int modifiersJ[] = {0,1,-1,1,-1,0}; 
const char representation[3] = {'.','B','W'};
typedef pair<int,int> pii;


ostream& operator << (ostream& out, const pii p) {
  out << "(" << p.first << "," << p.second << ")"; 
  return out;
}
template <class T>
ostream& operator << (ostream& out, const vector<T> v) {
  out << "[";
  for (auto a : v){
    out << a << ","; 
  }
  out << "]";
  return out;
}


class strTimes {
  public:
    int n;
    string charsToOut; 
    strTimes(int n,string charsToOut): n(n),charsToOut(charsToOut) {};

  friend ostream& operator << (ostream& out,const strTimes& a) {
    for (int i = 0;i<a.n;i++) {
      out << a.charsToOut;
    }
    return out;
  };
};
class HexBoard : public Graph {
  public:
    HexBoard(int n):Graph(n*n),n(n),occupied(vector<int>(n*n,0)),emptySquares(n*n) {
      for (int i = 0;i<n;i++) {
        for (int j = 0;j<n;j++){
          for (int a = 0;a < 6;a++){
            if (!(i+modifiersI[a] <0 || i+modifiersI[a] >= n || j+modifiersJ[a]< 0 || j+modifiersJ[a]>=n)) {
              if (!hasEdge(coordsToNumber(pair<int,int>(i+modifiersI[a],j+modifiersJ[a])),coordsToNumber(pair<int,int>(i,j)))) { 
                  addEdge(coordsToNumber(pair<int,int>(i+modifiersI[a],j+modifiersJ[a])),coordsToNumber(pair<int,int>(i,j)),1) ;//Unit cost, undirected edges i.e. unweighted undirected
                  addEdge(coordsToNumber(pair<int,int>(i,j)),coordsToNumber(pair<int,int>(i+modifiersI[a],j+modifiersJ[a])),1) ;//Unit cost, undirected edges i.e. unweighted undirected  
              }
            }
          }
        }
      }
    }
    HexBoard (string filename,int n):HexBoard(n) {
      ifstream myStream(filename);
      istream_iterator<string> it(myStream),end; it++;
      int i = 0;
      for (;it!=end;it++) { //for every line in file except first which contains the line of Ws
        int j = -1;     
        for (auto ch : *it) {
          if (j == -1 || j==n){  //skip the edge black lines.
            j++;continue;
          }
          else if (ch == '-')
              continue; //Ignore - spacers
          if (ch == 'B') {
            occupied[coordsToNumber(pii(i,j))] = 1;
          } else if (ch == 'W') {
            occupied[coordsToNumber(pii(i,j))] = 2;
          }
          j++;          
        }
        i++;
        if (i == n){break;} //Skip the bottom line of Ws too.
      }
    };
    //Creates hex board size nxn
    inline int coordsToNumber(pair<int,int> coords) const {
      return coords.first*n + coords.second;
    };
    inline pair<int,int> numberToCoords(int number) const {
      return pair<int,int>(number/n,number%n);
    };
    void placeHex(pair<int,int>coords,int colourNum){
      if (coords.first >= n || coords.first < 0 || coords.second >= n || coords.second < 0) {
        cout << coords;
        throw "Invalid move grumble grumble.";
      } else if (colourNum < 0 || colourNum > 2) {
        throw "Invalid colour number grumble";
      }
      else {
        occupied[coordsToNumber(coords)] = colourNum;
        emptySquares--;
      }
    }
    void placeHex(int number,int colourNum) {
      if (number < 0 || number >= n*n) {
        cout << number;
        throw "Invalid move grumble grumble.";
      } else if (colourNum < 0 || colourNum > 2) {
        throw "Invalid colour number grumble";
      }
      else {
        occupied[number] = colourNum;
        emptySquares--;
      }
    }
    bool gameWonByWhite(){ //Uses bfs
      queue<int> q;
      unordered_set<int> visited;
      for (int j=0;j<n;j++){
        if (occupied[coordsToNumber(pii(0,j))] == 2){
          q.push(coordsToNumber(pii(0,j))); //Only if square is white.
          visited.insert(coordsToNumber(pii(0,j)));
        }
      }
      while (!q.empty()) {
        int vertexToProcess = q.front(); q.pop();
        for (auto neighbour : AL[vertexToProcess]){
          if (occupied[neighbour.first] == 2){ //Only if black.
            if (numberToCoords(neighbour.first).first == n-1){
              return true;//A path exists from top to bottom.
            }
            else if (visited.count(neighbour.first) == 0){
              visited.insert(neighbour.first);
              q.push(neighbour.first);
            }
          }
        }
      }
      if (!q.empty()){
        return false;//The black player has not won as no path
      }
    };
    bool gameWonByBlack() {
      queue<int> q;
      unordered_set<int> visited;
      for (int j=0;j<n;j++){
        if (occupied[coordsToNumber(pii(j,0))] == 1){
          q.push(coordsToNumber(pii(j,0))); //Only if square is black.
          visited.insert(coordsToNumber(pii(j,0)));
        }
      }
      while (!q.empty()) {
        int vertexToProcess = q.front(); q.pop();
        for (auto neighbour : AL[vertexToProcess]){
          if (occupied[neighbour.first] == 1){ //Only if black.
            if (numberToCoords(neighbour.first).second == n-1){
              return true;//A path exists from top to bottom.
            }
            else if (visited.count(neighbour.first) == 0){
              visited.insert(neighbour.first);
              q.push(neighbour.first);
            }
          }
        }
      }
      if (!q.empty()){
        return false;//The black player has not won as no path
      }
    };
    bool gameWon(int turn) { //Separating them reduces checks - only check for white win on white turn and vv.
      return (turn==1?gameWonByBlack():gameWonByWhite());
    }
    int findLongestPath(int colour,int square,unordered_set<int> &visited)  {
      int maxPath = 0;
      int subPath = 0;
      visited.insert(square);
      for (auto neighbour : AL[square]) {
        if (occupied[neighbour.first] == colour && visited.count(neighbour.first) == 0 ) {
          subPath = findLongestPath(colour, neighbour.first,visited);
          if (subPath > maxPath) {
            maxPath = subPath;
          }
        }
      }
      return maxPath + 1;
    };
    friend ostream& operator << (ostream& out,const HexBoard &h) {
      int j = 0;
      int i = 0;
      out << " ";
      for (i = 0;i<h.n;i++) {
        out << i << " ";
      }
      out << endl;
      out << " ";
      out << strTimes(h.n*2,"W") << endl;
      for (i = 0;i<h.n;i++) {
        out << i;
        out << strTimes(i," "); //start with i spaces to line up
        out << "B";
        for (j = 0;j<h.n-1;j++) {
          out << representation[h.occupied[h.coordsToNumber(pair<int,int>(i,j))]] << '-';
        } 
        out << representation[h.occupied[h.coordsToNumber(pair<int,int>(i,j))]] << "B" << endl;
      }
      out << strTimes(i+1," ");        
      out << strTimes(h.n*2,"W") << endl;
      return out;
    };
    inline int get_n() {return n;}
    inline int currentSquareColour(int a) {return occupied[a];} 
    inline vector<int> getAllSquares() {return occupied;}
    inline void setAllSquares(vector<int> a) {occupied = a;};
    inline int howManyMovesLeft() {return emptySquares;}
    inline void clearBoard() {
      fill(occupied.begin(),occupied.end(),0);
    }
  private:
    int n;
    vector<int> occupied; //0 for unoccupied, 1 for B, 2 for W.
    int emptySquares;
};

bool playerTurn(HexBoard &board,const int &playerColour,bool canPie=false) {
  cout << board;//Player takes turn on board
  if (canPie){
    char pies;
    cout << "To make the game fair, only on this turn, the second player can decide to swap colours with the 1st player if they think the first player has made a good move. This is called the 'Pie' rule.";
    cout << "Do you wish to use this rule and swap colours and become the " << representation[playerColour %2 + 1] << " player?(y/n)";
    cin >> pies;
    if (pies == 'y') {
      cout << "Ok; you are now the " << representation[playerColour%2+1] << " player." << endl;
      return true;
    }
    else {
      cout << "Fair enough. You are still the " << representation[playerColour] << " player." << endl;
    }
  }
  int i = -1;
  int j = -1;
  while (i < 0 || j < 0 || i>=board.get_n() || j>= board.get_n() || board.currentSquareColour(board.coordsToNumber(pii(i,j)))  != 0 ) {
    cout << "Where do you want to place a Hex?" << endl;
    cout << "Enter x coord counting across from left of desired row:";
    cin >> j;
    cout << "Enter y coord counting down from top row:";
    cin >> i;
  }
  board.placeHex(pii(i,j),playerColour); 
  cout << board;
  return false;
}
bool computerTurn(HexBoard &board,const int &computerColour,bool canPie=false) {
  //Monte Carlo
  int square;
  const int kIterationsPerMove = 10000;
  if (canPie) { //For now if can pie it will.
    cout << "The computer invokes the Pie rule and becomes the " << representation[computerColour %2 + 1] << " player. You are now the " << representation[computerColour] << " player" << endl;
    return true;
  }
  else {
    vector<int> current = board.getAllSquares();
    int maxComputerWins = 0;
    pii bestMove;
    int n = board.get_n();
    for (int i = 0;i<n;i++) {
      for (int j = 0;j<n;j++) {    
        if (board.currentSquareColour(board.coordsToNumber(pii(i,j))) == 0) { //empty
          board.placeHex(pii(i,j),computerColour);
          int computerWins = 0;
          for (int k=0;k<kIterationsPerMove;k++) {
            int placed[2] = {0,0}; //Both should hit a max of remaining/2 except if odd remaining in which case the other player's can be 1 more.
            int total = board.howManyMovesLeft();
            for (int j=0;j<n*n;j++) { //for every square on board
              if (board.currentSquareColour(j) == 0) {
                if (placed[computerColour-1] == total/2) { //integer division if odd.
                  board.placeHex(j,computerColour%2+1); //Computer done - only player squares                  
                } 
                else if ((total %2 == 0 && placed[computerColour%2] == total/2) || (total%2 == 1 && placed[computerColour%2] == total/2+1)) { // player done 
                  board.placeHex(j,computerColour);             
                }
                else {
                  int randomZeroOne = rand()%2;
                  board.placeHex(j,randomZeroOne+1);
                  placed[randomZeroOne]++;
                }
              }
            }
            //On each iteration - 
            if (board.gameWon(computerColour)){
              computerWins++;
            }
            board.setAllSquares(current); //reset
          }
          //for each possible move
          if (computerWins > maxComputerWins) {
            maxComputerWins = computerWins;
            bestMove = pii(i,j);
          }
        }
      }
    }
  board.placeHex(bestMove,computerColour);
  return false;  
  }
}

/*
Alternative Strategies:
	
bool computerTurn(HexBoard &board,const int &computerColour,bool canPie=false) { //Purely Random
  int square;
  if (canPie) {
    cout << "The computer invokes the Pie rule and becomes the " << representation[computerColour %2 + 1] << " player. You are now the " << representation[computerColour] << "player" << endl;
    return true;
  }
  else {
    square = rand()%(board.get_n()*board.get_n());
    while (board.currentSquareColour(square) != 0) {
      square = rand()%(board.get_n()*board.get_n()); // Random square until a free one found.
    }
    board.placeHex(board.numberToCoords(square),computerColour);
    cout << "Computer plays: " << pii(board.numberToCoords(square).second,board.numberToCoords(square).first) << endl;
    return false;
  }
  
}


bool computerTurn(HexBoard &board,const int &computerColour,bool canPie=false) { //Based on some rudimentary strategy
  int square;
  if (canPie) { //For now if can pie it will.
    cout << "The computer invokes the Pie rule and becomes the " << representation[computerColour %2 + 1] << " player. You are now the " << representation[computerColour] << " player" << endl;
    return true;
  }
  else {
    int maxPathLength = -1;
    pii bestSquare; 
    int candidate;
    unordered_set<int> a;
    for (int i = 0;i<board.get_n();i++) {
      for (int j=0;j<board.get_n();j++) {
        if (board.currentSquareColour(board.coordsToNumber(pii(i,j))) == 0) {
          cout << pii(i,j);
          board.placeHex(pii(i,j), computerColour);
          a.clear();
          candidate = board.findLongestPath(computerColour,board.coordsToNumber(pii(i,j)),a);
          if (candidate > maxPathLength) {
            maxPathLength = candidate;
            bestSquare = pii(i,j);
          }

  // what length path could the human get on next move?        
          unordered_set<int> a;        
          board.placeHex(pii(i,j), computerColour%2 + 1);
          a.clear();
          candidate = board.findLongestPath(computerColour%2 + 1,board.coordsToNumber(pii(i,j)),a);
          if (candidate > maxPathLength) {
            maxPathLength = candidate;
            bestSquare = pii(i,j);
          }
          board.placeHex(pii(i,j), 0); // reset  
        }      
      }
    }
    board.placeHex(bestSquare,computerColour);
    cout << "Computer plays: (" << bestSquare.second << "," << bestSquare.first << ")" << endl;    
    return false;
    //First we check all possible places we could add our square - choose biggest
    //Then all possible places they could extend if we didn't block now
    //If both are same or they have a rubbish path - pick offence i.e extend computer path
    //if their path is strictly better than the computer's then we block.
  }  
}
*/

void playGame(int n) {
  HexBoard board(n);
  int playerColour;
  int firstOrSecond;
  int currentColour;
  cout << "Which colour do you want to start as - black (1) or white(2)?";
  cin >> playerColour;
  cout << "Do you want to go first (1) or second (2)?";
  cin >> firstOrSecond;

  if (firstOrSecond == 1) {
    playerTurn(board,playerColour);
    if (computerTurn(board,playerColour%2+1,true)) {
      playerColour = playerColour%2+1;//Pied
    }
    currentColour = playerColour;//Player Played, ComputerPlayed/Pied, player plays.
  } else {
    computerTurn(board,playerColour%2 + 1);
    if (playerTurn(board,playerColour,true)) {
      playerColour = playerColour%2 + 1;//pied
    }
    currentColour = playerColour%2+1;//Computer Played, Player played or pied, computer
  }
  clock_t start,end;
  while (!(board.gameWon(currentColour%2+1))) { //Did the colour that just played win? - if so quit
    if (currentColour == playerColour) {
      playerTurn(board,currentColour);
    }
    else {
      cout << "Computer thinking ...." << endl;
      start = clock();
      computerTurn(board,currentColour);
      end = clock();
      cout << "Computer took " << 1.0*(end-start)/CLOCKS_PER_SEC << " seconds." << endl;
    }
    currentColour = currentColour%2 + 1;
  }
  cout << "The " << representation[currentColour%2+1] << " player " << "has won." << endl;
};

int main() {
  srand(time(0));
  int size;
  cout << "What size board do you want to use? (3+)";
  cin >> size;
  playGame(size);
}
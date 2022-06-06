#A chess game with a basic computer player - Hamish Starling
# The AI is based on a FreeCodeCamp tutorial, which was written in JS
import sys # sys module used to exit program at game end
import copy# used to create copies of the gameboard so that we can work out best moves for AI

def printBoard(board):
  """A function for printing the board out."""
  NewBoard = []
  #Flipping board if it is the white player's turn, so that the current player always has their pieces at the bottom.
  if turn == "B":
    NewBoard = board
  else:
    for row in board[::-1]:
      NewBoard.append(row[::-1])
  print("\n")
  nums = " "
  #Print out the numbers of the columns in one row so its easy to move pieces because you can track with your finger.
  for i in range(0,8):
    nums += " "+str(i)+" "
  print(nums)
  print(" " + ("-"*24))#Print a line of dashes for the top of the board
  for i,row in enumerate(NewBoard): # loop through every row in the board, print its number, and then print every item in the row with pipes between them
    NewRow = ""
    NewRow += str(i)
    NewRow += "|"
    for piece in row:
      NewRow += piece + "|"
    print(NewRow)
    print(" " + ("-"*24))
  print("\n")


def calculateValidMoves(square,board,checking=False): # checking is to avoid recursion errors. It is True when we are checking for checkmate. The problem is that I was checking if any moves resulted in check, but that function called this function. This meant I ended up in a cycle with these two functions and exceded the maximum recursion depth.
  """Calculating available moves for a given piece""" 
  global BlackKingMoved
  global WhiteKingMoved
  global checked
  piece = board[square[0]][square[1]] #Find out what piece is at the chosen square
  colour = piece[0]
  piece = piece[1]
  availibles = [] # An array of all availible moves e.g [[1,2],[2,3]] meaning the piece is able to move to [1,2] or [2,3]

  if piece == "P": #Pawn
    if colour == "W":#Colour matters, it decides direction of movement
      if square[0] + 1 <8 and board[square[0] + 1][square[1]] == "  ":
        availibles.append([square[0] + 1, square[1]])
      if square[0] == 1 and board[square[0] + 2] [ square[1]] == "  ":
        availibles.append([square[0] + 2, square[1]])
      if square[0]+1 <8 and square[1] + 1 <8 and board[square[0] + 1][square[1]+1][0] != " " and board[square[0] + 1][square[1]+1][0] != "W":
          availibles.append([square[0] + 1, square[1] + 1])
      if square[0]+1 <8 and square[1] - 1 > -1 and board[square[0] + 1][square[1]-1][0] != " " and board[square[0] + 1][square[1]-1][0] != "W":
          availibles.append([square[0] + 1, square[1] - 1])
    elif colour == "B":
      if board[square[0] - 1][square[1]] == "  ":
        availibles.append([square[0] - 1, square[1]])#
      if square[0] == 6  and board[square[0] - 2] [ square[1]] == "  ":
        availibles.append([square[0] - 2 , square[1]])
      if square[0] - 1 >-1 and square[1] - 1 > -1 and board[square[0] - 1][square[1]-1][0] != " " and board[square[0] - 1][square[1]-1][0] != "B" :
          availibles.append([square[0] - 1, square[1] - 1])
      if square[0]-1 > -1 and square[1] + 1 <8 and board[square[0] - 1][square[1]+1][0] != " " and board[square[0] - 1][square[1]+1][0] != "B":
          availibles.append([square[0] - 1, square[1] + 1])  
   #Need to add the en-passant rule, tried but didn't have time

  elif piece == "R": # Rook
  #Loop through possible moves, from 1 space to 7 spaces, moving up, down, left or right. If it encounters the edge of the board, then break. If it encounters a piece of another colour, add the ability to take that piece, then break. Break if another piece is encountered of the same colour as this will fail.
    for i in range(1,8):
      if square[0] + i > 7: # Edge of board
        break
      if board[square[0] + i][square[1]][0] == board[square[0]][square[1]][0]: # Same colour
        break 
      elif board[square[0] + i][square[1]][0] != " ": # Piece of another colour
        availibles.append([square[0] + i, square[1]] )
        break
      availibles.append([square[0] + i, square[1]] )
    for i in range(1,8):
      if square[0] - i < 0:
        break
      if board[square[0] - i][square[1]][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0] - i][square[1]][0] != " ":
        availibles.append([square[0] - i, square[1]] )
        break
      availibles.append([square[0] - i, square[1]] )
    for i in range(1,8):
      if square[1] + i > 7:
        break
      if board[square[0]][square[1] + i][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0]][square[1] + i][0] != " ":
        availibles.append([square[0], square[1] + i] )
        break
      availibles.append([square[0],square[1] + i] )
    for i in range(1,8):
      if square[1] - i < 0:
        break
      if board[square[0]][square[1] - i][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0]][square[1] - i][0] != " ":
        availibles.append([square[0], square[1] - i] )
        break
      availibles.append([square[0],square[1] - i] )

  elif piece == "N": # Knight
    if not (square[0] + 2 > 7 ):  #Move 2 spaces down
      if not (square[1] + 1 > 7): # Move 1 space to right
        if not board[square[0]+2][square[1]+1][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]+2,square[1]+1])
      if not (square[1] - 1 < 0): #Move 1 space to left
        if not board[square[0]+2][square[1]-1][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]+2,square[1]-1])

    if not (square[0] - 2 < 0 ): # 2 spaces up
      if not (square[1] + 1 > 7): #1 space to right
        if not board[square[0]-2][square[1]+1][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]-2,square[1]+1])
      if not (square[1] - 1 < 0): #  1 space to left
        if not board[square[0]-2][square[1]-1][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]-2,square[1]-1])

    if not (square[1] + 2 > 7 ): # 2 spaces right
      if not (square[0] + 1 > 7): # 1 space down
        if not board[square[0]+1][square[1]+2][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]+1,square[1]+2])
      if not (square[0] - 1 < 0):
        if not board[square[0]-1][square[1]+2][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]-1,square[1]+2])

    if not (square[1] - 2 < 0 ): #etc
      if not (square[0] + 1 > 7):
        if not board[square[0]+1][square[1]-2][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]+1,square[1]-2])
      if not (square[0] - 1 < 0):
        if not board[square[0]-1][square[1]-2][0] == board[square[0]][square[1]][0]:
          availibles.append([square[0]-1,square[1]-2])

  elif piece == "B": # Bishop
  #Same as rook, just diagonal
    for i in range(1,8):
      if square[0] + i > 7 or square[1] + i > 7 or board[square[0]+i][square[1]+i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]+i][square[1]+i][0] != " ":
        availibles.append([square[0] + i, square[1] + i ])
        break
      availibles.append([square[0] + i, square[1] + i ])


    for i in range(1,8):
      if square[0] - i < 0 or square[1] - i < 0 or board[square[0]-i][square[1]-i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]-i][square[1]-i][0] != " ":
        availibles.append([square[0] - i, square[1] - i ])
        break
      availibles.append([square[0] - i, square[1] - i ])


    for i in range(1,8):
      if square[0] + i > 7 or square[1] - i < 0 or board[square[0]+i][square[1]-i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]+i][square[1]-i][0] != " ":
        availibles.append([square[0] + i, square[1] - i ])
        break
      availibles.append([square[0] + i, square[1] - i ])

    for i in range(1,8):
      if square[0] - i < 0 or square[1] + i > 7 or board[square[0]-i][square[1]+i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]-i][square[1]+i][0] != " ":
        availibles.append([square[0] - i, square[1] + i ])
        break
      availibles.append([square[0] - i, square[1] + i ])


  elif piece == "K": # King
    if square[0]+1 <= 7 and (board[square[0]+1][square[1]][0] == " " or board[square[0]+1][square[1]][0] != colour):
      availibles.append([square[0]+1,square[1]])
    if square[0]-1 >= 0 and (board[square[0]-1][square[1]][0] == " " or board[square[0]-1][square[1]][0] != colour):
      availibles.append([square[0]-1,square[1]])
    if square[0]+1 <= 7 and square[1]+1 <= 7 and (board[square[0]+1][square[1]+1][0] == " " or board[square[0]+1][square[1]+1][0] != colour):
      availibles.append([square[0]+1,square[1]+1])
    if square[0]-1 >= 0 and square[1]-1 >= 0 and (board[square[0]-1][square[1]-1][0] == " " or board[square[0]-1][square[1]-1][0] != colour):
      availibles.append([square[0]-1,square[1]-1])
    if square[1]+1 <= 7 and (board[square[0]][square[1]+1][0] == " " or board[square[0]][square[1]+1][0] != colour):
      availibles.append([square[0],square[1]+1])
    if square[1]-1 >= 0 and (board[square[0]][square[1]-1][0] == " " or board[square[0]][square[1]-1][0] != colour):
      availibles.append([square[0],square[1]-1])
    if  square[0]-1 >= 0 and square[1]+1 <= 7 and (board[square[0]-1][square[1]+1][0] == " " or board[square[0]-1][square[1]+1][0] != colour):
      availibles.append([square[0]-1,square[1]+1])
    if square[0]+1 <= 7 and square[1]-1 >= 0 and (board[square[0]+1][square[1]-1][0] == " " or board[square[0]+1][square[1]-1][0] != colour):
      availibles.append([square[0]+1,square[1]-1])

    if checking == False: # Castling. The positions are hard-coded in, because there are very few ways to castle, and they always have fixed positions
    #I use my function to check if the space the king would move into, or through is in check. I do this by telling it that the king is there, even though it's not
      if square == [0,3] and colour == "W" and WhiteKingMoved == False and WhiteLeftRookMoved == False and board[0][1] == "  " and board[0][2] == "  " and checked[0] == False and KingCheck(board,[0,0],[0,1])[0] == False and KingCheck(board,[0,0],[0,2])[0] == False:
        availibles.append([0,1])

      if square == [0,3] and colour == "W" and WhiteKingMoved == False and WhiteRightRookMoved == False and board[0][4] == "  " == board[0][5] == board[0][6] and checked[0] == False and KingCheck(board,[0,0],[0,4])[1] == False and KingCheck(board,[0,0],[0,5])[1] == False and KingCheck(board,[0,0],[0,6])[0] == False:
        availibles.append([0,5])

      if square == [7,3] and colour == "B" and BlackKingMoved == False and BlackLeftRookMoved == False and board[7][1] == "  " and board[7][2] == "  " and checked[1] == False and KingCheck(board,[7,1],[0,0])[1] == False and KingCheck(board,[7,2],[0,0])[1] == False :
        availibles.append([7,1])

      if square == [7,3] and colour == "B" and BlackKingMoved == False and BlackRightRookMoved == False and board[7][4] == "  " == board[7][5] == board[7][6] and checked[1] == False and KingCheck(board,[7,4],[0,0])[1] == False and KingCheck(board,[7,4],[0,0])[1] == False and KingCheck(board,[7,6],[0,0])[1] == False:
        availibles.append([7,5])




  elif piece == "Q": # Queen, this is the bishop, plus the rook.
    for i in range(1,8):
      if square[0] + i > 7:
        break
      if board[square[0] + i][square[1]][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0] + i][square[1]][0] != " ":
        availibles.append([square[0] + i, square[1]] )
        break
      availibles.append([square[0] + i, square[1]] )
    for i in range(1,8):
      if square[0] - i < 0:
        break
      if board[square[0] - i][square[1]][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0] - i][square[1]][0] != " ":
        availibles.append([square[0] - i, square[1]] )
        break
      availibles.append([square[0] - i, square[1]] )
    for i in range(1,8):
      if square[1] + i > 7:
        break
      if board[square[0]][square[1] + i][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0]][square[1] + i][0] != " ":
        availibles.append([square[0], square[1] + i] )
        break
      availibles.append([square[0],square[1] + i] )
    for i in range(1,8):
      if square[1] - i < 0:
        break
      if board[square[0]][square[1] - i][0] == board[square[0]][square[1]][0]:
        break 
      elif board[square[0]][square[1] - i][0] != " ":
        availibles.append([square[0], square[1] - i] )
        break
      availibles.append([square[0],square[1] - i] )
    
    for i in range(1,8):
      if square[0] + i > 7 or square[1] + i > 7 or board[square[0]+i][square[1]+i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]+i][square[1]+i][0] != " ":
        availibles.append([square[0] + i, square[1] + i ])
        break
      availibles.append([square[0] + i, square[1] + i ])


    for i in range(1,8):
      if square[0] - i < 0 or square[1] - i < 0 or board[square[0]-i][square[1]-i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]-i][square[1]-i][0] != " ":
        availibles.append([square[0] - i, square[1] - i ])
        break
      availibles.append([square[0] - i, square[1] - i ])


    for i in range(1,8):
      if square[0] + i > 7 or square[1] - i < 0 or board[square[0]+i][square[1]-i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]+i][square[1]-i][0] != " ":
        availibles.append([square[0] + i, square[1] - i ])
        break
      availibles.append([square[0] + i, square[1] - i ])

    for i in range(1,8):
      if square[0] - i < 0 or square[1] + i > 7 or board[square[0]-i][square[1]+i][0] == board[square[0]][square[1]][0]:
        break
      elif board[square[0]-i][square[1]+i][0] != " ":
        availibles.append([square[0] - i, square[1] + i ])
        break
      availibles.append([square[0] - i, square[1] + i ])

  if checking == False: # Remove all available moves that lead to the current player putting themself in check.
    dels = []
    for i,move in enumerate(availibles):
      boardBackup = copy.deepcopy(board) # Use deepcopy to create a backup of the board which can be edited without affecting the original board.
      boardBackup = movePiece(square,move,boardBackup) # Make every possible availible move and then check if check is true
      if turn == "W":
        if KingCheck(boardBackup)[0] == True:
          dels.append(i)
      else:
        if KingCheck(boardBackup)[1] == True:
          dels.append(i)
    for item in dels[::-1]:
      del availibles[item]
    




  return availibles

checked = [False,False]

def selectPiece():
  pieceOnSpace = False
  while not pieceOnSpace:
    print("\nSelect a valid piece to move")
    try:
      row = int(input("Select a row to move a piece"))
    except:
      print("You need to enter an integer:")
      continue
    try:
      column = int(input("Select a file to move a piece"))
    except:
      print("You need to enter an integer")
      continue
    if turn == "W":
      row = 7-row
      column = 7 - column
    if row > 7 or row < 0 or column > 7 or column < 0:
      print("Enter an integer which corresponds to a board square. Remember indexing is from 0.")
      continue
    if board[row][column][0] == turn:
      pieceOnSpace = True
  oldPos = [row,column]
  print(board[row][column])
  valids = calculateValidMoves(oldPos,board,False)
  return [valids,oldPos]

def UserTurn():
  global turn
  global board
  global checked
  print("\nIt is " + turn + "'s turn")
  checked = KingCheck(board)
  if checked[0] == True:
    print("The white player is in check")
  elif checked[1] == True:
    print("The black player is in check.")
  if checked[0] == True or checked[1] == True:
    CheckMate(checked[0],checked[1],checked[2],checked[3])
  CheckStale(board)
 
  cancelled = True
  while cancelled == True:
    printBoard(board)
    selected = selectPiece()
    valids = selected[0]
    oldPos = selected[1]
    if valids == []:
      print("\nThis piece has no valid moves, please select another.")
      cancelled = True
      continue
    if turn == "W":
      for i,validMove in enumerate(valids):
        for j,item in enumerate(validMove):
          valids[i][j] = 7 - item
    choice = False
    print("Available options are:",valids)
    otherflag = False
    while not ( choice in valids ):
      try:
        row = int(input("Select a row to move this piece to:"))
        column = int(input("Select a file to move this piece to:"))
        if row > 7 or row < 0 or column > 7 or column < 0:
          print("Enter an integer which corresponds to a board square. Remember indexing is from 0.")
          continue
        choice = [row,column]
        cancelled = False
      except:
        print("You need to enter an integer.")
        if input("Would you like to choose another piece to move or continue with the same one? (another/same)") == "another":
          otherflag = True
          break
        
    if otherflag == True:
      continue

  if turn == "W":
    row = 7-row
    column = 7 - column
    choice = [row,column]
  board = movePiece(oldPos,choice,board)
  promotePawns()
  printBoard(board)
  if turn == "W":
    turn = "B"
  else:
    turn = "W"

def movePiece(oldPos,newPos,Inboard):
  if Inboard[oldPos[0]][oldPos[1]][1] == "K" and (newPos[1] - oldPos[1] == 2 or newPos[1] - oldPos[1] == -2 ): # Castling, making both moves when a castle happens.
      Inboard[newPos[0] ]   [newPos[1] ] = Inboard[oldPos[0]  ]  [oldPos[1]  ] 
      Inboard[oldPos[0]][oldPos[1]] = "  "
      print(newPos[1])
      if newPos[1] == 1:
        Inboard[newPos[0]] [2] = Inboard[oldPos[0]][0] 
        Inboard[oldPos[0]][0] = "  "
      elif newPos[1] == 5:
        Inboard[newPos[0] ] [4] = Inboard[oldPos[0]][7] 
        Inboard[oldPos[0]][7] = "  "
  else:
    Inboard[newPos[0] ]   [newPos[1] ] = Inboard[oldPos[0]  ]  [oldPos[1]  ] # Set the new space to the old space
    Inboard[oldPos[0]][oldPos[1]] = "  " # set the old space as blank/empty
  return Inboard

def KingCheck(INboard,BlackKingPosition=[],WhiteKingPosition=[]):
  if BlackKingPosition == []:#Find the kings if their positions are not supplied
    for i,row in enumerate(INboard):
      for j,piece in enumerate(row):
        if piece == "WK":
          WhiteKingPosition = [i,j]
        if piece == "BK":
          BlackKingPosition = [i,j]
  WhiteInCheck = False
  BlackInCheck = False
  for i,row in enumerate(INboard):#Check all pieces on board and if they are able to take opposit king, then check is true.
    for j,piece in enumerate(row):
      validMoves = calculateValidMoves([i,j],INboard,True)
      if BlackKingPosition in validMoves and piece[0] == "W":
        BlackInCheck = True
      if WhiteKingPosition in validMoves  and piece[0] == "B":
        WhiteInCheck = True
  return WhiteInCheck,BlackInCheck,WhiteKingPosition,BlackKingPosition

def CheckMate(WhiteInCheck,BlackInCheck,WhiteKingPosition,BlackKingPosition):  
  global board
  if WhiteInCheck == True:
    for i,row in enumerate(board):
      for j,piece in enumerate(row):
        if piece[0] == "W": # Find all white pieces and check if they can make a move which would result in no check. This includes blocking and taking the checking piece.
          for move in calculateValidMoves([i,j],board,False):
            backupBoard = copy.deepcopy(board)
            movePiece([i,j],move,backupBoard)
            if calculateValidMoves(WhiteKingPosition, backupBoard, False) != []: # Check if the king can move after making a potential blocking move
              return "No Checkmate"  
    if calculateValidMoves(WhiteKingPosition,board,False) == []: # If the king has no valid moves, and we already established you can't block, then the game ends.
      GameEnd("Black") # Black wins
  if BlackInCheck == True:
    for i,row in enumerate(board):
      for j,piece in enumerate(row):
        if piece[0] == "B":
          for move in calculateValidMoves([i,j],board,False):
            backupBoard = copy.deepcopy(board)
            movePiece([i,j],move,backupBoard)
            if calculateValidMoves(BlackKingPosition, backupBoard, False) != []:
              return "No Checkmate"
    if calculateValidMoves(BlackKingPosition,board,False) == []:
      GameEnd("White")

def GameEnd(result):
  print("The game has finished.")
  if result == "draw":
    print("The result is a draw.")
  else:
    print("CheckMate, The winner is " + result)
  sys.exit()

def CheckStale(INboard):
  global turn
  WhiteCanMove = False
  BlackCanMove = False
  for i,row in enumerate(INboard):
    for j,piece in enumerate(row):
      validMoves = calculateValidMoves([i,j],INboard,True)
      if validMoves != [] and board[i][j][0] == "W":
        WhiteCanMove = True
      if validMoves != [] and board[i][j][0] == "B":
        BlackCanMove = True
    if WhiteCanMove and BlackCanMove:
      break
  if WhiteCanMove ==False and turn == "W":
    GameEnd("draw")
  if BlackCanMove ==False and turn == "B":
    GameEnd("draw")
  
def promotePawns(chosenPiece="F"):
  global board
  global turn
  for i,row in enumerate(board):
    for j, piece in enumerate(row):
      if piece== "WP" and i == 7:
        while not chosenPiece in ["R","B","Q","N"]:
          chosenPiece = input("A pawn has reached the end, at square " + str([i,j]) + " . Select a new piece to promote to, using its inital.")
        board[i][j] = "W" + chosenPiece
      elif piece == "BP" and i==0:
        while not chosenPiece in ["R","B","Q","N"]:
          chosenPiece = input("A pawn has reached the end, at square " + str([i,j]) + " . Select a new piece to promote to, using its inital.")
        board[i][j] = "B" + chosenPiece
  if chosenPiece in ["R","B","Q","N"]:
    print("The " + turn + " player has promoted its pawn to a " + chosenPiece)


print("Pieces are selected using rows and then files, starting at 0. The highest is 7.")
print("Please remember, coordinates have been displayed as row-first.")

board = [["WR","WN","WB","WK","WQ","WB","WN","WR"],    # White
         ["WP","WP","WP","WP","WP","WP","WP","WP"],
         ["  ","  ","  ","  ","  ","  ","  ","  "],
         ["  ","  ","  ","  ","  ","  ","  ","  "],
         ["  ","  ","  ","  ","  ","  ","  ","  "],
         ["  ","  ","  ","  ","  ","  ","  ","  "],
         ["BP","BP","BP","BP","BP","BP","BP","BP"],
         ["BR","BN","BB","BK","BQ","BB","BN","BR"]] # Black


#board = [["  ","  ","  ","WK","  ","  ","  ","  "],    # White
#         ["BR","  ","  ","  ","  ","  ","  ","  "],
#         ["  ","  ","BR","  ","BR","  ","  ","  "],
#         ["  ","  ","  ","  ","  ","  ","  ","  "],
#         ["  ","  ","  ","  ","  ","  ","  ","  "],
#         ["  ","  ","  ","  ","  ","  ","  ","  "],
#         ["  ","  ","  ","  ","  ","  ","  ","  "],
#         ["  ","  ","  ","BK","  ","  ","  ","  "]] # Black
  

turn =  "W"
BlackKingMoved = False
BlackLeftRookMoved = False
BlackRightRookMoved = False
WhiteLeftRookMoved = False
WhiteRightRookMoved = False
WhiteKingMoved = False


def computerTurn(): # AI is based on a freeCodeCamp tutorial for JavaScript. It does not look any moves ahead, just makes the best possible move in terms of current material. I recognise, however, that this means it is extremely easy to beat it as you can bait its queen out with a pawn and have somthing else out in backup.
  global turn
  global board
  global checked
  print("\nIt is " + turn + "'s turn")
  checked = KingCheck(board)
  if checked[0] == True:
    print("The white player is in check")
  elif checked[1] == True:
    print("The black player is in check.")
  if checked[0] == True or checked[1] == True:
    CheckMate(checked[0],checked[1],checked[2],checked[3])
  CheckStale(board)
  printBoard(board)

  totalValids = {} #A dictionary of all the pieces the computer has, with their availible moves as the value.

  for i,row in enumerate(board):
    for j,piece in enumerate(row):
      if piece[0] == "B":
        validMoves = calculateValidMoves([i,j],board,False)
        totalValids[str(i) + str(j)] = validMoves
  
  best = []
  bestScore = -99999999
  for key in list(totalValids.keys()):
    for i,move in enumerate(totalValids[key]):
      boardBackup = copy.deepcopy(board)
      oldPos = [int(key[0]),int(key[1])]
      newBoard = movePiece(oldPos,move,boardBackup)
      blackTotal = 0
      whiteTotal = 0
      points = {"P" : 10,
                "N" : 30,
                "B" : 30,
                "R" : 50,
                "Q" : 90,
                "K" : 900 
               }
      for row in newBoard:
        for piece in row:
          if piece[0] == "W":
            whiteTotal += points[piece[1]]
          elif piece[0] == "B":
            blackTotal += points[piece[1]]
        
      newTotal = blackTotal - whiteTotal
      if newTotal > bestScore:
        bestScore = newTotal
        best = [key,i]

  oldPos = [int(best[0][0]),int(best[0][1])]
  newPos = totalValids[best[0]][best[1]]

  board = movePiece(oldPos,newPos,board)
  promotePawns("Q") # Promote any pawns that have reached the end to a queen.
  printBoard(board)
  if turn == "W":
    turn = "B"
  else:
    turn = "W"


gameType = input("Do you want to play against a computer or a friend?  (c/f)").lower()
while not (gameType == "c" or gameType == "f"):
  gameType = input("Do you want to play against a computer or a friend?  (c/f)")
if gameType == "c":
  while True:
    if turn == "W":
      UserTurn()
    else:
      computerTurn()
else:
    while True:
      UserTurn()
    
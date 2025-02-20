# ChessEngine
 Elby's Chess Engine that can beat any human (hopefully... eventually).

 Search Engine Features:
- Min Max Search
- Alpha Beta Pruning
- Transposition Table (TT)
- Search 'saved-best-move' first from TT
- Iterative Deepening

Current Branching Factor: 13-14 ((branching_factor)^(search_depth) = leaf_nodes_visited)
Current Performance: 1000kn/s (1,000,000 nodes per second)

How to Run
-
1. Run chess_engine.exe
2. Enter which colour you would like to play. (w = white, b = black)
3. Enter the depth the engine will search to for each move
4. Enter if you want the engine to print performance matrix
5. Enter move and play

Moves
  -
  I will not explain the rules of chess here. Learn how to play before continuing.
  - Coordinates will be given in modern algebraic chess coordinates. See 'Algebraic Chess Coordinates'.
  - Normal Moves - 'pe2e4'
    - first char 'p': piece type:
      - 'k' = King
      - 'q' = Queen
      - 'b' = bishop
      - 'n' =  Knight
      - 'r' = Rook
      - 'p' = Pawn
    - next two chars 'e2': Original coordinates of the piece. 
    - last two chars 'e4': New coordinates of the piece.
    - Pawn will move from the e2 square to e4 square.
  - Capture Moves - 'bg5xf6'
     - Everything is the same as a normal move except there has to be an 'x' between the orignal and new coordinates.
     - In this example, there has to be an enemy piece on the f6 square.
     - Same format is used for enpassant captures, in which case, the new coordinates will be an empty square.
   - Castle Moves
     - King Side Castle: 'O-O'
     - Queen Side Castle: 'O-O-O'
   - Promotion Moves - 'pe7e8=q'
     - Add '=' + piece letter (see Normal Moves above) at the end of a Normal Move.



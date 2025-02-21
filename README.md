# **ChessEngine**
 ### **Elby's Chess Engine that can beat any human (hopefully... eventually).**

---

### Search Engine Features

- Min Max Search
- Alpha Beta Pruning
- Transposition Table (TT)
- Search 'saved-best-move' first from TT
- Iterative Deepening

Current Branching Factor: 13-14 ((branching_factor)^(search_depth) = leaf_nodes_visited)
Current Performance: 1000kn/s (1,000,000 nodes per second)

----

### How to Run

1. Run chess_engine.exe
2. Enter which colour you would like to play. (w = white, b = black)
3. Enter the depth the engine will search to for each move
4. Enter if you want the engine to print performance matrix
5. Enter move and play

---

### Moves

  I will not explain the rules of chess here. Learn how to play before continuing
  - Coordinates will be given in modern algebraic chess coordinates. See 'Algebraic Chess Coordinates'
  - Normal Moves - 'pe2e4'
    - first char 'p': piece type:
      - 'k' = King
      - 'q' = Queen
      - 'b' = bishop
      - 'n' =  Knight
      - 'r' = Rook
      - 'p' = Pawn
    - next two chars 'e2': Original coordinates of the piece
    - last two chars 'e4': New coordinates of the piece
    - Pawn will move from the e2 square to e4 square
  - Capture Moves - 'bg5xf6'
     - Everything is the same as a normal move except there has to be an 'x' between the orignal and new coordinates
     - In this example, there has to be an enemy piece on the f6 square
     - Same format is used for enpassant captures, in which case, the new coordinates will be an empty square
   - Castle Moves
     - King Side Castle: 'O-O'
     - Queen Side Castle: 'O-O-O'
   - Promotion Moves - 'pe7e8=q'
     - Add '=' + piece letter (see Normal Moves above) at the end of a Normal Move
<br/><br/>
<br/><br/>

---
## **Development Environment Setup**


### Install Dependencies

   - Install a C++ compiler (e.g., GCC, Clang, MSVC)
   - Install CMake for build configuration
   - Use `clang-format` and `clang-tidy` for code formatting and static analysis. Install llvm version 18.1.8

### Hooks
  - Copy hooks/pre-commit to .git/hooks/pre-commit
    ```bash
    cp hooks/pre-commit .git/hooks/pre-commit
    ```

### Linting
  - Run on terminal:
    ```bash
    clang-format -i src/*.cpp include/*.h
    ```

### Static Analysis

  - Run CMake on a build dir to create compile_commands.json
  - Run on terminal:
    ```bash
    clang-tidy --fix -p build/ src/*.cpp include/*.h
    ```

### CI/CD Jobs
  - Linting: Will use clang-format and clang-tidy, both at v18.1.8. This job must pass before branch can be pushed to main
  - Build: Will build the project using CMake. This job must pass before branch can be pushed to main
  - chess_engine.exe artifact will be uploaded when Build is run on main branch.






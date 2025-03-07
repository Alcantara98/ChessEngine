# **ChessEngine**
 ### **Elby's Chess Engine that can beat any human (hopefully... eventually).**

---

### Search Engine Features
#### from v1.0.0
- Min Max Search
- Alpha Beta Pruning
- Transposition Table (TT)
- Search 'saved-best-move' first from TT
- Iterative Deepening

#### from v1.2.0
- Null Pruning

**Current Branching Factor**: 9-10 ((branching_factor)^(search_depth) = leaf_nodes_visited)
<br/><br/>
**Current Performance**: 1000-2000kn/s (1-2 million nodes per second)

----

### How to Run

1. Run chess_engine.exe.
2. Enter 'y' if you want to play the engine.
3. Engine will ask for engine settings.
4. Enter a move when it is your turn (See [Moves](#moves)).
5. You can also enter 'help' at any time to see all possible commands.

---

### Moves

  I will not explain the rules of chess here. Learn how to play before continuing.
  - Coordinates will be given in modern algebraic chess coordinates. See 'Algebraic Chess Coordinates'.
  - Normal Moves - 'pe2e4'
    - First char 'p': piece type:
      - 'k' = King
      - 'q' = Queen
      - 'b' = bishop
      - 'n' =  Knight
      - 'r' = Rook
      - 'p' = Pawn
    - Next two chars 'e2': Original coordinates of the piece.
    - Last two chars 'e4': New coordinates of the piece.
    - Pawn will move from the e2 square to e4 square.
  - Capture Moves - 'bg5xf6'
     - Everything is the same as a normal move except there has to be an 'x' between the orignal and new coordinates.
     - In this example, there has to be an enemy piece on the f6 square.
     - Same format is used for en passant captures, in which case, the new coordinates will be an empty square.
   - Castle Moves
     - King Side Castle: 'O-O'
     - Queen Side Castle: 'O-O-O'
   - Promotion Moves - 'pe7e8=q'
     - Add '=' + piece letter (see Normal Moves above) at the end of a Normal Move.
<br/><br/>
<br/><br/>

---
## **Development Environment Setup**


### Install Dependencies

   - Install CMake for build configuration.
   - Set compiler cmake -DCMAKE_C_COMPILER="C:/path/to/compiler" -DCMAKE_CXX_COMPILER="C:/path/to/compiler++"
   - Use `clang-format` and `clang-tidy` for code formatting and static analysis. Install llvm version 18.1.8. See here: https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8

### Hooks
  - Copy hooks/pre-commit to .git/hooks/pre-commit.
    ```bash
    cp hooks/pre-commit .git/hooks/pre-commit
    ```

### Linting
  - Run on terminal:
    ```bash
    clang-format -i src/*.cpp include/*.h
    ```

### Static Analysis

  - Run CMake on a build dir to create compile_commands.json.
  - Run on terminal:
    ```bash
    clang-tidy --fix -p build/ src/*.cpp include/*.h
    ```

### CI/CD Jobs
  - Linting: Will use clang-format and clang-tidy, both at v18.1.8. This job must pass before branch can be pushed to main.
  - Build: Will build the project using CMake. This job must pass before branch can be pushed to main.
  - chess_engine.exe artifact will be uploaded when Build is run on main branch.






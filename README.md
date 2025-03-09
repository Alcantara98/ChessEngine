# **ChessEngine** ♟️
### **Elby's Chess Engine that can beat any human (hopefully... eventually).** 🤖

---

## **Search Engine Features** 🔭

### **v1.0.0** 🛠️
- ~~Min Max Search~~ (deprecated by NegaMax)
- Alpha Beta Pruning
- Transposition Table (TT)
- Search 'saved-best-move' first from TT
- Iterative Deepening
- **Performance**:
  - Branching Factor: `13-14`
  - Speed: `1000-2000 kN/s`

### **v1.2.0** 🛠️
- Null Move Pruning
- NegaMax Search
- Improved Command Line Interface
- **Performance**:
  - Branching Factor: `9-10`
  - Speed: `1000-2000 kN/s`

### **v1.3.0** 
- Lockless Transposition Table
- **Performance**:
  - Branching Factor: `9-10`
  - Speed: `8000-9000 kN/s`

**Current Branching Factor**: `9-10` ((branching_factor)^(search_depth) = leaf_nodes_visited)
<br/>
**Current Performance**: `8000-9000 kN/s` (8-9 million nodes per second)

---

## **How to Run** 🏃‍♂️

1. **Run** `chess_engine.exe`.
2. **Enter** `'y'` if you want to play the engine.
3. **Configure** engine settings as prompted.
4. **Make a move** when it is your turn (See [Moves](#moves)).
5. **Type** `'help'` at any time to see all possible commands.

---

## **Moves** 💃

I will not explain the rules of chess here. Learn how to play before continuing.
- Coordinates will be given in modern algebraic chess coordinates. See 'Algebraic Chess Coordinates'.
- **Normal Moves** - `'pe2e4'`
  - First char `'p'`: piece type:
    - `'k'` = King
    - `'q'` = Queen
    - `'b'` = Bishop
    - `'n'` = Knight
    - `'r'` = Rook
    - `'p'` = Pawn
  - Next two chars `'e2'`: Original coordinates of the piece.
  - Last two chars `'e4'`: New coordinates of the piece.
  - Pawn will move from the e2 square to e4 square.
- **Capture Moves** - `'bg5xf6'`
  - Everything is the same as a normal move except there has to be an `'x'` between the original and new coordinates.
  - In this example, there has to be an enemy piece on the f6 square.
  - The same format is used for en passant captures, in which case, the new coordinates will be an empty square.
- **Castle Moves**
  - King Side Castle: `'O-O'`
  - Queen Side Castle: `'O-O-O'`
- **Promotion Moves** - `'pe7e8=q'`
  - Add `'='` + piece letter (see Normal Moves above) at the end of a Normal Move.

---

## **Development Environment Setup** 🛠️

### **Install Dependencies** 📦

- Install CMake for build configuration.
- Set compiler:
  ```bash
  cmake -DCMAKE_C_COMPILER="C:/path/to/compiler" -DCMAKE_CXX_COMPILER="C:/path/to/compiler++"
  ```
- Use `clang-format` and `clang-tidy` for code formatting and static analysis. Install LLVM version 18.1.8. See here: [LLVM Project Releases](https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8)

### **Hooks** 🔗

- Copy hooks/pre-commit to .git/hooks/pre-commit.
  ```bash
  cp hooks/pre-commit .git/hooks/pre-commit
  ```

### **Linting** 🧹

- Run on terminal:
  ```bash
  clang-format -i src/*.cpp include/*.h
  ```

### **Static Analysis** 🔍

- Run CMake on a build directory to create compile_commands.json.
- Run on terminal:
  ```bash
  clang-tidy --fix -p build/ src/*.cpp include/*.h
  ```

### **CI/CD Jobs** ⚙️

- **Linting**: Will use clang-format and clang-tidy, both at v18.1.8. This job must pass before a branch can be pushed to main.
- **Build**: Will build the project using CMake. This job must pass before a branch can be pushed to main.
- **Artifact**: `chess_engine.exe` artifact will be uploaded when Build is run on the main branch.






# **LiYing** ♟️
### **Elby's Chess Engine that can beat any human (hopefully... eventually).** 🤖

---

## **Search Engine Features** 🔭

#### Speed is measured using an M2 Macbook Air during the Middle Game.
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

### **v1.4.1**
- Aspiration Window
- Move-Ordering - Capture Moves First
- MVV-LVV Move Ordering
- History Heuristic
- Late Move Reduction
- Quiescence Search
- Futility Pruning
- Delta Pruning
- Pondering - Search during Player's Turn
- **Performance**:
  - Branching Factor: `3.5-4.5`
  - Speed: `5000-6000 kN/s`

**Current Branching Factor**: `3.5-4.5` ((branching_factor)^(search_depth) = leaf_nodes_visited)
<br/>
**Current Performance**: `5000-6000 kN/s` (5-6 million nodes per second)

---

## **How to Run** 🏃‍♂️

1. **Build and Run Locally (Cross-Platform)**
   Use the provided Docker setup to build and run the chess engine container. See the [Docker Setup](#chess-engine-docker-setup) section for detailed instructions.

2. **Run on Windows**
   Execute the prebuilt binary `chess_engine.exe` available as an artifact from the CI/CD pipeline.

3. **Run on ARM64**
   Use the prebuilt binary `chess_engine` available as an artifact from the CI/CD pipeline.

---

## **Command Options** 🛠️

1. **Enter** `'y'` if you want to play the engine.
2. **Configure** engine settings as prompted.
3. **Make a move** when it is your turn (See [Moves](#moves)).
4. **Type** `'help'` at any time to see all possible commands.

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

## **Chess Engine Docker Setup** 🧠

This repository includes a simple Bash script to build and run a Docker container for your chess engine.

### **Prerequisites** 📦

Before you can build and run the container, make sure you have:

- **[Docker](https://www.docker.com/)** installed and running (version 20.10+ recommended).
- A **Unix-like shell environment** with `bash` (macOS, Linux, or [WSL](https://learn.microsoft.com/en-us/windows/wsl/) on Windows).

### **Usage (Linux/macOS/WSL)** 🚀

To build and run the chess engine container:

```bash
./run_chess_engine.sh
```

The script will:

1. Check if the Docker image `chess-engine` already exists.
2. Prompt you to rebuild it (optional).
3. Run the container interactively.

### **One-liner for Windows CMD or PowerShell** 💡

If you're on Windows and not using WSL or Bash, you can build and run the container with this one-liner:

```
docker build -t chess-engine . && docker run --rm -it chess-engine
```
---

## **Development Environment Setup** 🛠️

### **Install Dependencies** 📦

- Install CMake for build configuration.
- Set compiler:
  ```bash
  cmake -DCMAKE_C_COMPILER="C:/path/to/compiler" -DCMAKE_CXX_COMPILER="C:/path/to/compiler++"
  ```
- Use `clang-format` and `clang-tidy` for code formatting and static analysis. Project uses LLVM version 18.1.8. See here to install: [LLVM Project Releases](https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8)

### **Hooks** 🔗

- Copy hooks/pre-commit to .git/hooks/pre-commit.
  ```bash
  cp hooks/pre-commit .git/hooks/pre-commit
  ```

### **Linting** 🧹

- Run on terminal:
  ```bash
  clang-format -i src/*.cpp src/*.h
  ```

### **Static Analysis** 🔍

- Run CMake on a build directory to create compile_commands.json.
- Run on terminal:
  ```bash
  clang-tidy --fix -p build/ src/*.cpp src/*.h
  ```

### **CI/CD Jobs** ⚙️

- **Linting**: Will use clang-format and clang-tidy, both at v18.1.8. This job must pass before a branch can be pushed to main.
- **Build**: Will build the project using CMake-MinGW-GCC/G++. This job must pass before a branch can be pushed to main.
- **Artifact**: `chess_engine.exe` artifact will be uploaded when Build is run on the main branch.






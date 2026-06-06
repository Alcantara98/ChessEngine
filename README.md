# **LiYing** ♟️
### **Elby's Chess Engine that can beat any human (hopefully... eventually).** 🤖

---

## **Search Engine Features** 🔭

### **v1.5.0**
- Alpha Beta Pruning (Negamax)
- Iterative Deepening
- PV Search
- MVV-LVV Move Ordering
- History Table Move Ordering
- Late Move Reduction
- Quiescence Search
- Futility Pruning
- Delta Pruning
- Null Move Pruning
- Transposition Table
- Pondering
- Lazy SMP Threading

**Current Branching Factor**: `3-4`  (Single Thread)
<br>
(`branching_factor`)^(search_depth) = leaf_nodes_visited

---

## **Build and Run** 🏃‍♂️

### **Prerequisites** 📦

Before you can build and run the container, make sure you have:
- **[Docker](https://www.docker.com/)** installed and running (version 29.0.0+ recommended).
- A Unix-like shell environment with `bash` (macOS, Linux, or [WSL](https://learn.microsoft.com/en-us/windows/wsl/) on Windows).

### **Usage** 🚀

- Run the following command to build and run the engine:
```bash
./dev/docker/run_chess_engine/run_chess_engine.sh
```

---

## **CLI Application Usage** 🛠️

1. **Enter** `'y'` if you want to play the engine.
2. **Configure** engine settings as prompted.
3. **Make a move** when it is your turn (See [Moves](#moves)).
4. **Type** `'help'` at any time to see all possible commands.

---

## **Moves** 💃

I will not explain the rules of chess here. Learn how to play before continuing.
- **Coordinates** will be given in modern algebraic chess coordinates. See 'Algebraic Chess Coordinates'.
- **Normal Moves** - `e2e4`
  - Next two chars `e2`: Original coordinates of the piece.
  - Last two chars `e4`: New coordinates of the piece.
  - Piece will move from the `e2` square to `e4` square.
  - `e4` square must be empty, and `e2` square must have one of your pieces.
- **Capture Moves** - `g5f6`
  - Everything is the same as a normal move except there has to be an enemy piece on the new coordinates.
  - In this example, there has to be an enemy piece on the `f6` square and `g5` square must have one of your pieces.
  - The same format is used for en passant captures, in which case, the new coordinates will be an empty square.
- **Castle Moves** - `e1g1`
  - Treat castle moves as the king moving to the new coordinates.
  - The given example is a white short castle move.
- **Promotion Moves** - `e7e8q`
  - The given example is a white pawn promotion to a queen.

---

## **Dev Container Setup** 🛠️

### **Prerequisites** 📦

See [Prerequisites](#prerequisites).

### **Usage** 🚀
- Run the following command to create container:
```bash
./dev/docker/dev_env/run_build_container.sh
``` 
- Run the following command to build chess CLI and UCI binaries:
```bash
make build
```
- See `Makefile` to see linting and other build options.

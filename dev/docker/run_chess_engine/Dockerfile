# Runner image for the ChessEngine project.
#
# Multi-stage build:
#   1. `builder` compiles the engine via `make build` using Clang/LLVM.
#   2. `runtime` ships only the produced binary on a minimal Ubuntu base.
#
# Build context is the project root, e.g.:
#   docker build -f dev/docker/run_chess_engine/Dockerfile -t chess-engine .

# ---------- Stage 1: builder ----------
FROM ubuntu:24.04 AS builder

ARG LLVM_VERSION=18

ENV DEBIAN_FRONTEND=noninteractive

# Retry `apt-get update` to ride out transient Ubuntu mirror sync failures.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        cmake \
        make \
        clang-${LLVM_VERSION} \
        lld-${LLVM_VERSION} \
        llvm-${LLVM_VERSION} \
    && rm -rf /var/lib/apt/lists/*

# Create symlinks for the LLVM versioned binaries.
RUN update-alternatives --install /usr/bin/clang   clang   /usr/bin/clang-${LLVM_VERSION}   100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 100

WORKDIR /src
COPY CMakeLists.txt ./
COPY src ./src
COPY Makefile ./

RUN make build

# ---------- Stage 2: runtime ----------
FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# libstdc++ is the only runtime dependency for the Clang-built binary.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/chess_engine /usr/local/bin/chess_engine

ENTRYPOINT ["/usr/local/bin/chess_engine"]

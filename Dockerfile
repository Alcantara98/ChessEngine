# Stage 1: Build environment
FROM debian:bookworm-slim AS builder

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    make \
    g++ \
    && \
    rm -rf /var/lib/apt/lists/*

# Copy project files
WORKDIR /ChessEngine
COPY . .

# Build the project using CMake
RUN mkdir build && \
    cd build && \
    cmake -DASAN=OFF -DRELEASE=ON .. && \
    cmake --build .

# Stage 2: Runtime image (minimal, optional)
FROM debian:bookworm-slim AS runtime

# Copy only the built binaries (no build tools)
COPY --from=builder /ChessEngine/build/chess_engine /usr/local/bin/

# Entrypoint
CMD ["/usr/local/bin/chess_engine"]
#!/usr/bin/env bash
set -eo pipefail

IMAGE_NAME="chess-engine"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
DOCKERFILE="${SCRIPT_DIR}/Dockerfile"

# Function to check if the Docker image exists
image_exists() {
  docker image inspect "$IMAGE_NAME" &> /dev/null
}

# Function to build the image
build_image() {
  echo "Building Docker image '$IMAGE_NAME'..."
  docker build -f "$DOCKERFILE" -t "$IMAGE_NAME" "$PROJECT_ROOT"
}

# Function to run the container
run_container() {
  echo "Starting container from image '$IMAGE_NAME'..."
  docker run --rm -it "$IMAGE_NAME"
}

# Main logic
if image_exists; then
  read -rp "Image '$IMAGE_NAME' already exists. Rebuild? (y/N): " REBUILD
  if [[ "$REBUILD" =~ ^[Yy]$ ]]; then
    build_image
  else
    echo "Using existing image."
  fi
else
  build_image
fi

run_container
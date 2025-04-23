#!/usr/bin/env bash
set -eo pipefail

IMAGE_NAME="chess-engine"

# Function to check if the Docker image exists
image_exists() {
  docker image inspect "$IMAGE_NAME" &> /dev/null
}

# Function to build the image
build_image() {
  echo "Building Docker image '$IMAGE_NAME'..."
  docker build -t "$IMAGE_NAME" .
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
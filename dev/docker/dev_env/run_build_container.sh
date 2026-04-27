#!/usr/bin/env bash
# Build and start the ChessEngine dev container.
#
# The container is started detached with `sleep infinity` so it can be
# attached to from VSCode / Cursor via the Dev Containers extension
# ("Attach to Running Container...").

set -eo pipefail

IMAGE_NAME="chess-engine-dev"
CONTAINER_NAME="chess-engine-dev"

# Resolve the project root (two levels up from this script's directory:
# dev/docker/dev_enc/ -> dev/docker/ -> dev/ -> <repo root>).
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"

HOST_UID="$(id -u)"
HOST_GID="$(id -g)"
HOST_USERNAME="$(id -un)"

image_exists() {
  docker image inspect "${IMAGE_NAME}" &> /dev/null
}

container_exists() {
  docker container inspect "${CONTAINER_NAME}" &> /dev/null
}

container_running() {
  [[ "$(docker container inspect -f '{{.State.Running}}' "${CONTAINER_NAME}" 2> /dev/null)" == "true" ]]
}

build_image() {
  echo "Building Docker image '${IMAGE_NAME}'..."
  docker build \
    --build-arg "USER_UID=${HOST_UID}" \
    --build-arg "USER_GID=${HOST_GID}" \
    -t "${IMAGE_NAME}" \
    -f "${SCRIPT_DIR}/Dockerfile" \
    "${SCRIPT_DIR}"
}

start_container() {
  if container_exists; then
    if container_running; then
      echo "Container '${CONTAINER_NAME}' is already running."
    else
      echo "Starting existing container '${CONTAINER_NAME}'..."
      docker start "${CONTAINER_NAME}" > /dev/null
    fi
  else
    echo "Creating and starting container '${CONTAINER_NAME}'..."
    docker run -d \
      --name "${CONTAINER_NAME}" \
      --hostname "${CONTAINER_NAME}" \
      -v "${HOME}:${HOME}" \
      -v "${PROJECT_ROOT}:/workspaces/ChessEngine" \
      -w /workspaces/ChessEngine \
      "${IMAGE_NAME}"
  fi
}

if image_exists; then
  read -rp "Image '${IMAGE_NAME}' already exists. Rebuild? (y/N): " REBUILD
  if [[ "${REBUILD}" =~ ^[Yy]$ ]]; then
    if container_exists; then
      echo "Removing existing container '${CONTAINER_NAME}' before rebuild..."
      docker rm -f "${CONTAINER_NAME}" > /dev/null
    fi
    build_image
  else
    echo "Using existing image."
  fi
else
  build_image
fi

start_container

cat <<EOF

Dev container '${CONTAINER_NAME}' is up.

Attach from VSCode / Cursor:
  1. Install the "Dev Containers" extension (ms-vscode-remote.remote-containers).
  2. Open the Command Palette and run:
       "Dev Containers: Attach to Running Container..."
  3. Pick '${CONTAINER_NAME}', then open /workspaces/ChessEngine.

Or drop into a shell directly:
  docker exec -it ${CONTAINER_NAME} bash
EOF

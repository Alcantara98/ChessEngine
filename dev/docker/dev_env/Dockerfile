# Dev container for the ChessEngine project.
#
# Provides Clang 18 + clang-format/clang-tidy/clangd, CMake,
# and common dev utilities.
#
# Use extensions (Dev Containers / Remote - Containers) 
# to attach to the container from VSCode / Cursor.

FROM ubuntu:24.04

ARG LLVM_VERSION=18
ARG USER_UID=1000
ARG USER_GID=${USER_UID}
ARG USERNAME=master-elby
# Host home directory, used to source the host's .bashrc from inside the
# container. Pass via --build-arg HOST_HOME="${HOME}". A sentinel default
# is used so the build still succeeds when the arg is omitted; the source
# line in the container's .bashrc is guarded by `[ -f ... ]`.
ARG HOST_HOME=/nonexistent

ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        wget \
        sudo \
        git \
        ssh \
        make \
        cmake \
        ninja-build \
        pkg-config \
        python3 \
        python3-pip \
        gdb \
        valgrind \
        less \
        nano \
        vim \
        bash-completion \
        locales \
        clang-${LLVM_VERSION} \
        clang-format-${LLVM_VERSION} \
        clang-tidy-${LLVM_VERSION} \
        clangd-${LLVM_VERSION} \
        lld-${LLVM_VERSION} \
        lldb-${LLVM_VERSION} \
        llvm-${LLVM_VERSION} \
    && rm -rf /var/lib/apt/lists/*

# Make the versioned LLVM tools available under their unversioned names so
# editors / scripts that look for `clang`, `clangd`, etc. just work. (Creates symlinks)
RUN update-alternatives --install /usr/bin/clang        clang        /usr/bin/clang-${LLVM_VERSION}        100 && \
    update-alternatives --install /usr/bin/clang++      clang++      /usr/bin/clang++-${LLVM_VERSION}      100 && \
    update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-${LLVM_VERSION} 100 && \
    update-alternatives --install /usr/bin/clang-tidy   clang-tidy   /usr/bin/clang-tidy-${LLVM_VERSION}   100 && \
    update-alternatives --install /usr/bin/clangd       clangd       /usr/bin/clangd-${LLVM_VERSION}       100 && \
    update-alternatives --install /usr/bin/lld          lld          /usr/bin/lld-${LLVM_VERSION}          100 && \
    update-alternatives --install /usr/bin/lldb         lldb         /usr/bin/lldb-${LLVM_VERSION}         100 && \
    update-alternatives --install /usr/bin/llvm-ar      llvm-ar      /usr/bin/llvm-ar-${LLVM_VERSION}      100 && \
    update-alternatives --install /usr/bin/llvm-ranlib  llvm-ranlib  /usr/bin/llvm-ranlib-${LLVM_VERSION}  100

ENV CC=clang \
    CXX=clang++

# Reuse the default `ubuntu` user that ships with Ubuntu 24.04 as the dev
# user, renaming it to ${USERNAME} and adjusting its UID/GID to match the
# host so bind-mounted source files keep correct ownership.
RUN groupmod --gid ${USER_GID} --new-name ${USERNAME} ubuntu && \
    usermod  --uid ${USER_UID} --gid ${USER_GID} \
             --login ${USERNAME} \
             --home /home/${USERNAME} --move-home ubuntu && \
    chown -R ${USER_UID}:${USER_GID} /home/${USERNAME}

# Grant the dev user passwordless sudo.
RUN echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} && \
    chmod 0440 /etc/sudoers.d/${USERNAME}

USER ${USERNAME}
WORKDIR /workspaces/ChessEngine

# Source the host's .bashrc (made available via the bind-mount of the host
# HOME in run_build_container.sh) from the container user's .bashrc. The
# `[ -f ... ]` guard makes this a no-op if the host home isn't mounted, and
# `|| true` keeps shell startup from aborting if the host bashrc references
# tools that don't exist inside the container.
RUN printf '\n# Source host .bashrc if mounted.\n[ -f "%s/.bashrc" ] && . "%s/.bashrc" 2>/dev/null || true\n' \
        "${HOST_HOME}" "${HOST_HOME}" >> /home/${USERNAME}/.bashrc

# Keep container running
CMD ["sleep", "infinity"]

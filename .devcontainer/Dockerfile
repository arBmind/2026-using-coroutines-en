FROM mcr.microsoft.com/devcontainers/javascript-node:1-22-bookworm

ARG LLVM_VERSION=22
ARG \
  APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1 \
  DEBIAN_FRONTEND=noninteractive

RUN <<LLVM_REPOSITORY_EOF
  set -e
  apt-get -qq update -o=Dpkg::Use-Pty=0
  apt-get -qq --yes install -o=Dpkg::Use-Pty=0 --no-install-recommends \
    ca-certificates \
    curl \
    gnupg
  curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key \
    | gpg --dearmor --yes -o /usr/share/keyrings/llvm-archive-keyring.gpg
  echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] https://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-${LLVM_VERSION} main" \
    > /etc/apt/sources.list.d/llvm-${LLVM_VERSION}.list
LLVM_REPOSITORY_EOF

RUN <<INSTALL_EOF
  set -e
  apt-get -qq update -o=Dpkg::Use-Pty=0
  apt-get -qq --yes upgrade -o=Dpkg::Use-Pty=0
  apt-get -qq --yes install -o=Dpkg::Use-Pty=0 --no-install-recommends \
    build-essential \
    ca-certificates \
    clang-${LLVM_VERSION} \
    clang-format-${LLVM_VERSION} \
    clangd-${LLVM_VERSION} \
    cmake \
    curl \
    fonts-noto \
    fonts-noto-cjk \
    fonts-noto-color-emoji \
    git \
    libasound2 \
    libatk-bridge2.0-0 \
    libatk1.0-0 \
    libcups2 \
    libdrm2 \
    libgbm1 \
    libgtk-3-0 \
    libnss3 \
    libxcomposite1 \
    libxdamage1 \
    libxfixes3 \
    libxkbcommon0 \
    libxrandr2 \
    libc++-${LLVM_VERSION}-dev \
    libc++abi-${LLVM_VERSION}-dev \
    lld-${LLVM_VERSION} \
    ninja-build \
    xdg-utils
  update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION} 220
  update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 220
  update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-${LLVM_VERSION} 220
  update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-${LLVM_VERSION} 220
  apt-get -qq --yes autoremove -o=Dpkg::Use-Pty=0
  apt-get -qq clean autoclean -o=Dpkg::Use-Pty=0
  rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*
INSTALL_EOF

RUN <<PREPARE_EOF
  set -e
  mkdir -p \
    /workspaces/2026-11_UsingCoroutines/node_modules \
    /home/node/.cache/ms-playwright \
    /home/node/.npm
  chown -R node:node /workspaces /home/node
PREPARE_EOF

USER node

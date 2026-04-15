
FROM ubuntu:24.04 AS base

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    cmake \
    g++ \
    gcc \
    git \
    libudev-dev \
    make \
    mingw-w64 \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY . .

FROM base AS linux-builder

RUN cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libstdc++ -static-libgcc" && \
    cmake --build build-linux --config Release --parallel

FROM base AS windows-builder

RUN cmake -S . -B build-windows \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libstdc++ -static-libgcc" && \
    cmake --build build-windows --config Release --parallel

FROM alpine:3.21 AS artifacts

WORKDIR /out

COPY --from=linux-builder /src/build-linux/src/astra-update /out/linux/astra-update
COPY --from=linux-builder /src/build-linux/src/astra-boot /out/linux/astra-boot
COPY --from=windows-builder /src/build-windows/src/astra-update.exe /out/windows/astra-update.exe
COPY --from=windows-builder /src/build-windows/src/astra-boot.exe /out/windows/astra-boot.exe

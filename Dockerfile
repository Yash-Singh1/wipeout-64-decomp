FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        binutils-mips-linux-gnu \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work

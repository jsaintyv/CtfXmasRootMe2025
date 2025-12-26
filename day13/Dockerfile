# Wii Root-Me Edition — internal building tool
# WARNING: DO NOT DISTRIBUTE OUTSIDE DEV TEAM
FROM debian:bookworm

# --- Build & signing deps ---
RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    python3 python3-pip python3-pycryptodome \
    openssl libssl-dev \
    binutils libtiff-tools imagemagick python3-opencv libtiff-dev  libpng-dev 
 
# --- Final output directory ---
WORKDIR /app

VOLUME ["/app"]

# Done.
CMD ["/bin/bash"]

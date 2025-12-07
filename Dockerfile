FROM alpine as build

RUN apk add --no-cache \
    g++ \
    make \
    cmake \
    git \
    linux-headers \
    openssl-dev \
    opus-dev \
    libsodium-dev

COPY . /app

WORKDIR /app

RUN mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . --target discord_cpp -j$(nproc --all) && \
    cmake --build . --target test_bot -j$(nproc --all)

FROM alpine

RUN apk add --no-cache \
    openssl \
    opus \
    libsodium \
    ffmpeg \
    yt-dlp


COPY --from=build /app/build/libdiscord_cpp.so /usr/local/lib
RUN ldconfig /usr/local/lib

RUN mkdir /app
COPY --from=build /app/build/test_bot /app

WORKDIR /app

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["./test_bot"]

FROM alpine as build

RUN apk add \
	g++ \
	make \
	cmake \
	git \
	zlib-dev \
	opus-dev \
	libsodium-dev \
	boost1.82-dev \
	openssl-dev

COPY . /app

WORKDIR /app

RUN mkdir build && \
	cd build && \
	cmake .. && \
	cmake --build . --target discord_cpp -j$(nproc --all) && \
	cmake --build . --target test_bot -j$(nproc --all)

FROM alpine

RUN apk add \
	zlib \
	opus \
	libsodium \
	boost1.82 \
	boost1.82-filesystem \
	openssl \
	ffmpeg \
	py3-pip

RUN pip install youtube_dl
RUN pip cache purge

COPY --from=build /app/build/libdiscord_cpp.so /usr/local/lib
RUN ldconfig /usr/local/lib

RUN mkdir /app
COPY --from=build /app/build/test_bot /app

WORKDIR /app

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["./test_bot"]

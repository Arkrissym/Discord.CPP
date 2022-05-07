FROM debian:bullseye-slim as build

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential \
	make \
	cmake \
	git \
	zlib1g-dev \
	libopus-dev \
	libsodium-dev \
	libboost1.74-dev \
	libboost-filesystem1.74-dev \
	libssl-dev

COPY . /app

WORKDIR /app

RUN mkdir build && \
	cd build && \
	cmake .. && \
	cmake --build . --target discord_cpp -j$(nproc --all) && \
	cmake --build . --target test_bot -j$(nproc --all)
#	make -j8 discord_cpp && \
#	make test_bot

FROM debian:bullseye-slim

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
	zlib1g \
	libopus0 \
	libsodium23 \
	libboost1.74 \
	libboost-filesystem1.74 \
	libssl1.1 \
	ffmpeg \
	python3-pip
RUN apt-get clean

RUN pip install youtube_dl
RUN pip cache purge

COPY --from=build /app/build/libdiscord_cpp.so /usr/local/lib
RUN ldconfig

RUN mkdir /app
COPY --from=build /app/build/test_bot /app

WORKDIR /app

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["./test_bot"]

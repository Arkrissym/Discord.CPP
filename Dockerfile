FROM debian:bullseye-slim as build

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential \
	make \
	cmake \
	libcpprest-dev \
	nlohmann-json-dev \
	libopus-dev \
	libsodium-dev

COPY . /app

WORKDIR /app

RUN mkdir build && \
	cd build && \
	cmake .. && \
	make -j8 discord_cpp && \
	make test_bot

FROM debian:bullseye-slim

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install \
	libcpprest \
	libopus0 \
	libsodium23 \
	libboost-filesystem1.74 \
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

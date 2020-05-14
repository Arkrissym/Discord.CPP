FROM ubuntu

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential \
	make \
	cmake \
	libcpprest-dev \
	libopus-dev \
	libsodium-dev \
	ffmpeg

COPY . /app

WORKDIR /app

RUN mkdir build && \
	cd build && \
	cmake .. && \
	make -j4 && \
	make install && \
	ldconfig && \
	cd ..

WORKDIR /app/test_bot

RUN g++ -Wall -o test main.cpp -ldiscord_cpp -lcpprest -lpthread -lssl -lcrypto

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["/app/test_bot/test"]

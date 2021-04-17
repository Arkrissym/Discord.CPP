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
	make -j8 discord_cpp && \
	make install discord_cpp && \
	ldconfig && \
	cd ..

RUN cd build && \
	make test_bot && \
	make install test_bot

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["test_bot"]

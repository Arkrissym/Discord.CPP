FROM ubuntu

RUN apt-get update && apt-get -y install build-essential \
	make \
	libcpprest-dev \
	libopus-dev \
	libsodium-dev

COPY . /app

WORKDIR /app

RUN make -j4
RUN make install

RUN make test

ARG token
ENV DISCORD_TEST_TOKEN=$token

ENTRYPOINT ["/app/test"]

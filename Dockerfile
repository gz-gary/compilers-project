FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y -qq \
    build-essential \
    flex \
    bison \
    libbison-dev \
    gcc-7 \
    gdb \
    git \
    vim \
    zip \
    python3.8 \
    python3-pip \
    python2-minimal && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100

WORKDIR /workspace
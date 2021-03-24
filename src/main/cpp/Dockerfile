FROM phusion/baseimage:master

#from https://www.gungorbudak.com/blog/2018/06/13/memory-leak-testing-with-valgrind-on-macos-using-docker-containers/
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y g++ valgrind \
  build-essential \
  cmake \
  git \
  libssl-dev \
  sudo \
  wget \
  maven \
  default-jdk \
  libgflags-dev \
  libgmp-dev \
  libpq-dev \
  libboost-all-dev \
  postgresql \
  postgresql-contrib \
  postgresql-client \
  python \
  openssh-server \
  libprotobuf-dev \
  protobuf-c-compiler \
  tmux
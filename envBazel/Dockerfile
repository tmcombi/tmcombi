# This is a tmcombi build env Dockerfile

FROM l.gcr.io/google/bazel

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
  && apt-get install -y wget libboost-all-dev

RUN cd /usr/local/src && wget https://github.com/google/googletest/archive/release-1.10.0.tar.gz \
  && tar xfz release-1.10.0.tar.gz \
  && rm release-1.10.0.tar.gz \
  && mv googletest-* googletest

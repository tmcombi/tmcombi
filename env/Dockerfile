# This is a tmcombi build env Dockerfile

FROM l.gcr.io/google/bazel

RUN apt-get update \
  && apt-get install -y wget g++

RUN cd /home && wget https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.gz \
  && tar xfz boost_1_73_0.tar.gz \
  && rm boost_1_73_0.tar.gz \
  && cd boost_1_73_0 \
  && ./bootstrap.sh --prefix=/usr/local --with-libraries=program_options \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_73_0


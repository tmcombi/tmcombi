# This is a tmcombi build env Dockerfile

# FROM ubuntu

# RUN apt-get update && \
#     apt-get install -y wget gnupg g++ unzip zip

# RUN wget https://bazel.build/bazel-release.pub.gpg && \
#     apt-key add bazel-release.pub.gpg && \
#     echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
#     apt-get update && \
#     apt-get -y install bazel

FROM l.gcr.io/google/bazel


RUN apt-get update \
  && apt-get install -y wget

RUN cd /usr/local && wget https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.gz \
  && tar vxfz boost_1_73_0.tar.gz \
  && rm boost_1_73_0.tar.gz

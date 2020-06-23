# This is a tmcombi build env Dockerfile

FROM ubuntu

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y g++ ccache cmake libboost-all-dev

RUN sudo useradd -m jenkins

CMD ["cmake"]

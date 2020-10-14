# This is a tmcombi build env Dockerfile

FROM ubuntu

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y g++ ccache cmake libboost-all-dev git

RUN git clone https://github.com/nushoin/RTree.git
RUN git clone https://github.com/Sergey-Grosman/DynDimRTree.git

# RUN useradd -m jenkins

CMD ["cmake"]

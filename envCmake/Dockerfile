# This is a tmcombi build env Dockerfile

FROM gcc

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y cmake libboost-all-dev
    
CMD ["cmake"]

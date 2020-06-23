# This is a tmcombi build env Dockerfile

FROM ubuntu

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y g++ cmake libboost-all-dev
    
CMD ["cmake"]

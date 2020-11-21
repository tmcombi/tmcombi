# This is a tmcombi build env Dockerfile

FROM ubuntu

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
    && apt-get install -y g++ ccache cmake libboost-all-dev git

RUN git clone https://github.com/nushoin/RTree.git
RUN git clone https://github.com/Sergey-Grosman/DynDimRTree.git

# glpk
#RUN apt-get update \
#    && apt-get install -y wget
#RUN wget http://ftp.gnu.org/gnu/glpk/glpk-4.65.tar.gz && tar -zxvf glpk-4.65.tar.gz && rm -fr glpk-4.65.tar.gz
#RUN cd glpk-4.65 && ./configure && make && make check && make install && make clean


# RUN useradd -m jenkins

CMD ["cmake"]

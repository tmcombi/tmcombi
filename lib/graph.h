#ifndef LIB_GRAPH_H_
#define LIB_GRAPH_H_


class Graph {
public:
    explicit Graph(const unsigned int);

private:
    const unsigned int dim_;
};

Graph::Graph(const unsigned int dim) : dim_(dim) {}


#endif

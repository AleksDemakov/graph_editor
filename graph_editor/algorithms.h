#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QMap>
#include <QObject>

class Node;
class GraphWidget;

class Algorithms : public QObject
{
    Q_OBJECT

public:
    Algorithms(GraphWidget * graph);
    ~Algorithms();

    void setup_dfs(Node * start);
    void setup_bfs(Node * start);
    void setup_dijkstra(Node * node);

public slots:
    void start_dfs_alg();
    void start_bfs_alg();
    void start_dijkstra_alg();

signals:
    void finished();


private:
    QMap< Node *, bool > used;
    QList< Node * > bfs_queue;
    QMap< Node *, int > dijkstra_distance;
    void dfs(Node * v);
    void bfs();
    void dijkstra();

    GraphWidget * graph;
    Node * start_vertex;

    int algos_time_ms = 1000;

};

#endif // ALGORITHMS_H

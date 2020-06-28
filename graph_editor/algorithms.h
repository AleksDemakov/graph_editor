#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QMap>
#include <QSet>
#include <QObject>

class Node;
class Edge;
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
    void setup_kruskal();
    void setup_eulerian(Node * node);

    QMap< Node *, int > getDijkstraDistance();

    void setAlgosTime(int time);

public slots:
    void start_dfs_alg();
    void start_bfs_alg();
    void start_dijkstra_alg();
    void start_kruskal_alg();
    void start_eulerian_alg();

signals:
    void finished();
    void finishedDijkstra( QMap< Node *, int > dijkstraDistance );
    void finishedEulerian( QVector< Node * > eulerian_path );


private:
    QMap< Node *, bool > used;
    QMap< Edge *, bool > used_edges;
    QList< Node * > bfs_queue;
    QMap< Node *, int > dijkstra_distance;
    QMap< Node *, Node *> parents;
    QVector< std::pair< int, Edge * > > graph_edges_kruskal;
    QVector< Node * > eulerian_path;
    void dfs(Node * v);
    void bfs();
    void dijkstra();
    Node * dsu_get(Node * node);
    void dsu_unite(Node * node1, Node * node2);
    void kruskal();
    void findEulerianPath(Node * node);

    GraphWidget * graph;
    Node * start_vertex;

    void delay();



    int algos_time_ms = 1000;

};

#endif // ALGORITHMS_H

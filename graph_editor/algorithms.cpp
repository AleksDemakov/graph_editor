#include "algorithms.h"
#include "graphwidget.h"
#include "node.h"
#include "edge.h"
#include "mainwindow.h"

#include <QThread>
#include <QDebug>
#include <QWindow>
#include <QTableWidget>

Algorithms::Algorithms(GraphWidget * graph)
{
    this->graph = graph;
}

Algorithms::~Algorithms()
{
    used.clear();
    bfs_queue.clear();
}

void Algorithms::setup_dfs(Node *start) {
    this->start_vertex = start;

    for (Node * node : graph->get_graph()) {
        used[ node ] = 0;
    }
}

void Algorithms::start_dfs_alg() {
    dfs(start_vertex);

    emit finished();
}

void Algorithms::dfs(Node * v) {
    used[ v ] = 1;
    bool was_operation = 0;

    v->set_color( QColor("green") );

    delay();

    for (Edge * edge : v->get_edges()) {
        was_operation = 0;

        if (!used[ edge->get_destination_node() ]) {

            v->set_color( QColor("yellow") );
            was_operation = 1;
            dfs( edge->get_destination_node() );

        }

        if (!graph->isDirected && !used[ edge->get_source_node() ]) {

            v->set_color( QColor("yellow") );
            was_operation = 1;
            dfs( edge->get_source_node() );

        }

        v->set_color( QColor("green") );
        if (was_operation) delay();

    }

    v->set_color( QColor("red") );

}




void Algorithms::setup_bfs(Node *start)
{
    for (Node * node : graph->get_graph()) {
        used[ node ] = 0;
    }


    bfs_queue.push_back( start );
    used[ start ] = 1;

}

void Algorithms::start_bfs_alg()
{
    bfs();

    emit finished();
}

void Algorithms::bfs()
{
    Node * v;
    bool was_operation;

    while (!bfs_queue.isEmpty()) {

        v = bfs_queue.front();
        bfs_queue.pop_front();

        v->set_color( QColor("green") );
        delay();



        for (Edge * edge : v->get_edges()) {
            was_operation = 0;

            if (!used[ edge->get_destination_node() ]) {

                was_operation = 1;
                edge->get_destination_node()->set_color( QColor("yellow") );
                bfs_queue.push_back( edge->get_destination_node() );
                used[ edge->get_destination_node() ] = 1;

            }

            if (!graph->isDirected && !used[ edge->get_source_node() ]) {

                was_operation = 1;
                edge->get_source_node()->set_color( QColor("yellow") );
                bfs_queue.push_back( edge->get_source_node() );
                used[ edge->get_source_node() ] = 1;

            }

            if (was_operation) delay();

        }

        v->set_color( QColor("red") );

    }

}



void Algorithms::setup_dijkstra(Node * node) {

    used.clear();
    dijkstra_distance.clear();

    for (Node * node : graph->get_graph()) {
        used[ node ] = 0;
        dijkstra_distance[ node ] = INT_MAX;
    }

    dijkstra_distance[ node ] = 0;

}

void Algorithms::start_dijkstra_alg()
{
    dijkstra();

    emit finishedDijkstra( getDijkstraDistance() );
}

void Algorithms::dijkstra()
{

    Node * v = nullptr;

    for (int i = 0; i < graph->cnt_of_nodes; i++) {

        v = nullptr;

        for (Node * node : graph->get_graph()) {
            if (!used[ node ] && ( v == nullptr || dijkstra_distance[ node ] < dijkstra_distance[ v ] ) ) {
                v = node;
            }
        }

        if ( v == nullptr || dijkstra_distance[ v ] == INT_MAX ) break;

        used[ v ] = true;

        for (Edge * edge : v->get_edges()) {

            if ( edge->get_destination_node() != v ) {
                dijkstra_distance[ edge->get_destination_node() ] = qMin( dijkstra_distance[ edge->get_destination_node() ],
                                                                          dijkstra_distance[ v ] + edge->get_weight() );
            }

            if (graph->isDirected) continue;

            if ( edge->get_source_node() != v ) {
                dijkstra_distance[ edge->get_source_node() ] = qMin( dijkstra_distance[ edge->get_source_node() ],
                                                                     dijkstra_distance[ v ] + edge->get_weight() );
            }

        }

    }

    QMap< Node * , int>::iterator i;
    for (i = dijkstra_distance.begin(); i != dijkstra_distance.end(); ++i)
         qDebug() << i.key()->get_name() << ": " << i.value() << endl;
}

QMap< Node *, int > Algorithms::getDijkstraDistance()
{
    return dijkstra_distance;
}




Node * Algorithms::dsu_get(Node *node)
{
    if (parents[node] == node) return node;
    return parents[node] = dsu_get( parents[node] );
}

void Algorithms::dsu_unite(Node *node1, Node *node2)
{
    node1 = dsu_get(node1);
    node2 = dsu_get(node2);

    if (node1 != node2) {
        parents[node1] = node2;
    }
}

void Algorithms::kruskal()
{
    QVector< std::pair<int, Edge *> >::iterator it;
    Node * a;
    Node * b;

    for (it = graph_edges_kruskal.begin(); it != graph_edges_kruskal.end(); it++) {

        a = it->second->get_source_node();
        b = it->second->get_destination_node();

        if (dsu_get(a) != dsu_get(b)) {
            dsu_unite(a, b);

            it->second->set_color( QColor("red") );
        }

    }
}

void Algorithms::setup_kruskal()
{
    parents.clear();
    graph_edges_kruskal.clear();

    for (Node * node : graph->get_graph()) {
        parents[ node ] = node;
    }

    for (Edge * edge : graph->getAllEdges()) {
        graph_edges_kruskal.push_back({ edge->get_weight(), edge });
    }
    std::sort( graph_edges_kruskal.begin(), graph_edges_kruskal.end() );

    //qDebug() << graph_edges_kruskal;

}

void Algorithms::start_kruskal_alg()
{
    kruskal();
    emit finished();
}




void Algorithms::setup_eulerian(Node *node)
{
    used_edges.clear();
    eulerian_path.clear();

    for (Edge * edge : graph->getAllEdges()) {
        used_edges[ edge ] = false;
    }

    start_vertex = node;
}

void Algorithms::start_eulerian_alg()
{

    findEulerianPath(start_vertex);
    std::reverse( eulerian_path.begin(), eulerian_path.end() );
    emit finishedEulerian( eulerian_path );

}

void Algorithms::findEulerianPath(Node *node)
{

    for (Edge * edge : node->get_edges()) {
        if (used_edges[ edge ] == true) continue;

        if ( edge->get_destination_node() != node ) {

            used_edges[ edge ] = true;

            findEulerianPath( edge->get_destination_node() );

        }

        if (graph->isDirected) continue;

        if ( edge->get_source_node() != node ) {

            used_edges[ edge ] = true;

            findEulerianPath( edge->get_source_node() );

        }
    }

    eulerian_path.push_back(node);

}

void Algorithms::setAlgosTime(int time)
{
    algos_time_ms = time;
}

void Algorithms::delay()
{
    for (int i = 0; i < algos_time_ms; i++) {
        QThread::msleep( 1 );
    }
}

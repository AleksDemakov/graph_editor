#include "algorithms.h"
#include "graphwidget.h"
#include "node.h"
#include "edge.h"

#include <QThread>
#include <QDebug>

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
    QThread::msleep( algos_time_ms );

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
        if (was_operation) QThread::msleep( algos_time_ms );

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
        QThread::msleep( algos_time_ms );



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

            if (was_operation) QThread::msleep( algos_time_ms );

        }

        v->set_color( QColor("red") );

    }

}



void Algorithms::setup_dijkstra(Node * node) {

    for (Node * node : graph->get_graph()) {
        used[ node ] = 0;
        dijkstra_distance[ node ] = INT_MAX;
    }

    dijkstra_distance[ node ] = 0;

}

void Algorithms::start_dijkstra_alg()
{
    dijkstra();

    emit finished();
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

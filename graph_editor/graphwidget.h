#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include <QGraphicsView>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QQueue>
#include <qthread.h>

class Edge;
class Node;
class Algorithms;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:

    enum {
        DFS,
        BFS
    };

    GraphWidget(QWidget *parent = nullptr);
    //void nodesColorChange(QString text);
    void item_is_changed();
    Edge * added_edge_to_make_Eulerian_cycle;
    QVector<Node *> &get_graph();
    QGraphicsScene *sc;
    bool isDirected = false;
    bool isWeighted = false;
    QColor edgeColor;
    QColor nodeColor;
    double edge_length = 110;
    int cnt_of_nodes = 0;
    QFont font;
    int mex();
    QSet< Edge * > getAllEdges();

    void setAlgosTime(int time);

    void start_dfs(QString name);
    void start_bfs(QString name);
    void start_dijkstra(QString name);
    void start_kruskal();
    void start_eulerian();

    void adjust_cnt_of_nodes();



signals:
    void graphChanged();
    void edgedAdded(Edge * edge);
    void dirChanged(bool state);
    void dirChangedReverse(bool state);
public slots:
    void graphDraw();
    void nodesColorChange(QString);
    void edgesColorChange(QString);
    void setDirected();
    void setDirected(bool isdir);
    void setWeighted();
    void setWeighted(bool isdir);




protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void nodesColorChange();
    void edgesColorChange();
    bool isEulerian();
    bool isSemiEulerian(Node * &start_node, Node * &finish_node);

private slots:
    void disconnect_thread();
    void processDijkstraResults( QMap< Node *, int > dijkstraDistance );
    void processEulerianResults( QVector< Node * > eulerain_path );

private:

    //bool drawing_an_edge;
    Edge * drawing_edge;

    QVector<Node *> graph;

    int timerId = 0;

    void paint_nodes_in_algorithm( Node * cur_node );
    QThread algos_thread;
    Algorithms * alg;



};
#endif // MAINWINDOW_H

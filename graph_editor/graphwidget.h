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

    void start_dfs(QString name);
    void start_bfs(QString name);
    void start_dijkstra(QString name);

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

    void disconnect_thread();



protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void nodesColorChange();


private:

    bool drawing_an_edge;
    Edge * drawing_edge;
    QVector<Node *> graph;

    int timerId = 0;

    void paint_nodes_in_algorithm( Node * cur_node );
    QThread algos_thread;
    Algorithms * alg;


};
#endif // MAINWINDOW_H

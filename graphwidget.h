#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include <QGraphicsView>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QQueue>

class Edge;
class Node;

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
    bool isDirected;
    QColor edgeColor;
    QColor nodeColor;
    double edge_length = 110;
    int cnt_of_nodes = 0;
    QFont font;
    int mex();

    void start_dfs(QString name);
    void start_bfs(QString name);

    void adjust_cnt_of_nodes();

signals:
    void graphChanged();
    void edgedAdded(Edge * edge);
public slots:
    void graphDraw();
    void nodesColorChange(QString);
    void edgesColorChange(QString);
    void setDirected();


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void nodesColorChange();


private:

    bool drawing_an_edge;
    Edge * drawing_edge;
    QVector<Node *> graph;
    std::map<QString, int> used;

    QList< QPair<Node *, QSet< Edge * >::iterator > > dfs_stack;
    QList< QPair<Node *, QSet< Edge * >::iterator > > bfs_deque;

    QVector< QTimer * > timers;
    QTimer * timer;
    int algos_time_ms = 2000;

    int timerId = 0;

    void paint_nodes_in_algorithm( Node * cur_node );


private slots:
    void dfs_iteration();
    void bfs_iteration();

};
#endif // MAINWINDOW_H

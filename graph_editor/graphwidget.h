#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include <QGraphicsView>
#include <QMap>

class Edge;
class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
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

    void start_dfs(QString name);


private:

    bool drawing_an_edge;
    Edge * drawing_edge;
    QVector<Node *> graph;
    QMap<QString, bool> used;
    void dfs(Node * node);

    int timerId = 0;
};
#endif // MAINWINDOW_H

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include <QGraphicsView>

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

signals:
    void graphChanged();
public slots:
    void graphDraw();
    void nodesColorChange(QString);
    void edgesColorChange(QString);
    void setDirected();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    QString mex();
private:

    bool drawing_an_edge;
    Edge * drawing_edge;
    QVector<Node *> graph;

    int timerId = 0;
};
#endif // MAINWINDOW_H

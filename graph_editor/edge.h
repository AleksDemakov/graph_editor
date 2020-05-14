#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include <QPoint>

class Node;
class GraphWidget;

class Edge: public QGraphicsItem {
public:
    Edge(GraphWidget *graphWidget,Node * source, Node * destination, bool isDir);
    Edge(GraphWidget *graphWidget, Node * source, QPointF destPoint, bool isDir);
    ~Edge();

    Node * get_source_node() const;
    Node * get_destination_node() const;
    void set_destination_node(Node * destination);

    void adjust();
    void adjust(QPointF pos);

    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    bool getIsDirected();
    void setIsDirected(bool dir);
    void disable_following_the_cursor();
public slots:
    void set_color(QColor new_color);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
    GraphWidget *graph;
    QPainterPath shape() const override;

    Node *source, *destination;
    QPointF sourcePoint;
    QPointF destPoint;
    QColor color;
    const qreal penWidth = 1.9;
    const qreal extra = (penWidth) / 2.0;
    bool isDirected;
    bool following_the_cursor;
};

#endif // EDGE_H

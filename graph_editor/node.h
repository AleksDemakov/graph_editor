#ifndef NODE_H
#define NODE_H
#include <QGraphicsItem>
#include <QSet>

class Edge;
class GraphWidget;
class NodeLabel;


class Node : public QGraphicsItem
{
public:
    Node(GraphWidget *graphWidget);

    Node(GraphWidget *graphWidget, QPointF pos, QString name);
    Node(GraphWidget *graphWidget, QPointF pos);
    Node(GraphWidget *graphWidget, QString name);
    
    ~Node();

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget ) override;
    void add_edge(Edge * edge);
    bool is_adjacent_with(Node * node);

    void calculateForces();
    bool advancePosition();

    QSet<Edge *> & get_edges();
    QString get_name();
    void set_name(QString new_name);

public slots:
    void set_color(QColor new_color);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QPainterPath shape() const override;
    GraphWidget *graph;
    QSet<Edge *> edges;

    QPointF new_calculated_pos;
    QColor color;
    QString name;
};

#endif // NODE_H

#include "edge.h"
#include "node.h"

#include <QPainter>
#include <QDebug>
#include <QPoint>

Edge::Edge(Node * source, Node * destination) {
    if (source == NULL || destination == NULL) return;

    this->source = source;
    this->destination = destination;

    //Makes edges no-clickable
    setAcceptedMouseButtons(Qt::NoButton);

    //addes this edge to nodes' edge lists

    source->add_edge(this);
    destination->add_edge(this);

    adjust();
}

Edge::Edge(Node * source, QPointF destPoint) {
    if (source == NULL) return;

    this->source = source;
    this->destination = NULL;


    //Makes edges no-clickable
    //setAcceptedMouseButtons(Qt::NoButton);

    //addes this edge to source's edge list
    source->add_edge(this);

    adjust( destPoint );
}

Edge::~Edge() {

    if (source != NULL) {
        source->edges.remove( this );
    }
    if (destination != NULL) {
        destination->edges.remove( this );
    }

}


Node * Edge::get_source_node() const
{
    return this->source;
}

Node *Edge::get_destination_node() const
{
    return this->destination;
}

void Edge::adjust() {

    if (source == NULL || destination == NULL) return;

    QGraphicsItem::prepareGeometryChange();

    sourcePoint = source->pos();
    destPoint = destination->pos();

}

void Edge::adjust(QPointF pos) {
    if (source == NULL) return;

    QGraphicsItem::prepareGeometryChange();

    sourcePoint = source->pos();
    destPoint = pos;

}

QRectF Edge::boundingRect() const
{
    //if ( !following_the_cursor && (!source || !destination) )
        //return QRectF();

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    painter->setPen(QPen(color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
}

void Edge::disable_following_the_cursor() {
    this->following_the_cursor = false;
}

void Edge::set_destination_node(Node * destination) {
    this->destination = destination;
    this->destination->add_edge(this);
    adjust();
}
void Edge::set_color(QColor new_color)
{
    color = new_color;
    update();
}



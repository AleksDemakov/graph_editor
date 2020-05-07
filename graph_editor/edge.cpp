#include "edge.h"
#include "node.h"
#include "graphwidget.h"
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include <QtMath>



Edge::Edge(Node * source, Node * destination, bool isDir) {
    if (source == NULL || destination == NULL) return;

    this->source = source;
    this->destination = destination;
    this->setIsDirected(isDir);
    //Makes edges no-clickable
    setAcceptedMouseButtons(Qt::NoButton);

    //addes this edge to nodes' edge lists
    this->setIsDirected(isDir);
    source->add_edge(this);
    destination->add_edge(this);
    adjust();
}



Edge::Edge(Node * source, QPointF destPoint, bool isDir){
    if (source == NULL) return;

    this->source = source;
    this->destination = NULL;
    this->setIsDirected(isDir);
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
    QLineF line(sourcePoint, destPoint);
    qreal length = line.length();
    //qDebug()<<length;
    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 20) / length, (line.dy() * 20) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
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


    if(!isDirected)return;

    qreal arrowSize = 15;
    // Draw the arrows
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);
    //to draw arrow in invert direction swap plus on minus(see Elastic node)
    painter->setBrush(color);

    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);



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
bool Edge::getIsDirected(){
    return isDirected;
}
void Edge::setIsDirected(bool dir){
    isDirected = dir;
    update();
}


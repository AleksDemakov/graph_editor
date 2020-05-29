#include "edge.h"
#include "node.h"
#include "graphwidget.h"
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include <QVector2D>
#include <QtMath>


Edge::Edge(GraphWidget *graphWidget, Node * source, Node * destination, bool isDir)
    :Edge(graphWidget, source, destination, isDir, 1)
{
}

Edge::Edge(GraphWidget *graphWidget, Node * source, Node * destination, bool isDir, int weight) {
    if (source == NULL || destination == NULL) return;
    this->graph = graphWidget;
    this->source = source;
    this->destination = destination;
    setCacheMode(DeviceCoordinateCache);

    this->setIsDirected(isDir);

    //Makes edges no-clickable
    setAcceptedMouseButtons(Qt::NoButton);

    //addes this edge to nodes' edge lists
    source->add_edge(this);
    destination->add_edge(this);

    weight_label = new QGraphicsSimpleTextItem( QString::number( weight ) );
    weight_label->setFont( graph->font );
    graph->scene()->addItem( weight_label );
    if (!graph->isWeighted) hide_weight_label();

    this->set_weight(weight);
    this->setIsDirected(isDir);
    this->set_color(this->graph->edgeColor);

    adjust();
}

Edge::Edge(GraphWidget *graphWidget,Node * source, QPointF destPoint, bool isDir) {
    if (source == NULL) return;
    this->graph = graphWidget;
    this->source = source;
    this->set_color(this->graph->edgeColor);
    this->destination = NULL;
    this->setIsDirected(isDir);

    //Makes edges no-clickable
    setAcceptedMouseButtons(Qt::NoButton);

    //addes this edge to source's edge list
    source->add_edge(this);

    weight_label = new QGraphicsSimpleTextItem(QString::number( weight ) );
    weight_label->setFont( graph->font );
    graph->scene()->addItem( weight_label );

    this->set_weight(1);

    hide_weight_label();

    adjust( destPoint );


}
Edge::~Edge() {
    if (source != nullptr) {
        //if (source->get_edges().find( this ) != source->get_edges().end())
        source->get_edges().remove( this );
    }
    if (destination != nullptr) {
        //if (destination->get_edges().find( this ) != destination->get_edges().end())
        destination->get_edges().remove( this );
    }
    if(scene() != nullptr){
        scene()->removeItem(this);
    }

    if (weight_label->scene() != nullptr) graph->scene()->removeItem( weight_label );
   
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

    QPointF pos = destPoint + sourcePoint;
    pos /= 2;
    QVector2D vec( destPoint - sourcePoint );

    vec = { -vec.y(), vec.x() };
    if (vec.y() > 0) vec.setY( -vec.y() );
    vec.normalize();
    vec *= 20;

    if (vec.x() > 0) vec.setX( vec.x() - weight_label->boundingRect().width() / 2.0 );

    pos += vec.toPointF();
    weight_label->setPos( pos );

    if ( graph->isWeighted && !weight_label->isVisible() ) this->show_weight_label();

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

    QRectF rect(sourcePoint, QSizeF( destPoint.x() - sourcePoint.x() ,
                                     destPoint.y() - sourcePoint.y() ));
    rect = rect.normalized();
    rect = rect.adjusted(-extra, -extra, extra, extra);

    //rect.setWidth( qMax( rect.width(), 50.0 ) );
    //rect.setHeight( qMax( rect.height(), 50.0 ) );

    return rect;
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPointF adjust_source_and_destination = destPoint - sourcePoint;
    qreal d = sqrt( adjust_source_and_destination.x() * adjust_source_and_destination.x() + adjust_source_and_destination.y() * adjust_source_and_destination.y() );
    adjust_source_and_destination.rx() /= d;
    adjust_source_and_destination.ry() /= d;

    adjust_source_and_destination.rx() *= 6;
    adjust_source_and_destination.ry() *= 6;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    painter->setPen(QPen(color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    if (destination == nullptr) return;


    if(isDirected) {

        qreal arrowSize = 15;
        // Draw the arrows
        double angle = std::atan2(-line.dy(), line.dx());

        QPointF destArrowP1 = destPoint - adjust_source_and_destination + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                                  cos(angle - M_PI / 3) * arrowSize);
        QPointF destArrowP2 = destPoint - adjust_source_and_destination + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                                  cos(angle - M_PI + M_PI / 3) * arrowSize);
        //to draw arrow in invert direction swap plus on minus(see Elastic node)
        painter->setBrush(color);

        painter->drawPolygon(QPolygonF() << line.p2() - adjust_source_and_destination << destArrowP1 << destArrowP2);

    }


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
void Edge::set_weight(int weight){
    this->weight = weight;
    weight_label->setText( QString::number( weight ) );

}

QPainterPath Edge::shape() const
{
    QPainterPath path;
    QPolygonF polygon;

    QVector2D vec(destPoint - sourcePoint);
    vec = { -vec.y(), vec.x() };
    vec.normalize();
    vec *= 2 * penWidth;

    polygon << sourcePoint + vec.toPointF() << sourcePoint - vec.toPointF();
    polygon << destPoint - vec.toPointF() << destPoint + vec.toPointF();

    path.addPolygon( polygon );
    path.closeSubpath();

    return  path;

}

int Edge::get_weight(){
    return weight;
}
QString Edge::get_weight_str(){
    return QString::number(weight);
}
bool Edge::getIsDirected(){
    return isDirected;
}

void Edge::setIsDirected(bool dir)
{
    isDirected = dir;
    extra = ( penWidth + (dir ? 15 : 0) ) / 2.0;
    update();
}

void Edge::show_weight_label()
{
    weight_label->show();
}

void Edge::hide_weight_label()
{
    weight_label->hide();
}

bool Edge::weight_label_is_on_scene()
{
    return  (weight_label->scene() != nullptr);
}

#include "node.h"
#include "graphwidget.h"
#include "edge.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QtMath>
#include <QRandomGenerator>

Node::Node(GraphWidget *graphWidget, QString name)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);

    color = graphWidget->nodeColor;



    int from, to;
    double x, y;

    from = graph->sceneRect().left();
    to = graph->sceneRect().right();
    x = QRandomGenerator::global()->bounded(from, to);

    to = graph->sceneRect().bottom();
    from = graph->sceneRect().top();
    y = QRandomGenerator::global()->bounded(from, to);

    this->setPos(x, y);

    if (name == "") this->set_name( QString::number( graph->mex() ) );//
    else this->set_name( name );

    graph->get_graph().push_back( this );
    graph->adjust_cnt_of_nodes();

}

Node::Node(GraphWidget *graphWidget, QPointF pos, QString name)
    : Node(graphWidget, name)
{
    this->setPos( pos );
}

Node::Node(GraphWidget *graphWidget, QPointF pos)
    : Node(graphWidget, QString::number(graphWidget->mex()))//
{
    //qDebug()<<pos;
    this->setPos( pos );

}

Node::Node(GraphWidget *graphWidget)
    : Node(graphWidget, QString::number(graphWidget->mex())) {}//


Node::~Node() {

    graph->get_graph().removeOne(this);
    graph->adjust_cnt_of_nodes();

    QSet<Edge *> copy_edges = get_edges();

    for (Edge * edge : copy_edges) {
        delete edge;
    }

    get_edges().clear();
    copy_edges.clear();

    if(scene() != NULL)
        scene()->removeItem(this);
}


QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    //return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
    //17 39.1
    return QRectF( -24 - adjust, -24 - adjust, 48 + 2 * adjust, 48 + 2 * adjust);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    //painter->setBrush(Qt::darkGray);
    //painter->drawEllipse(-7, -7, 20, 20);
    //painter->drawEllipse(-15, -15, 30, 30);

    QColor node_background;

    if (option->state & QStyle::State_Sunken) {
        node_background = color;
    } else {
        /*gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);*/
        node_background = QColor( Qt::white );
    }


    //painter->setPen(QPen(Qt::black, 0));
    painter->setBrush( node_background );
    painter->setPen(QPen(color, 1));
    //painter->drawEllipse(-10, -10, 20, 20);
    //painter->drawEllipse(-17, -17, 34, 34);

    painter->drawEllipse(-24, -24, 48, 48);

    painter->setFont(graph->font);

    painter->setPen( QColor(255 - node_background.red(), 255 - node_background.green(), 255 - node_background.blue()) );

    painter->drawText( this->boundingRect(), Qt::AlignCenter, this->get_name() );


}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::update();
    QGraphicsItem::mousePressEvent(event);
}
void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::update();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {

    if (change == QGraphicsItem::ItemPositionHasChanged) {

        for (Edge * edge : edges) {
            edge->adjust();
            //qDebug()<<"!";
        }

        graph->item_is_changed();
    }

    return QGraphicsItem::itemChange(change, value);

}

void Node::add_edge(Edge * edge) {
    Node::edges.insert(edge);
}

bool Node::is_adjacent_with(Node * node) {
    for (Edge * edge : edges) {
        if ( (!graph->isDirected && edge->get_source_node() == node) || edge->get_destination_node() == node) return true;
    }

    return  false;
}

bool Node::advancePosition() {
    if (new_calculated_pos == this->pos()) {
        return false;
    }

    this->setPos(new_calculated_pos);

    return true;
}

void Node::calculateForces() {

    if (scene() == NULL || scene()->mouseGrabberItem() == this) {
        new_calculated_pos = this->pos();
        return;
    }


    qreal xvel = 0;
    qreal yvel = 0;

    const QVector<Node *> all_nodes = graph->get_graph();


    for (Node * node : all_nodes) {

        if (node == this) continue;

        QPointF vec = node->pos() - this->pos();
        qreal dx = vec.x();
        qreal dy = vec.y();
        qreal d = qSqrt(dx * dx + dy * dy);

        //qDebug() << pos() << dx << " " << dy << node->pos();

        if (d < 1) dx = dy = 3;


        if (d < graph->edge_length) {
            xvel -= 6 / dx;
            yvel -= 6 / dy;
        }
//        else if (d > 2.0 * edge_length){
//            xvel += 0.01;
//            yvel -= 0.01;
//        }


    }

   // qDebug() << xvel << " " << yvel;

    xvel = xvel > 0 ? qMin(xvel, 1.0) : qMax(xvel, -1.0) ;
    yvel = yvel > 0 ? qMin(yvel, 1.0) : qMax(yvel, -1.0) ;




    //QRectF sceneRect = scene()->sceneRect();

    //qDebug() << sceneRect.left() << sceneRect.right() << this->pos();

    new_calculated_pos = pos() + QPointF(xvel, yvel);
    new_calculated_pos.setX( qMax( new_calculated_pos.x(), -300.0 ) );
    new_calculated_pos.setX( qMin( new_calculated_pos.x(), 300.0 ) );
    new_calculated_pos.setY( qMax( new_calculated_pos.y(), -300.0 ) );
    new_calculated_pos.setY( qMin( new_calculated_pos.y(), 300.0 ) );
}
QSet<Edge *> & Node::get_edges()
{
    return edges;
}
QString Node::get_name()
{
    return name;
}
void Node::set_name(QString new_name)
{
    name = new_name;
}
void Node::set_color(QColor new_color)
{
    color = new_color;
    update();
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

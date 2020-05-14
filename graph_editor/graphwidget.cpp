#include "graphwidget.h"
#include "node.h"
#include "edge.h"
#include "settings.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QRandomGenerator>
#include <QTextEdit>
#include <QApplication>
#include <QRadioButton>
#include <qalgorithms.h>


GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{

    sc = new QGraphicsScene(this);
    sc->setItemIndexMethod(QGraphicsScene::NoIndex);
    sc->setSceneRect(-200, -200, 400, 400);
    setScene(sc);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(500, 400);

    drawing_an_edge = false;
    drawing_edge = NULL;



    cnt_of_nodes = QRandomGenerator::global()->bounded(2, 10);

    //quint32 v = QRandomGenerator::bounded();

//    Node * node_test_edge1 = new Node(this);
//    node_test_edge1->setPos(-50, -50);
//    Node * node_test_edge2 = new Node(this);
//    node_test_edge2->setPos(50, 50);
//    Node * node_test_edge3 = new Node(this);
//    node_test_edge2->setPos(0, 100);

//    sc->addItem(node_test_edge1);
//    sc->addItem(node_test_edge2);
//    sc->addItem(node_test_edge3);

//    Edge * edge1 = new Edge(node_test_edge1, node_test_edge2);
//    Edge * edge2 = new Edge(node_test_edge1, node_test_edge3);
//    sc->addItem(edge1);
//    sc->addItem(edge2);


    for (int i = 0; i < cnt_of_nodes; i++) {
        new Node(this);
        sc->addItem(graph[i]);
    }

    bool make_edge;
    Edge * new_edge;

    for (int i = 0; i < cnt_of_nodes; i++) {
        for (int j = i + 1; j < cnt_of_nodes; j++) {

            make_edge = QRandomGenerator::global()->bounded(6);

            if (!make_edge) {
                new_edge = new Edge( graph[i], graph[j], isDirected );
                sc->addItem(new_edge);
            }

        }
    }

}

void GraphWidget::setDirected(){

    QRadioButton *but =  qobject_cast<QRadioButton *>(sender());
    bool buttonDirValue = false;
    if(but->objectName() == "buttonDirected")
        buttonDirValue = true;
    isDirected = buttonDirValue;
    for(Node *node:get_graph())
        for(Edge *edge:node->get_edges())
            edge->setIsDirected(buttonDirValue);
}

void GraphWidget::nodesColorChange(QString text)
{
    for(Node* i:get_graph()){
        i->set_color(QColor(text));
    }
}

void GraphWidget::edgesColorChange(QString text)
{
    for(Node* i:get_graph()){
        for(Edge* j:i->get_edges())
            j->set_color(QColor(text));
    }
}

void GraphWidget::graphDraw()
{
    QTextEdit *gtext = qobject_cast<QTextEdit *>(sender());
    QList<QString> edges = gtext->toPlainText().split('\n');
    //qDebug()<<edges;

    QVector<Node*> odd;

    for(Node *i:graph){
        for(Edge *j:i->get_edges()){
            if(!edges.contains(j->get_source_node()->get_name()+" "+j->get_destination_node()->get_name())){
                delete j;
            }
        }
    }





    Node *uNode, *vNode;
    for(QString edge:edges){
        uNode=NULL;
        vNode=NULL;
        if(!edge.contains(' ') || *edge.end() == ' ')continue;

        QList<QString> u = edge.split(' ');
        if(u[1]=="")continue;
        //qDebug()<<u;


        for(Node *i:graph){

            if(i->get_name() == u[1]){
                vNode = i;
            }
            if(i->get_name() == u[0]){
                uNode = i;
            }
        }
        if(!uNode->is_adjacent_with(vNode)){

            sc->addItem(new Edge( uNode, vNode, isDirected));
        }
    }

}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    //update();

    //catch item clicked
    //qDebug()<<itemAt(event->pos())->type();
    Node * item_click = qgraphicsitem_cast<Node*>(itemAt( event->pos() ));
    Edge * item_click_edge = qgraphicsitem_cast<Edge*>(itemAt( event->pos() ));

    //if (item_click) qDebug() << item_click->pos() << mapToScene( event->pos() );
    //return;

    if(item_click_edge && QApplication::keyboardModifiers() == Qt::ShiftModifier && event->button() == Qt::LeftButton){

        delete item_click_edge;
        emit this->graphChanged();
    }


    //deleting node Shift+leftButton
    if(item_click && QApplication::keyboardModifiers() == Qt::ShiftModifier && event->button() == Qt::LeftButton)
    {
        for(Edge *edge:item_click->get_edges())
        {
            delete edge;
        }
       //sc->removeItem(item_click);
       graph.removeOne(item_click);
       delete item_click;
       emit this->graphChanged();
    }
    // !!-----------------
    // just mouse below
    if(QApplication::keyboardModifiers() != Qt::NoModifier)
        return;
    //if click on graph view, creates new node
    if(!item_click  && event->button() == Qt::LeftButton)
    {

        new Node(this, mapToScene( event->pos() ) );
        sc->addItem( graph.back() );
        //emit this->graphChanged();
    }
    //if click on graph view, creates new node

    if (event->button() == Qt::RightButton) {
        //create a pointer
        //if clicked item isn't a node then delete an edge

        if (!item_click) {
            if (drawing_edge != NULL) {
                delete drawing_edge;
                drawing_edge = NULL;
            }
        }
        else {

            if (drawing_edge != NULL) {

                if (!item_click->is_adjacent_with( drawing_edge->get_source_node() )) {
                    drawing_edge->disable_following_the_cursor();
                    drawing_edge->set_destination_node(item_click);
                    emit this->graphChanged();
                }
                else {
                    delete (drawing_edge);
                }

                drawing_edge = NULL;
            }
            else {

                drawing_edge = new Edge( item_click, mapToScene( event->pos() ), isDirected );
                sc->addItem(drawing_edge);
            }

        }


    }

    QGraphicsView::mousePressEvent(event);
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{

    if (drawing_edge) {
        drawing_edge->adjust( mapToScene( event->pos() ) );
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);


    for (Node *node : get_graph())
        node->calculateForces();

    bool itemsMoved = false;
    for (Node *node : get_graph()) {
        if (node->advancePosition())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

void GraphWidget::item_is_changed()
{

    if (timerId == 0) {
        timerId = startTimer(10);
    }
}


QVector<Node *> & GraphWidget::get_graph()
{
    return graph;
}
QString GraphWidget::mex(){
    int mex = 0;
    QVector<QString> t;
    for(Node *i:graph)
        t.push_back(i->get_name());
    std::sort(t.begin(), t.end());
    for(QString i:t){
        if(i != QString::number(mex)){
            return QString::number(mex);
        }else{
            mex++;}
    }
    return QString::number(mex);
}



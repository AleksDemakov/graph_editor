#include "graphwidget.h"
#include "node.h"
#include "edge.h"
#include "mainwindow.h"

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
    font = QFont("serif");

    timer = nullptr;


    int cnt = QRandomGenerator::global()->bounded(2, 10);

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


    for (int i = 0; i < cnt; i++) {
        new Node(this);
        sc->addItem(graph[i]);
    }

    bool make_edge;
    Edge * new_edge;

    for (int i = 0; i < cnt; i++) {
        for (int j = i + 1; j < cnt; j++) {

            make_edge = QRandomGenerator::global()->bounded(6);

            if (!make_edge) {
                new_edge = new Edge(this, graph[i], graph[j], isDirected );
                sc->addItem(new_edge);
            }

        }
    }

}
void GraphWidget::setDirected(bool isdir){
    isDirected = isdir;
    for(Node *node:get_graph())
        for(Edge *edge:node->get_edges())
            edge->setIsDirected(isDirected);

    if(isDirected == true){
        emit this->dirChanged(true);
    }else{
        emit this->dirChangedReverse(true);
    }
}
void GraphWidget::setDirected(){

    QRadioButton *but =  qobject_cast<QRadioButton *>(sender());
    bool buttonDirValue = false;
    if(but->objectName() == "buttonDirected")
        buttonDirValue = true;
    setDirected(buttonDirValue);
}
void GraphWidget::nodesColorChange(QString text)
{
    nodeColor = QColor(text);
    nodesColorChange();
}
void GraphWidget::nodesColorChange()
{
    for(Node* i:get_graph()){
        i->set_color(nodeColor);
    }
}

void GraphWidget::edgesColorChange(QString text)
{
    edgeColor = QColor(text);
    for(Node* i:get_graph()){
        for(Edge* j:i->get_edges())
            j->set_color(QColor(text));
    }
}

void GraphWidget::graphDraw()
{
    QTextEdit *gtext = this->parent()->findChild<QTextEdit *>("textEdit");

    QList<QString> edges = gtext->toPlainText().split('\n');




    QVector<Node*> odd;

    for(Node *i:graph){
        for(Edge *j:i->get_edges()){

            if (j->get_destination_node() == NULL || j->get_source_node() == NULL) continue;

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

        if (uNode == NULL) {
            uNode = new Node(this, u[0]);
            sc->addItem(uNode);
        }
        if (vNode == NULL) {
            vNode = new Node(this, u[1]);
            sc->addItem(vNode);
        }

        if(!uNode->is_adjacent_with(vNode)){
            qDebug()<<uNode->get_name()<<" "<<vNode->get_name();
            sc->addItem(new Edge(this, uNode, vNode, isDirected));
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
        emit this->graphChanged();
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

                if (!drawing_edge->get_source_node()->is_adjacent_with( item_click )) {
                    drawing_edge->disable_following_the_cursor();
                    drawing_edge->set_destination_node(item_click);
                    drawing_edge->setIsDirected(isDirected);
                    emit this->graphChanged();
                }
                else {
                    delete (drawing_edge);
                }

                drawing_edge = NULL;
            }
            else {

                drawing_edge = new Edge(this, item_click, mapToScene( event->pos() ), 0 );
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

int GraphWidget::mex(){

    QVector<bool> was( cnt_of_nodes );
    bool is_number = new bool(false);
    int name;

    for (Node * node : get_graph()) {
        name = node->get_name().toInt(&is_number, 10);

        if (is_number) {

            if (name >= was.size()) {
                was.resize(name + 1);
            }

            was[ name ] = true;
        }
    }

    for (int i = 0; i < cnt_of_nodes; i++) {
        if (!was[ i ]) return  i;
    }

    return  cnt_of_nodes;
}

void GraphWidget::adjust_cnt_of_nodes()
{
    cnt_of_nodes = get_graph().size();

    qDebug() << cnt_of_nodes;
}

void GraphWidget::paint_nodes_in_algorithm(Node *cur_node)
{
    //first paint nodes
    for (Node * node : graph) {

        if (node == cur_node) continue;
        if (used[ node->get_name() ] == 1) node->set_color( QColor("yellow") );
        if (used[ node->get_name() ] == 2) node->set_color( QColor("red") );
    }
}


void GraphWidget::start_dfs(QString name)
{
    if (timer != nullptr && timer->isActive()) return;

    if (timers.length() <= GraphWidget::DFS) {
        timers.resize( GraphWidget::DFS + 1 );
    }
    if (timers[ GraphWidget::DFS ] == NULL) {
        timers[ GraphWidget::DFS ] = new QTimer;
        connect(timers[ GraphWidget::DFS ], SIGNAL( timeout() ), this, SLOT( dfs_iteration() ));
    }

    timer = timers[ GraphWidget::DFS ];

    nodesColorChange();

    for (Node * node : graph) {
        used[ node->get_name() ] = 0;
    }

    Node * start;
    for (int i = 0; i < cnt_of_nodes; i++) {
        if (graph[i]->get_name() == name) {
            start = graph[i];
        }
    }

    if (start == NULL) return;

    nodesColorChange();

    timer->setInterval( algos_time_ms );

    dfs_stack.clear();

    dfs_stack.push_back({ start, start->get_edges().begin() });

    timer->start();

}

void GraphWidget::dfs_iteration()
{

    if (dfs_stack.isEmpty()) {
        timer->stop();
        return;
    }

    QPair< Node *, QSet< Edge * >::iterator > v = dfs_stack.back();
    dfs_stack.pop_back();

    v.first->set_color( QColor("green") );

    used[ v.first->get_name() ] = 1;

    Edge * cur_edge;

    QSet<Edge *>::iterator it;

    //first paint nodes
    paint_nodes_in_algorithm( v.first );

    //then go to other nodes

    for (it = v.second; it != v.first->get_edges().end(); it++) {

        cur_edge = *( it );



        if (used[ cur_edge->get_source_node()->get_name() ] == 0) {

            dfs_stack.push_back(v);
            dfs_stack.push_back({ cur_edge->get_source_node(), cur_edge->get_source_node()->get_edges().begin() });

            return;
        }
        else if (used[ cur_edge->get_destination_node()->get_name() ] == 0) {

            dfs_stack.push_back(v);
            dfs_stack.push_back({ cur_edge->get_destination_node(), cur_edge->get_destination_node()->get_edges().begin() });

            return;
        }

    }

    used[ v.first->get_name() ] = 2;


}


void GraphWidget::start_bfs(QString name)
{
    if (timer != NULL && timer->isActive()) return;

    if (timers.size() <= GraphWidget::BFS) {
        timers.resize( GraphWidget::BFS + 1 );
    }
    if (timers[ GraphWidget::BFS ] == NULL) {
        timers[ GraphWidget::BFS ] = new QTimer;
        connect(timers[ GraphWidget::BFS ], SIGNAL( timeout() ), this, SLOT( bfs_iteration() ));
    }

    timer = timers[ GraphWidget::BFS ];

    nodesColorChange();


    for (Node * node : graph) {
        used[ node->get_name() ] = 0;
    }

    Node * start;
    for (int i = 0; i < cnt_of_nodes; i++) {
        if (graph[i]->get_name() == name) {
            start = graph[i];
        }
    }

    if (start == NULL) return;



    bfs_deque.push_back({ start, start->get_edges().begin() });
    used[ start->get_name() ] = 1;

    timer->setInterval( algos_time_ms );
    timer->start();
}

void GraphWidget::bfs_iteration() {

    if (bfs_deque.isEmpty()) {
        timer->stop();
        return;
    }

    QPair< Node *, QSet< Edge * >::iterator > &v = bfs_deque.front();

    v.first->set_color( QColor("green") );

    Edge * cur_edge;

    QSet<Edge *>::iterator it;

    //first paint nodes
    paint_nodes_in_algorithm(v.first);

    //then go on

    for (it = v.second; it != v.first->get_edges().end(); it++) {
        cur_edge = *( it );

        if ( used[ cur_edge->get_source_node()->get_name() ] == 0 ) {

            v.second++;
            bfs_deque.push_back({ cur_edge->get_source_node(), cur_edge->get_source_node()->get_edges().begin() });
            used[ cur_edge->get_source_node()->get_name() ] = 1;

            return;
        }
        if ( used[ cur_edge->get_destination_node()->get_name() ] == 0 ) {

            v.second++;
            bfs_deque.push_back({ cur_edge->get_destination_node(), cur_edge->get_destination_node()->get_edges().begin() });
            used[ cur_edge->get_destination_node()->get_name() ] = 1;

            return;
        }

    }

    bfs_deque.pop_front();
    used[ v.first->get_name() ] = 2;

}

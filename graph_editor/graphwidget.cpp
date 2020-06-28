#include "graphwidget.h"
#include "node.h"
#include "edge.h"
#include "mainwindow.h"
#include "algorithms.h"

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
    setMinimumSize(500, 400);
    sc = new QGraphicsScene(this);
    sc->setItemIndexMethod(QGraphicsScene::NoIndex);
    sc->setSceneRect(-200, -200, 400, 400);
    //sc->setSceneRect(this->rect());
    setScene(sc);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));


    drawing_edge = NULL;
    radius = 25;
    font = QFont("serif");
    this->setFontSize(9);



    //create thread and object for algos
    alg = new Algorithms(this);
    alg->moveToThread( &algos_thread );
    connect( alg, SIGNAL( finished() ), this, SLOT( disconnect_thread() ) );

    qRegisterMetaType< QMap<Node *, int> >("QMap<Node *, int>");
    connect( alg, SIGNAL( finishedDijkstra( QMap< Node *, int >  ) ), this, SLOT( processDijkstraResults( QMap< Node *, int >  ) ) );

    qRegisterMetaType< QVector<Node *> >("QVector<Node *>");
    connect( alg, SIGNAL( finishedEulerian( QVector< Node * >  ) ), this, SLOT( processEulerianResults( QVector< Node * > ) ) );

    added_edge_to_make_Eulerian_cycle = nullptr;

    int cnt = QRandomGenerator::global()->bounded(2, 10);
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
void GraphWidget::setWeighted(bool isW){
    isWeighted = isW;
    for (Node * node : graph) {
        for (Edge * edge : node->get_edges()) {
            if (isWeighted) edge->show_weight_label();
            else edge->hide_weight_label();
        }
    }

    emit graphChanged();
}
void GraphWidget::setWeighted(){
    QRadioButton *but =  qobject_cast<QRadioButton *>(sender());
    bool buttonDirValue = false;
    if(but->objectName() == "weighted")
        buttonDirValue = true;
    setWeighted(buttonDirValue);
}
void GraphWidget::setFontSize(int size)
{
    font.setPointSize(size);
    for(Node *i:graph){
        i->update();
        for(Edge *j:i->get_edges())
            j->setFontSize(size);
    }
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
    edgesColorChange();
}
void GraphWidget::edgesColorChange()
{
    for(Node* i:get_graph()){
        for(Edge* j:i->get_edges())
            j->set_color(edgeColor);
    }
}

void GraphWidget::graphDraw()
{
    QTextEdit *gtext = this->parent()->findChild<QTextEdit *>("textEdit");

    QList<QString> edges = gtext->toPlainText().split('\n');

    //this is an inspector,
    //that checks all nodes are in data and at scene
    QMap<Node*, bool> inspect;

    for(Node *i:graph)
        inspect[i] = false;




/*
    //edge deleting
    QStack<Edge*> odd;

    for(Node *i:graph){
        inspect[i] = false;
        for(Edge *j:i->get_edges()){

            if (j->get_destination_node() == NULL || j->get_source_node() == NULL) continue;

            if(!edges.contains(j->get_source_node()->get_name()+
                               " "+j->get_destination_node()->get_name()+" "+j->get_weight_str())
                    &&
                    !edges.contains(j->get_source_node()->get_name()+
                                                   " "+j->get_destination_node()->get_name())
                    ){
                delete j;
            }
        }
    }*/



//    draw graph by data
//    uNode - first node in line
//    vNode - second node in line
//    weight - third record in line, weight
    Node *uNode, *vNode;
    QString weight;

    for(QString edge:edges) {
        //if(/*!edge.contains(' ') || */*edge.end() == ' ')continue;
        uNode=NULL;
        vNode=NULL;
        weight = "1";
        QList<QString> u = edge.split(' ');

        // if it is a single node
        if(!edge.contains(' ') || u[1]=="" ){
            bool ch=false;
            for(Node *i:graph){
                if(i->get_name() == u[0]){
                    inspect[i]=true;
                    ch = true;
                    break;
                }
            }
            if(ch==false && u[0]!=""){
                uNode = new Node(this, u[0]);
                sc->addItem(uNode);
            }
            continue;

        }

        // if there are two nodes
        if(u.size()==3){
            if(u[2] != "")
                weight = u[2];

        }

        for(Node *i:graph){
            if(i->get_name() == u[0]){
                inspect[i]=true;
                uNode = i;
            }
            if(i->get_name() == u[1]){
                inspect[i]=true;
                vNode = i;
            }
        }


        if(u[0] == u[1])
            continue;
        if (uNode == NULL) {
            uNode = new Node(this, u[0]);
            sc->addItem(uNode);
        }
        if (vNode == NULL) {
            vNode = new Node(this, u[1]);
            sc->addItem(vNode);
        }

        if(!uNode->is_adjacent_with(vNode)){
            //edge do not exist
            sc->addItem(new Edge(this, uNode, vNode, isDirected, weight.toInt()));
        }else{
            //edge exist
            Edge * t = uNode->get_edge(vNode);
            if(u.size()==3 && weight.toInt() != t->get_weight()){

                t->set_weight(weight.toInt());
            }
        }
    }

    //inspector delete odd nodes
    QMap<Node*, bool>::iterator i;

    for (i = inspect.begin(); i != inspect.end(); ++i)
        if(i.value()==false){
            delete i.key();

        }

    //edge deleting
    QVector<Node*> odd;
    QVector<Edge *> edge_to_del;
    bool need_to_del;

    for(Node *i:graph){
        for(Edge *j:i->get_edges()){

            if (i == nullptr || j == nullptr || j->get_destination_node() == nullptr || j->get_source_node() == nullptr) continue;

//            if(!edges.contains(j->get_source_node()->get_name()+" "+j->get_destination_node()->get_name())){
//                qDebug() << j->get_source_node()->get_name()+" ! "+j->get_destination_node()->get_name();
//            }

            need_to_del = 1;

            for (QString str : edges) {
                if ( str.indexOf( j->get_source_node()->get_name()+" "+j->get_destination_node()->get_name() ) == 0 ) {
                    need_to_del = 0;
                }
            }

            if (need_to_del) delete j;

        }
    }

}
void GraphWidget::radiusChange(int rad)
{
    radius = rad;
    for(Node *i:graph)
        i->set_radius(rad);
}
void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << algos_thread.isRunning();
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
}

void GraphWidget::start_dfs(QString name)
{
    if ( algos_thread.isRunning() ) {

        QMessageBox * msg = new QMessageBox();
        msg->setText("Algorithm is still running");
        msg->setIcon(QMessageBox::Information);
        msg->show();

        return;
    }

    nodesColorChange();
    edgesColorChange();

    Node * start = nullptr;
    for (int i = 0; i < cnt_of_nodes; i++) {
        if (graph[i]->get_name() == name) {
            start = graph[i];
        }
    }

    if (start == nullptr) return;


    alg->setup_dfs(start);
    connect( &algos_thread, SIGNAL( started() ), alg, SLOT( start_dfs_alg() ) );


    algos_thread.start();

}



void GraphWidget::start_bfs(QString name)
{
    if ( algos_thread.isRunning() ) {

        QMessageBox * msg = new QMessageBox();
        msg->setText("Algorithm is still running");
        msg->setIcon(QMessageBox::Information);
        msg->show();

        return;
    }

    nodesColorChange();
    edgesColorChange();

    Node * start = nullptr;
    for (int i = 0; i < cnt_of_nodes; i++) {
        if (graph[i]->get_name() == name) {
            start = graph[i];
        }
    }

    if (start == nullptr) return;


    alg->setup_bfs(start);
    connect( &algos_thread, SIGNAL( started() ), alg, SLOT( start_bfs_alg() ) );

    algos_thread.start();
}

void GraphWidget::disconnect_thread()
{
    algos_thread.quit();
    algos_thread.disconnect();
}
void GraphWidget::processDijkstraResults( QMap< Node *, int > dijkstraDistance )
{
    disconnect_thread();

    QTableWidget * table = new QTableWidget( 1 , dijkstraDistance.size() );
    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QTableWidget::NoSelection);

    QMap< Node *, int >::iterator it;
    QVector< QPair< std::string, int > > v;


    for (it = dijkstraDistance.begin(); it != dijkstraDistance.end(); it++) {
        v.push_back({ it.key()->get_name().toStdString() , it.value() });
    }

    std::sort( v.begin(), v.end() );

    for (int i = 0; i < v.size(); i++) {
        qDebug() << QString::fromStdString( v[i].first ) << " " << v[i].second;
    }

    for ( int i = 0; i < v.size(); i++ ) {
        table->setHorizontalHeaderItem( i, new QTableWidgetItem( QString::fromStdString( v[i].first ) ) );

        if ( v[i].second == INT_MAX ) {
            table->setItem( 0, i,  new QTableWidgetItem("∞") );
        }
        else  {
            table->setItem( 0, i, new QTableWidgetItem( QString::number( v[i].second ) ) );
        }

        table->item(0, i)->setTextAlignment( Qt::AlignCenter );
    }

    table->resizeColumnsToContents();
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int w = 0, h = 0;

    w += table->contentsMargins().left()
            + table->contentsMargins().right();
    h += table->contentsMargins().top()
            + table->contentsMargins().bottom();


    w += table->verticalHeader()->width();
    h += table->horizontalHeader()->height();


    for (int i = 0; i < table->columnCount(); ++i)
        w += table->columnWidth(i);
    w += table->columnWidth(0);
    for (int i = 0; i < table->rowCount(); ++i)
        h += table->rowHeight(i);

    table->setFixedSize(w, h);


    table->setVerticalHeaderItem(0, new QTableWidgetItem("Distance") );

    table->setWindowTitle("Dijkstra results");

    QMainWindow * wind = new QMainWindow();

    wind->setCentralWidget( table );
    wind->setContentsMargins(10, 10, 10, 10);
    wind->setFixedSize( table->width() + 20, table->height() + 20 );
    wind->setWindowTitle("Dijkstra results");
    wind->show();

}


void GraphWidget::start_dijkstra(QString name)
{
    if ( algos_thread.isRunning() ) {

        QMessageBox * msg = new QMessageBox();
        msg->setText("Algorithm is still running");
        msg->setIcon(QMessageBox::Information);
        msg->show();

        return;
    }

    nodesColorChange();
    edgesColorChange();

    Node * start = nullptr;
    for (int i = 0; i < cnt_of_nodes; i++) {
        if (graph[i]->get_name() == name) {
            start = graph[i];
        }
    }

    if (start == nullptr) return;


    alg->setup_dijkstra(start);
    connect( &algos_thread, SIGNAL( started() ), alg, SLOT( start_dijkstra_alg() ) );

    algos_thread.start();
}


QSet< Edge * > GraphWidget::getAllEdges() {
    QSet< Edge * > edges;

    for (Node * node : graph) {
        for (Edge * edge : node->get_edges()) {
            edges.insert( edge );
        }
    }
    return edges;
}

void GraphWidget::start_kruskal()
{
    if ( algos_thread.isRunning() ) {

        QMessageBox * msg = new QMessageBox();
        msg->setText("Algorithm is still running");
        msg->setIcon(QMessageBox::Information);
        msg->show();

        return;
    }

    nodesColorChange();
    edgesColorChange();

    alg->setup_kruskal();
    connect( &algos_thread, SIGNAL( started() ), alg, SLOT( start_kruskal_alg() ) );

    algos_thread.start();
}

bool GraphWidget::isEulerian()
{
    int from = 0, to = 0;
    for (Node * node : graph) {

        if ( !isDirected && (node->get_edges().size() % 2 == 1 ) ) return false;

        if ( isDirected ) {
            for (Edge * edge : node->get_edges()) {
                if (edge->get_source_node() == node) from++;
                else to++;
            }
            if (from != to) return false;
        }
    }
    return true;
}

bool GraphWidget::isSemiEulerian(Node *&start_node, Node *&finish_node)
{
    start_node = nullptr;
    finish_node = nullptr;
    int from = 0, to = 0;

    for (Node * node : graph) {

        from = to = 0;

        if ( !isDirected && (node->get_edges().size() % 2 == 1 ) ) {

            if (start_node == nullptr) start_node = node;
            else if (finish_node == nullptr) finish_node = node;
            else return false;

        }

        if ( isDirected ) {
            for (Edge * edge : node->get_edges()) {
                if (edge->get_source_node() == node) from++;
                else to++;
            }
            if (to - from == 0) {
                continue;
            }

            if (to - from == 1) {

                if (start_node == nullptr) start_node = node;
                else return false;

            }
            else if (from - to == 1) {

                if (finish_node == nullptr) finish_node = node;
                else return false;

            }
            else return false;
        }
    }

    return true;

}

void GraphWidget::start_eulerian()
{
    if ( algos_thread.isRunning() ) {

        QMessageBox * msg = new QMessageBox();
        msg->setText("Algorithm is still running");
        msg->setIcon(QMessageBox::Information);
        msg->show();

        return;
    }

    Node * start = nullptr;
    Node * finish = nullptr;

    if (!isEulerian()) {


        if (!isSemiEulerian(start, finish)) {
            //qDebug() << "!";
            QMessageBox * msg = new QMessageBox();
            msg->setText("Graph is not Eulerian");
            msg->setIcon(QMessageBox::Critical);
            msg->show();

            return;

        }

    }


    if (start == nullptr) start = graph[0];

    if (finish != nullptr) {
        added_edge_to_make_Eulerian_cycle = new Edge(this, start, finish, false);
        qDebug() << start->get_name() << " " << finish->get_name();
    }

    nodesColorChange();
    edgesColorChange();

    alg->setup_eulerian(start);
    connect( &algos_thread, SIGNAL( started() ), alg, SLOT( start_eulerian_alg() ) );

    algos_thread.start();


}

void GraphWidget::processEulerianResults(QVector<Node *> eulerian_path)
{
    disconnect_thread();
    for (Node * node : eulerian_path) {
        qDebug() << node->get_name();
    }

    int n = eulerian_path.size();


    Edge * edge = nullptr;

    for (int i = 0; i < n - 1; i++ ) {
        edge = eulerian_path[i]->get_edge( eulerian_path[ i + 1 ] );

        if (edge == added_edge_to_make_Eulerian_cycle) continue;

        if (edge->get_source_node() != eulerian_path[i]) {
            qDebug() << edge->get_source_node()->get_name() << " ! " << edge->get_destination_node()->get_name();

            edge->switchDirection();

            qDebug() << edge->get_source_node()->get_name() << " ! " << edge->get_destination_node()->get_name() << "\n";
        }

    }

    if (added_edge_to_make_Eulerian_cycle != nullptr) {

        added_edge_to_make_Eulerian_cycle->get_source_node()->set_color( QColor("#b072a9") );
        added_edge_to_make_Eulerian_cycle->get_destination_node()->set_color( QColor("red") );

        delete added_edge_to_make_Eulerian_cycle;
        added_edge_to_make_Eulerian_cycle = nullptr;


    }

    setDirected(true);
}

#include <QtWidgets>
#include <QDebug>
#include <QFile>
#include "mainwindow.h"
#include "node.h"
#include "edge.h"
#include <QTableView>


QWidget *loadUi(const QString url) // Return object QWidget
{
    QUiLoader loader;
    QFile ui_file(url);
    ui_file.open(QFile::ReadOnly);
    // Load .ui file and return pointer for object created
    QWidget* widget = loader.load(&ui_file, NULL);
    ui_file.close();


    return widget;
}

MainWindow::MainWindow()
{

    //items:generate a widget with a .ui file
    QWidget *formWidget = loadUi("../graph_editor/tabwidget.ui");
    //"../graph_editor/tabwidget.ui"

    //items
    //items:scene for drawing
    //GraphWidget *gwidget = new GraphWidget(this);

    gwidget = new GraphWidget(this);
    // !
    gwidget->setFixedSize(500, 500);
    //gwidget->setMinimumSize(500, 500);


    graph_data_changes_timer = new QTimer;
    connect(graph_data_changes_timer, SIGNAL(timeout()), gwidget, SLOT( graphDraw() ) );

    //menu
    createActions();
    createMenus();

    //containers
    formWidget->setMinimumSize(375, 492);

    QWidget *mainContainer = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
        mainLayout->addWidget(formWidget);
        mainLayout->addWidget(gwidget);
    mainLayout->setContentsMargins(10,0,35,0);
    mainContainer->setLayout(mainLayout);

    this->setCentralWidget(mainContainer);
    // !
    this->setFixedSize(1000, 600);

    QRadioButton *but  = findChild<QRadioButton*>("buttonDirected");
    gwidget->isDirected = but->isChecked();
    ui_textEdit = findChild<QTextEdit*>("textEdit");
    QComboBox *nodeColor = findChild<QComboBox*>("nodeColor");
    QComboBox *edgeColor = findChild<QComboBox*>("edgeColor");

    QPushButton *start_dfs_button = findChild<QPushButton *>("dfs_button");
    QPushButton *start_bfs_button = findChild<QPushButton *>("bfs_button");

    //start_vertex->text()

    connect(nodeColor, SIGNAL(currentTextChanged(QString)), gwidget, SLOT(nodesColorChange(QString)));
    connect(edgeColor, SIGNAL(currentTextChanged(QString)), gwidget, SLOT(edgesColorChange(QString)));
    connect(ui_textEdit, SIGNAL(textChanged()), this, SLOT(start_graph_data_changes_timer()));
    connect(gwidget, SIGNAL(graphChanged()), this, SLOT(graphWrite()));
    connect(start_dfs_button, SIGNAL(clicked()), this, SLOT( start_dfs() ) );
    connect(start_bfs_button, SIGNAL(clicked()), this, SLOT( start_bfs() ) );
    //connect(gwidget, SIGNAL(edgeAdded(Edge * edge)), this, SLOT( addEdgeToGraphData(Edge * edge) ) );

    connect(findChild<QRadioButton*>("weighted"), SIGNAL(pressed()), gwidget, SLOT(setWeighted()));
    connect(findChild<QRadioButton*>("unweighted"), SIGNAL(pressed()), gwidget, SLOT(setWeighted()));

    connect(findChild<QRadioButton*>("buttonDirected"), SIGNAL(pressed()), gwidget, SLOT(setDirected()));
    connect(findChild<QRadioButton*>("buttonUndirected"), SIGNAL(pressed()), gwidget, SLOT(setDirected()));
    connect(gwidget, SIGNAL(dirChanged(bool)), findChild<QRadioButton*>("buttonDirected"), SLOT(setChecked(bool)));
    connect(gwidget, SIGNAL(dirChangedReverse(bool)), findChild<QRadioButton*>("buttonUndirected"), SLOT(setChecked(bool)));

    connect(findChild<QPushButton *>("dijkstra_button"), SIGNAL(clicked()), this, SLOT( start_dijkstra() ) );
    connect(findChild<QPushButton *>("kruskal_button"), SIGNAL(clicked()), this, SLOT( start_kruskal() ) );
    connect(findChild<QPushButton *>("eulerian_button"), SIGNAL(clicked()), this, SLOT( start_eulerian() ) );


    emit gwidget->graphChanged();
}

//write graph in Graph Data
void MainWindow::graphWrite()
{
    QVector <Node *> tgraph = gwidget->get_graph();
    QString str = "";

    for(Node *i:tgraph){
        if(i->get_edges().size()==0)
            str = str + i->get_name() + "\n";
    }

    for(int i = 0; i < tgraph.size(); i++){
        for(Edge * j:tgraph[i]->get_edges()){
            if(tgraph[i]->get_name() == j->get_destination_node()->get_name())
                continue;

            str = str + tgraph[i]->get_name() + " ";
            str = str + j->get_destination_node()->get_name();
            if(gwidget->isWeighted)
                str = str + " " + QString::number(j->get_weight());
            str = str + "\n";
            //qDebug()<<"!-"<<j->get_weight();
        }
    }
    ui_textEdit->setText(str);
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    QString data;

    if(fileName.contains(".txt")){
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly)) {
            QMessageBox::warning(this, tr(""),
                                 tr("Cannot read file %1:\n%2")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file.errorString()));
            return;
        }
        data = file.readAll();
    }
    if(fileName.contains(".gv")){

        data = openDot(fileName);

    }


    ui_textEdit->setPlainText(data);
    //emit gwidget->graphChanged();
}


QString MainWindow::openDot(QString fileName){

    QString res="";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr(""),
                             tr("Cannot read file %1:\n%2")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return "";
    }
    QList<QByteArray> strs = file.readAll().split('\n');
    QByteArray splitter;
    if(strs[0].contains("digraph ")){
        gwidget->setDirected(true);
        splitter = " -> ";
    }else{
        gwidget->setDirected(false);
        splitter = " -- ";
    }

    //if record contain splitter then add to res

    QString weight;
    for(QByteArray i:strs){
        if(i.contains(splitter)){
            weight="";
            QList<QString> t = ((QString)i).split(splitter);

            if(t[1].contains("[label")){
                res+=t[0]+" ";
                for(int i=0;i<t[1].indexOf('[');i++){
                    res += t[1][i];
                }
                for(int i=t[1].indexOf('=')+1;i<t[1].size();i++){
                    if(t[1][i] == "]" || t[1][i] == ";")
                        break;
                    else
                        weight += t[1][i];
                }
                res += " " + weight;
            }else{
                t[1].chop(2);
                res += t[0] + " " + t[1];
            }
            res += "\n";
        }else if(!i.contains('{') && !i.contains('}')
                 && !i.contains('[') && !i.contains(']')){
            res += i;

            res += "\n";
        }
    }

    return res;

}
void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("This application allows to construct graphs"
               "<ul><li><b>Left Click</b> on the right veiw creates a node</li>"
               "<li><b>Right Click</b> on node creates edge from it,"
               " Second right click makes two nodes connected</li>"
               "<li><b> Shift + Left Click</b> delete items</li></ul>"));

}
void MainWindow::saveAsPNG(){
    const QString title = tr("Save As PNG(%1)");
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save graph"), "",
        tr("Image (*.png);;All Files (*)"));
    QGraphicsScene *scene = gwidget->sc;
    QImage image(gwidget->size(), QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    scene->render(&painter);
    image.save(fileName);
}

void MainWindow::save(){
    if(fileName == NULL){
        saveAs();
        return;
    }
    saveFile();
}



void MainWindow::saveAs(){
    const QString title = tr("Save As (%1)");


    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save graph"), "",
        tr("text (*.txt);; dot (*.gv);;Image (*.png);;All Files (*)"));
    this->fileName = fileName;
    saveFile();

}
void MainWindow::saveFile(){
    if(fileName.contains(".png")){
        QGraphicsScene *scene = gwidget->sc;
        QImage image(gwidget->size(), QImage::Format_ARGB32);
        image.fill(Qt::white);

        QPainter painter(&image);
        scene->render(&painter);
        image.save(fileName);
        return;
    }
    if(fileName.contains(".txt")){
        if (fileName.isEmpty())
            return;
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Cannot write file %1:\n%2")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file.errorString()));
            return;
        }

        QTextStream out(&file);
        out << ui_textEdit->toPlainText();
        return;
    }
    if(fileName.contains(".gv")){
        if (fileName.isEmpty())
            return;
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Cannot write file %1:\n%2")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file.errorString()));
            return;
        }
        QTextStream out(&file);
        out << toDot(fileName);
        return;
    }
}



void MainWindow::createActions(){
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setStatusTip(tr("Save"));
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    saveAsAct = new QAction(tr("&Save As.."), this);
    saveAsAct->setStatusTip(tr("Save As"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    saveAsPNGAct = new QAction(tr("&Save As PNG"), this);
    saveAsPNGAct->setStatusTip(tr("Save As PNG"));
    connect(saveAsPNGAct, &QAction::triggered, this, &MainWindow::saveAsPNG);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open");
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    aboutAct = new QAction(tr("&about"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveAsPNGAct);

    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    /*editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addSeparator();
    editMenu->addSeparator();*/

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

}


QString MainWindow::toDot(QString file) {
    QFileInfo fi(file);
    QString name = fi.baseName();

    QString resDot, splitter, label;


    if(gwidget->isDirected == true){
        //if graph directed
        resDot = "digraph "+ name +"{\n";
        splitter = " -> ";
    }else{
        //if graph not directed
        resDot = "graph "+ name +"{\n";
        splitter = " -- ";
    }

    //print all nodes
    for(Node *node:gwidget->get_graph())
        resDot += node->get_name() + "\n";
    //print edges
    for(Node *node:gwidget->get_graph()){
        for(Edge *edge:node->get_edges()){
            if(node == edge->get_destination_node())continue;
            resDot += node->get_name() + splitter;
            resDot += edge->get_destination_node()->get_name();
            if(gwidget->isWeighted)
                resDot += " [label ="+ QString::number(edge->get_weight()) +" ]";
            resDot += ";\n";

        }
    }
    resDot += "}";
    return resDot;
}

void MainWindow::start_dfs()
{
    QLineEdit * start_vertex = findChild<QLineEdit *>("start_vertex");
    gwidget->start_dfs( start_vertex->text() );

}
void MainWindow::start_bfs()
{
    QLineEdit * start_vertex = findChild<QLineEdit *>("start_vertex");
    gwidget->start_bfs( start_vertex->text() );

}
void MainWindow::start_dijkstra()
{
    QLineEdit * start_vertex = findChild<QLineEdit *>("start_vertex");
    gwidget->start_dijkstra( start_vertex->text() );
}
void MainWindow::start_kruskal()
{
    gwidget->start_kruskal();
}
void MainWindow::start_eulerian()
{
    gwidget->start_eulerian();
}

void MainWindow::start_graph_data_changes_timer()
{
    if (graph_data_changes_timer->isActive())
        graph_data_changes_timer->stop();

    graph_data_changes_timer->setInterval(500);
    graph_data_changes_timer->setSingleShot(true);

    graph_data_changes_timer->start();
}


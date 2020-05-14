#include <QtWidgets>
#include <QDebug>
#include "mainwindow.h"
#include "node.h"
#include "edge.h"

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
    //items
    //items:scene for drawing
    //GraphWidget *gwidget = new GraphWidget(this);
    gwidget = new GraphWidget(this);
        gwidget->setFixedSize(500, 500);

    //items:generate a widget with a .ui file
    QWidget *formWidget = loadUi("../graph_editor/tabwidget.ui");

    //menu
    createActions();
    createMenus();

    //containers
    QWidget *mainContainer = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
        mainLayout->addWidget(formWidget);
        mainLayout->addWidget(gwidget);

    mainContainer->setLayout(mainLayout);

    this->setCentralWidget(mainContainer);
    this->setFixedSize(1000, 600);

    ui_textEdit = findChild<QTextEdit*>("textEdit");
    QComboBox *nodeColor = findChild<QComboBox*>("nodeColor");
    QComboBox *edgeColor = findChild<QComboBox*>("edgeColor");
    connect(nodeColor, SIGNAL(currentTextChanged(QString)), gwidget, SLOT(nodesColorChange(QString)));
    connect(edgeColor, SIGNAL(currentTextChanged(QString)), gwidget, SLOT(edgesColorChange(QString)));
    connect(ui_textEdit, SIGNAL(textChanged()), gwidget, SLOT(graphDraw()));
    connect(gwidget, SIGNAL(graphChanged()), this, SLOT(graphWrite()));
    connect(gwidget, SIGNAL(edgeAdded(Edge * edge)), this, SLOT( addEdgeToGraphData(Edge * edge) ) );

    qDebug() << gwidget->get_graph().size();

    emit gwidget->graphChanged();
}

//write graph in Graph Data
void MainWindow::graphWrite()
{
    QVector <Node *> tgraph = gwidget->get_graph();
    QString str = "";

    for(int i = 0; i < tgraph.size(); i++){
        for(auto j:tgraph[i]->get_edges()){
            if(tgraph[i]->get_name() == j->get_destination_node()->get_name())
                continue;

            str = str + tgraph[i]->get_name() + " ";
            str = str + j->get_destination_node()->get_name() + "\n";
        }
    }
    ui_textEdit->setText(str);
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Codecs"),
                             tr("Cannot read file %1:\n%2")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return;
    }

    const QByteArray data = file.readAll();

    ui_textEdit->setPlainText(data);
    //emit gwidget->graphChanged();
}

void MainWindow::saveAs(){
    const QString title = tr("Save As (%1)");


    QString fileName = QFileDialog::getSaveFileName(this, title);
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


}

void MainWindow::createActions(){
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    saveAsAct = new QAction(tr("&Save As.."), this);
    saveAsAct->setStatusTip(tr("Save As"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open");
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
}

void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(newAct);

    fileMenu->addAction(openAct);

    //fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    //fileMenu->addAction(printAct);

    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    //editMenu->addAction(undoAct);
    //editMenu->addAction(redoAct);
    editMenu->addSeparator();
   // editMenu->addAction(cutAct);
    //editMenu->addAction(copyAct);
    //editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    //helpMenu->addAction(aboutAct);
    //helpMenu->addAction(aboutQtAct);

}

void MainWindow::addEdgeToGraphData(Edge * edge)
{

}

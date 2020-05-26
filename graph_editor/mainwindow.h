#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "graphwidget.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QtUiTools/QUiLoader>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class GraphWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        QString toDot(QString file);

    public slots:
        void graphWrite();
        //void addEdgeToGraphData(Edge * edge);

        void start_dfs();
        void start_bfs();

    private slots:
        void start_graph_data_changes_timer();

private:
    void createActions();
    void createMenus();
    void saveAs();
    void saveAsPNG();
    void open();
    QString openGV(QString file);

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *saveAsPNGAct;
    QAction *exitAct;

    QTextEdit *ui_textEdit;
    GraphWidget *gwidget;
    //QString fileName;

    QTimer * graph_data_changes_timer;
};


#endif // MAINWINDOW_H

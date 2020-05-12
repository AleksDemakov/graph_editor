/********************************************************************************
** Form generated from reading UI file 'tabwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABWIDGET_H
#define UI_TABWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QComboBox *nodeColor;
    QLabel *label;
    QLabel *label_2;
    QComboBox *edgeColor;
    QGroupBox *groupBox;
    QRadioButton *buttonUndirected;
    QRadioButton *buttonDirected;
    QWidget *tab_2;
    QTextEdit *textEdit;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(375, 492);
        tabWidget = new QTabWidget(Form);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 351, 451));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        nodeColor = new QComboBox(tab);
        nodeColor->addItem(QString());
        nodeColor->addItem(QString());
        nodeColor->addItem(QString());
        nodeColor->addItem(QString());
        nodeColor->addItem(QString());
        nodeColor->setObjectName(QString::fromUtf8("nodeColor"));
        nodeColor->setGeometry(QRect(110, 20, 81, 31));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 71, 16));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 71, 16));
        edgeColor = new QComboBox(tab);
        edgeColor->addItem(QString());
        edgeColor->addItem(QString());
        edgeColor->addItem(QString());
        edgeColor->addItem(QString());
        edgeColor->addItem(QString());
        edgeColor->setObjectName(QString::fromUtf8("edgeColor"));
        edgeColor->setGeometry(QRect(110, 60, 81, 31));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 110, 151, 91));
        buttonUndirected = new QRadioButton(groupBox);
        buttonUndirected->setObjectName(QString::fromUtf8("buttonUndirected"));
        buttonUndirected->setGeometry(QRect(20, 30, 103, 21));
        buttonUndirected->setChecked(true);
        buttonDirected = new QRadioButton(groupBox);
        buttonDirected->setObjectName(QString::fromUtf8("buttonDirected"));
        buttonDirected->setGeometry(QRect(20, 60, 103, 21));
        buttonDirected->setChecked(false);
        tabWidget->addTab(tab, QString());
        groupBox->raise();
        nodeColor->raise();
        label->raise();
        label_2->raise();
        edgeColor->raise();
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        textEdit = new QTextEdit(tab_2);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 311, 391));
        QFont font;
        font.setPointSize(12);
        textEdit->setFont(font);
        tabWidget->addTab(tab_2, QString());

        retranslateUi(Form);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "Form", nullptr));
        nodeColor->setItemText(0, QCoreApplication::translate("Form", "Black", nullptr));
        nodeColor->setItemText(1, QCoreApplication::translate("Form", "Yellow", nullptr));
        nodeColor->setItemText(2, QCoreApplication::translate("Form", "Blue", nullptr));
        nodeColor->setItemText(3, QCoreApplication::translate("Form", "Pink", nullptr));
        nodeColor->setItemText(4, QCoreApplication::translate("Form", "White", nullptr));

        label->setText(QCoreApplication::translate("Form", "Node Color", nullptr));
        label_2->setText(QCoreApplication::translate("Form", "Edge Color", nullptr));
        edgeColor->setItemText(0, QCoreApplication::translate("Form", "Black", nullptr));
        edgeColor->setItemText(1, QCoreApplication::translate("Form", "Yellow", nullptr));
        edgeColor->setItemText(2, QCoreApplication::translate("Form", "Blue", nullptr));
        edgeColor->setItemText(3, QCoreApplication::translate("Form", "Pink", nullptr));
        edgeColor->setItemText(4, QCoreApplication::translate("Form", "White", nullptr));

        groupBox->setTitle(QString());
        buttonUndirected->setText(QCoreApplication::translate("Form", "undirected", nullptr));
        buttonDirected->setText(QCoreApplication::translate("Form", "directed", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("Form", "Config", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("Form", "Graph Data", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABWIDGET_H

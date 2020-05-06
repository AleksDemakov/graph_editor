#ifndef NODELABEL_H
#define NODELABEL_H


#include <QGraphicsTextItem>

class NodeLabel : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    NodeLabel(const QString& text, QGraphicsItem* parent = 0);
    virtual int type() const;

};

#endif // NODELABEL_H

#include "nodelabel.h"

NodeLabel::NodeLabel(const QString& text, QGraphicsItem* parent) : QGraphicsTextItem(text, parent)
{
    setCacheMode(DeviceCoordinateCache);


}

int NodeLabel::type() const
{
    return Type;
}


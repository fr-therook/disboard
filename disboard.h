#ifndef DISBOARD_H
#define DISBOARD_H

#include <QtQuick/QQuickPaintedItem>

class Disboard : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
        Q_DISABLE_COPY(Disboard)
        public:
                 explicit Disboard(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
    ~Disboard() override;
};

#endif // DISBOARD_H

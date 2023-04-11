#ifndef DISBOARD_MOVELISTMODEL_H
#define DISBOARD_MOVELISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "controller.h"

class MoveListModel : public QAbstractListModel {
Q_OBJECT

    QML_ELEMENT
    Q_DISABLE_COPY(MoveListModel)

    Q_PROPERTY(Controller *controller READ controller WRITE setController NOTIFY controllerChanged REQUIRED)
    Q_PROPERTY(QUuid root READ root WRITE setRoot NOTIFY rootChanged)
public:
    enum ItemRoles {
        WhiteMoveRole = Qt::UserRole + 1, BlackMoveRole
    };

    explicit MoveListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Controller *controller() const;
    void setController(Controller *newValue);

    QUuid root() const;
    void setRoot(QUuid newValue);

private:
    class p;
    std::shared_ptr<p> p;

    void reset(Controller* controller, QUuid root);
    void handleNodePushed(QUuid node);

signals:
    void controllerChanged();
    void rootChanged();
};


#endif //DISBOARD_MOVELISTMODEL_H

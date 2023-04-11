#include "movelistmodel.h"

class MoveListModel::p {
    friend MoveListModel;
public:
    p(Controller* c, QUuid root, MoveListModel *q)
        : c(c), root(root), q(q),
            mainlineNodes(c->board().mainlineNodes(root)) {}

private:
    MoveListModel *q;
    Controller* c;
    QUuid root;

    QVector<QUuid> mainlineNodes;

    disboard::Color rootTurn() const {
        return c->board().turn(root);
    }

    int idxToRow(int idx) const {
        if (rootTurn() == disboard::Color::White) {
            return idx / 2;
        }
        return (idx + 1) / 2;
    }

    int rowToIdx(int row) const {
        if (rootTurn() == disboard::Color::White) {
            return row * 2;
        }
        return row * 2 - 1;
    }

    void addNode(QUuid node) {
        qDebug() << "root:" << root << "node:" << node;

        QUuid curNode = node;
        QVector<QUuid> prevNodes;
        while (true) {
            auto prevNode = c->board().prevNode(curNode);
            if (!prevNode.has_value()) break;
            curNode = *prevNode;
            if (curNode == root) break;
            prevNodes.push_front(curNode);
        }
        qDebug() << "prev:" << prevNodes;
        qDebug() << "main:" << mainlineNodes;
        if (curNode != root) return; // In another tree?

        if (prevNodes.empty()) { // first move?
            if (!mainlineNodes.empty()) { // variation of the first move
                // TODO: signal change of first item
                return;
            }
            q->beginInsertRows({}, 0, 0);
            mainlineNodes.push_back(node);
            q->endInsertRows();
            return;
        }

        int overlap = std::min(mainlineNodes.count(), prevNodes.count());
        int idx = 0;
        for (idx = 0; idx < overlap; idx += 1) {
            if (mainlineNodes[idx] != prevNodes[idx]) break;
        }

        if (idx == overlap) { // identical vectors
            if (mainlineNodes.count() <= prevNodes.count()) {
                // added node is new mainline variation
                Q_ASSERT(prevNodes.count() == mainlineNodes.count());
                auto newRow = idxToRow(mainlineNodes.count());
                if (newRow == idxToRow(mainlineNodes.count() - 1)) {
                    auto newQIdx = q->index(newRow);
                    mainlineNodes.push_back(node);
                    emit q->dataChanged(newQIdx, newQIdx, {WhiteMoveRole, BlackMoveRole});
                    return;
                }

                // Insert new row
                q->beginInsertRows({}, newRow, newRow);
                mainlineNodes.push_back(node);
                q->endInsertRows();

                return;
            }
        }

        // added node is a variation on an existing node
        // TODO: change node
    }
};

MoveListModel::MoveListModel(QObject *parent)
        : QAbstractListModel(parent), p(nullptr) {}

int MoveListModel::rowCount(const QModelIndex &parent) const {
    if (!p) return 0; // default
    if (p->mainlineNodes.empty()) return 0;
    return p->idxToRow(p->mainlineNodes.count() - 1) + 1;
}

QVariant MoveListModel::data(const QModelIndex &index, int role) const {
    if (!p) return {}; // default
    if (index.row() >= rowCount()) return {};
    int idx = p->rowToIdx(index.row());

    if (role == WhiteMoveRole) {
        if (idx < 0) return {};
        auto whiteNode = p->mainlineNodes[idx];
        auto whiteMove = p->c->board().lastMove(whiteNode);
        if (!whiteMove.has_value()) return {};
        return whiteMove->toString();
    }

    if (role == BlackMoveRole) {
        if (idx >= p->mainlineNodes.count() - 1) return {};
        auto blackNode = p->mainlineNodes[idx + 1];
        auto blackMove = p->c->board().lastMove(blackNode);
        if (!blackMove.has_value()) return {};
        return blackMove->toString();
    }
    return {};
}

QHash<int, QByteArray> MoveListModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

    roles[WhiteMoveRole] = "whiteMove";
    roles[BlackMoveRole] = "blackMove";

    return roles;
}

Controller *MoveListModel::controller() const {
    if (!p) return nullptr;
    return p->c;
}

void MoveListModel::setController(Controller *newValue) {
    if (p && (p->c == newValue)) return;
    auto newRoot = newValue->root(); // switch root node
    reset(newValue, newRoot);
    emit controllerChanged();
    emit rootChanged();
}

QUuid MoveListModel::root() const {
    if (!p) return {};
    return p->root;
}

void MoveListModel::setRoot(QUuid newValue) {
    if (p && p->root == newValue) return;
    if (!p) return; // no controller yet
    reset(p->c, newValue);
    emit rootChanged();
}

void MoveListModel::reset(Controller* newC, QUuid newR) {
    beginResetModel();
    {
        if (p) {
            disconnect(p->c, &Controller::nodePushed,
                       this, &MoveListModel::handleNodePushed);
            p.reset();
        }
        connect(newC, &Controller::nodePushed,
                this,&MoveListModel::handleNodePushed);
        p = std::make_shared<class MoveListModel::p>(newC, newR, this);
    }
    endResetModel();
}

void MoveListModel::handleNodePushed(QUuid node) {
    if (!p) return; // how?
    p->addNode(node);
}

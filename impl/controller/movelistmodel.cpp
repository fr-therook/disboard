#include "movelistmodel.h"

class MoveListModel::p {
    friend MoveListModel;
public:
    p(Controller *c, QUuid root, MoveListModel *q)
            : c(c), root(root), q(q),
              mainlineNodes(c->board().mainlineNodes(root)) {}

private:
    MoveListModel *q;
    Controller *c;
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

    int idxToCol(int idx) const {
        if (rootTurn() == disboard::Color::White) {
            return idx % 2;
        }
        return (idx + 1) % 2;
    }

    int modelIdxToIdx(const QModelIndex &idx) const {
        if (rootTurn() == disboard::Color::White) {
            return idx.row() * 2 + idx.column();
        }
        return idx.row() * 2 + idx.column() - 1;
    }

    void addNode(QUuid node) {
        QUuid curNode = node;
        QVector<QUuid> prevNodes;

        // Iterate parents of node until root (excluding)
        while (true) {
            auto prevNode = c->board().prevNode(curNode);
            if (!prevNode.has_value()) break;
            curNode = *prevNode;
            if (curNode == root) break;
            prevNodes.push_front(curNode);
        }

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
                auto oldRow = idxToRow(mainlineNodes.count() - 1);
                auto newRow = idxToRow(mainlineNodes.count());
                if (oldRow == newRow) {
                    auto topLeft = q->index(oldRow, 0);
                    auto bottomRight = q->index(newRow, 1);
                    mainlineNodes.push_back(node);
                    emit q->dataChanged(topLeft, bottomRight, {NodeRole, Qt::DisplayRole});
                    return;
                }

                // Insert new row
                q->beginInsertRows({}, oldRow, oldRow);
                mainlineNodes.push_back(node);
                q->endInsertRows();

                return;
            }
        }

        // added node is a variation on an existing node
        // TODO: change node
//        qDebug() << "variation idx:" << idx;

        auto qIdx = q->index(idxToRow(idx), idxToCol(idx));
        emit q->dataChanged(qIdx, qIdx, {VariationsRole});
    }
};

MoveListModel::MoveListModel(QObject *parent)
        : QAbstractTableModel(parent), p(nullptr) {}

int MoveListModel::rowCount(const QModelIndex &parent) const {
    if (!p) return 0; // default
    if (p->mainlineNodes.empty()) return 0;
    return p->idxToRow(p->mainlineNodes.count() - 1) + 1;
}

int MoveListModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant MoveListModel::data(const QModelIndex &idx, int role) const {
    if (!p) return {}; // default
    if (!idx.isValid()) return {};

    auto nodeIdx = p->modelIdxToIdx(idx);
    if (nodeIdx < 0 || nodeIdx >= p->mainlineNodes.count()) return {};

    QUuid node = p->mainlineNodes[nodeIdx];

    if (role == NodeRole) return node;
    if (role == Qt::DisplayRole) {
        auto move = p->c->board().lastMove(node);
        if (!move.has_value()) return {};
        return move->toString();
    }
    if (role == VariationsRole) {
        auto variations = p->c->board().siblings(node);
        if (variations.empty()) return {};
        return QVariant::fromValue(variations);
    }

    return {};
}

QHash<int, QByteArray> MoveListModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

    roles[NodeRole] = "node";
    roles[VariationsRole] = "variations";

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

void MoveListModel::reset(Controller *newC, QUuid newR) {
    beginResetModel();
    {
        if (p) {
            disconnect(p->c, &Controller::nodePushed,
                       this, &MoveListModel::handleNodePushed);
            p.reset();
        }
        connect(newC, &Controller::nodePushed,
                this, &MoveListModel::handleNodePushed);
        p = std::make_shared<class MoveListModel::p>(newC, newR, this);
    }
    endResetModel();
}

void MoveListModel::handleNodePushed(QUuid node) {
    if (!p) return; // how?
    p->addNode(node);
}

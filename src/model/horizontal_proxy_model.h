#ifndef HORIZONTALPROXYMODEL_H
#define HORIZONTALPROXYMODEL_H

#include <QAbstractProxyModel>

namespace PatientsDBManager::Model
{
    class HorizontalProxyModel : public QAbstractProxyModel
    {
    public:
      HorizontalProxyModel(QObject * parent = 0);

      QModelIndex mapToSource( const QModelIndex&proxyIndex ) const override;
      QModelIndex mapFromSource( const QModelIndex&sourceIndex ) const override;
      QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
      QModelIndex parent( const QModelIndex&child ) const override;
      int rowCount( const QModelIndex &parent ) const override;
      int columnCount( const QModelIndex &parent ) const override;
      QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    };
}

#endif // HORIZONTALPROXYMODEL_H

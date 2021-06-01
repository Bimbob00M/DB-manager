#ifndef HORIZONTALPROXYMODEL_H
#define HORIZONTALPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QMap>

namespace PatientsDBManager
{
    class HorizontalProxyModel : public QAbstractProxyModel
    {
    public:
        HorizontalProxyModel( QObject* parent = nullptr );

        QModelIndex mapToSource( const QModelIndex& proxyIndex ) const override;
        QModelIndex mapFromSource( const QModelIndex& sourceIndex ) const override;
        QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const override;
        QModelIndex parent( const QModelIndex& child ) const override;
        int rowCount( const QModelIndex& parent ) const override;
        int columnCount( const QModelIndex& parent ) const override;
        QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
        Qt::ItemFlags flags( const QModelIndex& index ) const override;

        void setFlags( const QModelIndex& targetIndex, Qt::ItemFlags flags );
        void resetFlags( const QModelIndex& targetIndex );

    private:
        QMap<QModelIndex, Qt::ItemFlags> m_flags;
    };
}

#endif // HORIZONTALPROXYMODEL_H

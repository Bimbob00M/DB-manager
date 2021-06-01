#include "horizontal_proxy_model.h"

namespace PatientsDBManager
{
    HorizontalProxyModel::HorizontalProxyModel( QObject* parent )
        : QAbstractProxyModel( parent )
    {}

    QModelIndex HorizontalProxyModel::mapToSource( const QModelIndex& proxyIndex ) const
    {
        return sourceModel() ? sourceModel()->index( proxyIndex.column(), proxyIndex.row() )
                             : QModelIndex();
    }

    QModelIndex HorizontalProxyModel::mapFromSource( const QModelIndex& sourceIndex ) const
    {
        return index( sourceIndex.column(), sourceIndex.row() );
    }

    QModelIndex HorizontalProxyModel::index( int row, int column, const QModelIndex& ) const
    {
        return createIndex( row, column, nullptr );
    }

    QModelIndex HorizontalProxyModel::parent( const QModelIndex& ) const
    {
        return QModelIndex();
    }

    int HorizontalProxyModel::rowCount( const QModelIndex& ) const
    {
        return sourceModel() ? sourceModel()->columnCount() : 0;
    }

    int HorizontalProxyModel::columnCount( const QModelIndex& ) const
    {
        return sourceModel() ? sourceModel()->rowCount() : 0;
    }

    QVariant HorizontalProxyModel::headerData( int section, Qt::Orientation orientation, int role ) const
    {
        if ( !sourceModel() )
          return QVariant();
        Qt::Orientation new_orientation = orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;

        return sourceModel()->headerData( section, new_orientation, role );
    }

    Qt::ItemFlags HorizontalProxyModel::flags( const QModelIndex& index ) const
    {
        return ( index.isValid() && m_flags.contains( index ) ) ? m_flags[index]
                                                                : QAbstractProxyModel::flags( index );

    }

    void HorizontalProxyModel::setFlags( const QModelIndex& targetIndex, Qt::ItemFlags flags )
    {
        if( targetIndex.isValid() && flags != Qt::NoItemFlags )
            m_flags[targetIndex] = flags;
    }

    void HorizontalProxyModel::resetFlags( const QModelIndex& targetIndex )
    {
        if( targetIndex.isValid() )
            m_flags.remove( targetIndex );
    }
}

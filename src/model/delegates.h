#ifndef DELEGATES_H
#define DELEGATES_H

#include <QStyledItemDelegate>
#include <QList>
#include <QItemDelegate>

namespace PatientsDBManager
{

    class NotModifiableItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit NotModifiableItemDelegate( QObject* parent = nullptr );

    protected:
        QWidget* createEditor( QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const override;
        void setEditorData( QWidget* editor,
                            const QModelIndex& index ) const override;
        void setModelData( QWidget* editor,
                           QAbstractItemModel* model,
                           const QModelIndex& index ) const override;
        void updateEditorGeometry( QWidget* editor,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index ) const override;
    };


//=========================================================================


    class NotNullItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit NotNullItemDelegate( const QString& toolTip, QObject* parent = nullptr );

    protected:
        QWidget* createEditor( QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const override;
        void setEditorData( QWidget* editor,
                            const QModelIndex& index ) const override;
        void setModelData( QWidget* editor,
                           QAbstractItemModel* model,
                           const QModelIndex& index ) const override;
        void updateEditorGeometry( QWidget* editor,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index ) const override;

    private:
        QString m_tooltip;
    };


//=====================================================================================


    class DateItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit DateItemDelegate( QObject* parent = nullptr );

        void connectMinimalDateToColumn( int column ) noexcept;
        void connectMinimalDateToRow( int row ) noexcept;
        void setMinimalDate( bool enable ) noexcept;

    protected:
        QWidget* createEditor( QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const override;
        void setEditorData( QWidget* editor,
                            const QModelIndex& index ) const override;
        void setModelData( QWidget* editor,
                           QAbstractItemModel* model,
                           const QModelIndex& index ) const override;
        void updateEditorGeometry( QWidget* editor,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index ) const override;

    private:
        QList<int> m_connectedColumns;
        QList<int> m_connectedRow;
        bool       m_isCurrentDateAsMinimal{ false };
    };
}



#endif // DELEGATES_H

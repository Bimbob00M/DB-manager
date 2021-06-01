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


    class RegexItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit RegexItemDelegate( const QString& pattern, QObject* parent = nullptr );

        void setPattern( const QString& pattern ) noexcept;
        void setToolTip( const QString& toolTip ) noexcept;
        void setInputMask( const QString& mask ) noexcept;

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
        QString m_regexPattern;
        QString m_tooltip;
        QString m_inputMask;
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
        void setNullable( bool enable ) noexcept;

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
        bool       m_nullable{ false };
    };


//=====================================================================================


    class DateTimeItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit DateTimeItemDelegate( QObject* parent = nullptr );

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
}



#endif // DELEGATES_H

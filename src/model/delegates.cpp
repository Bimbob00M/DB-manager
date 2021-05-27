#include "delegates.h"

#include <QDateEdit>
#include <QDebug>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QToolTip>

#include "utility/global.h"

namespace PatientsDBManager
{

    NotModifiableItemDelegate::NotModifiableItemDelegate( QObject* parent )
        : QItemDelegate( parent )
    {}

    QWidget* NotModifiableItemDelegate::createEditor( QWidget* parent,
                                                const QStyleOptionViewItem& /*option*/,
                                                const QModelIndex& /*index*/ ) const
    {
        auto editor = new QLineEdit( parent );
        editor->setEnabled( false );
        return editor;
    }

    void NotModifiableItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QString value = index.model()->data( index, Qt::EditRole ).toString();
        QLineEdit* line = static_cast<QLineEdit*>( editor );
        line->setText( value );
    }

    void NotModifiableItemDelegate::setModelData( QWidget* /*editor*/,
                                            QAbstractItemModel* /*model*/,
                                            const QModelIndex& /*index*/ ) const
    {}

    void NotModifiableItemDelegate::updateEditorGeometry( QWidget* editor,
                                                    const QStyleOptionViewItem& option,
                                                    const QModelIndex& /*index*/ ) const
    {
        editor->setGeometry( option.rect );
    }


//==========================================================================================


    NotNullItemDelegate::NotNullItemDelegate(const QString &toolTip, QObject* parent )
        : QItemDelegate( parent )
        , m_tooltip( toolTip )
    {}

    QWidget* NotNullItemDelegate::createEditor( QWidget* parent,
                                                const QStyleOptionViewItem& /*option*/,
                                                const QModelIndex& /*index*/ ) const
    {
        auto editor = new QLineEdit( parent );
        editor->setToolTip( m_tooltip );
        return editor;
    }

    void NotNullItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QString value = index.model()->data( index, Qt::EditRole ).toString();
        QLineEdit* line = static_cast<QLineEdit*>( editor );
        line->setText( value );
    }

    void NotNullItemDelegate::setModelData( QWidget* editor,
                                            QAbstractItemModel* model,
                                            const QModelIndex& index ) const
    {
        QLineEdit* line = static_cast<QLineEdit*>( editor );

        QRegExp regex( Global::NOT_EMPTY_STRING_REGEX );
        QRegExpValidator validator( regex );

        int pos = 0;
        auto data = line->text();
        if( validator.validate( data, pos ) != QValidator::Acceptable )
        {
            QToolTip::showText( line->mapToGlobal( line->geometry().topLeft() ), m_tooltip );
        }
        else
        {
            model->setData( index, data );
        }
    }

    void NotNullItemDelegate::updateEditorGeometry( QWidget* editor,
                                                    const QStyleOptionViewItem& option,
                                                    const QModelIndex& /*index*/ ) const
    {
        editor->setGeometry( option.rect );
    }


//==========================================================================================


    DateItemDelegate::DateItemDelegate( QObject* parent )
        : QItemDelegate( parent )
    {}

    void DateItemDelegate::connectMinimalDateToColumn( int column ) noexcept
    {
        m_connectedColumns.append( column );
    }

    void DateItemDelegate::connectMinimalDateToRow( int row ) noexcept
    {
        m_connectedRow.append( row );
    }

    void DateItemDelegate::setMinimalDate( bool enable ) noexcept
    {
        m_isCurrentDateAsMinimal = enable;
    }

    QWidget* DateItemDelegate::createEditor( QWidget* parent,
                                             const QStyleOptionViewItem& /*option*/,
                                             const QModelIndex& index ) const
    {
        auto editor = new QDateEdit( parent );
        editor->setDisplayFormat( Global::DATE_FORMAT );
        editor->setCalendarPopup( true );

        const auto& stringDate = index.model()->data( index, Qt::EditRole ).toString();
        if( !stringDate.isEmpty() )
            if( auto date = QDate::fromString( stringDate, Global::DATE_FORMAT ); date.isValid() )
            {
                if( m_isCurrentDateAsMinimal )
                    editor->setMinimumDate( date );
                else
                    editor->setDate( date );
            }
        return editor;
    }

    void DateItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QString value = index.model()->data( index, Qt::EditRole ).toString();
        auto dateEditor = static_cast<QDateEdit*>( editor );
        dateEditor->setDate( QDate::fromString( value ) );
    }

    void DateItemDelegate::setModelData( QWidget* editor,
                                         QAbstractItemModel* model,
                                         const QModelIndex& index ) const
    {
        auto dateEditor = static_cast<QDateEdit*>( editor );
        const auto& currentDate = dateEditor->date();

        model->setData( index, currentDate.toString( Global::DATE_FORMAT ) );


        auto modifyCell = [&]( const QModelIndex& index )
        {
            if( index.isValid() )
            {
                const auto& siblingStringDate = index.model()->data( index, Qt::EditRole ).toString();
                if( !siblingStringDate.isEmpty() )
                {
                    if( const auto& siblingDate = QDate::fromString( siblingStringDate, Global::DATE_FORMAT ); siblingDate.isValid() )
                    {
                        if( currentDate > siblingDate )
                            model->setData( index, currentDate.toString( Global::DATE_FORMAT ) );
                    }
                    else
                        model->setData( index, currentDate.toString( Global::DATE_FORMAT ) );

                }
            }
        };

        if( !m_connectedColumns.isEmpty() )
        {
            for( const auto column : m_connectedColumns )
            {
                modifyCell( index.siblingAtColumn( column ) );
            }
        }

        if( !m_connectedRow.isEmpty() )
        {
            for( const auto row : m_connectedRow )
            {
                modifyCell( index.siblingAtRow( row ) );
            }
        }

    }

    void DateItemDelegate::updateEditorGeometry( QWidget* editor,
                                                 const QStyleOptionViewItem& option,
                                                 const QModelIndex& /*index*/ ) const
    {
        editor->setGeometry( option.rect );
    }
}


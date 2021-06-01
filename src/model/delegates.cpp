#include "delegates.h"

#include <QDateEdit>
#include <QDebug>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QToolTip>

#include "utility/global.h"
#include "view/date_edit_ex.h"

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

    RegexItemDelegate::RegexItemDelegate( const QString& pattern, QObject* parent )
        : QItemDelegate( parent )
        , m_regexPattern( pattern )
     {}

    void RegexItemDelegate::setPattern( const QString& pattern ) noexcept
    {
       m_regexPattern = pattern;
    }

    void RegexItemDelegate::setToolTip( const QString& toolTip ) noexcept
    {
        m_tooltip = toolTip;
    }

    void RegexItemDelegate::setInputMask( const QString& mask ) noexcept
    {
        m_inputMask = mask;
    }

    QWidget* RegexItemDelegate::createEditor( QWidget* parent,
                                              const QStyleOptionViewItem& /*option*/,
                                              const QModelIndex& /*index*/ ) const
    {
        auto editor = new QLineEdit( parent );
        editor->setToolTip( m_tooltip );
        editor->setInputMask( m_inputMask );
        editor->setValidator( new QRegExpValidator( QRegExp( m_regexPattern ), parent ) );

        return editor;
    }

    void RegexItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QString value = index.model()->data( index, Qt::EditRole ).toString();
        QLineEdit* line = static_cast<QLineEdit*>( editor );
        line->setText( value );
    }

    void RegexItemDelegate::setModelData( QWidget* editor,
                                          QAbstractItemModel* model,
                                          const QModelIndex& index ) const
    {
        QLineEdit* line = static_cast<QLineEdit*>( editor );

        QRegExp regex( Global::NOT_EMPTY_REGEX_PATTERN );
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

    void RegexItemDelegate::updateEditorGeometry( QWidget* editor,
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

    void DateItemDelegate::setNullable( bool enable ) noexcept
    {
        m_nullable = enable;
    }

    QWidget* DateItemDelegate::createEditor( QWidget* parent,
                                             const QStyleOptionViewItem& /*option*/,
                                             const QModelIndex& index ) const
    {
        auto editor = new DateEditEx( parent );
        editor->setDisplayFormat( Global::DATE_FORMAT );
        editor->setCalendarPopup( true );
        editor->setNullable( m_nullable );
        editor->setMinimumDate( QDate( 100, 1, 1 ) );
        editor->reset();

        const auto& stringDate = index.model()->data( index, Qt::EditRole ).toString();

        auto date = QDate::fromString( stringDate, Global::DATE_FORMAT );
        if( m_isCurrentDateAsMinimal )
            editor->setMinimumDate( date );
        else
            editor->setDate( date );

        return editor;
    }

    void DateItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QString value = index.model()->data( index, Qt::EditRole ).toString();
        auto dateEditor = static_cast<DateEditEx*>( editor );
        const auto& date = QDate::fromString( value, Global::DATE_FORMAT );
        dateEditor->setDate( date.isValid() ? date : QDate::currentDate() );
    }

    void DateItemDelegate::setModelData( QWidget* editor,
                                         QAbstractItemModel* model,
                                         const QModelIndex& index ) const
    {
        auto dateEditor = static_cast<DateEditEx*>( editor );
        const auto& currentDate = dateEditor->date();

        model->setData( index, dateEditor->isEmpty() ?
                                   Global::EMPTY_CELL_DEFAULT_VALUE :
                                   currentDate.toString( Global::DATE_FORMAT ) );

        auto modifyCell = [&]( const QModelIndex& index )
        {
            if( index.isValid() )
            {
                const auto& stringDate = index.model()->data( index, Qt::EditRole ).toString();
                const auto& siblingDate = QDate::fromString( stringDate, Global::DATE_FORMAT );

                if( currentDate > siblingDate && siblingDate.isValid() )
                    model->setData( index, currentDate.toString( Global::DATE_FORMAT ) );
                else if( stringDate.isEmpty() )
                    model->setData( index, Global::EMPTY_CELL_DEFAULT_VALUE );
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


//==========================================================================================


    DateTimeItemDelegate::DateTimeItemDelegate( QObject* parent )
        : QItemDelegate( parent )
    {}

    QWidget* DateTimeItemDelegate::createEditor( QWidget* parent,
                                                 const QStyleOptionViewItem& /*option*/,
                                                 const QModelIndex& /*index*/ ) const
    {
        auto editor = new QDateTimeEdit( parent );
        editor->setDisplayFormat( Global::DATE_TIME_FORMAT );
        return editor;
    }

    void DateTimeItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        auto dateTime = QDateTime::fromString( index.model()->data( index, Qt::EditRole ).toString(),
                                               Global::DATE_TIME_FORMAT );
        auto dateTimeEditor = static_cast<QDateTimeEdit*>( editor );
        dateTimeEditor->setDateTime( dateTime.isValid() ? dateTime
                                                        : QDateTime::currentDateTime() );
    }

    void DateTimeItemDelegate::setModelData( QWidget* editor,
                                             QAbstractItemModel* model,
                                             const QModelIndex& index ) const
    {
        auto dateEditor = static_cast<QDateTimeEdit*>( editor );
        auto dateTime = dateEditor->dateTime();
        if( !dateTime.isValid() )
            dateTime = QDateTime::currentDateTime();
        model->setData( index, dateTime.toString( Global::DATE_TIME_FORMAT ) );
    }

    void DateTimeItemDelegate::updateEditorGeometry( QWidget* editor,
                                                     const QStyleOptionViewItem& option,
                                                     const QModelIndex& /*index*/ ) const
    {
        editor->setGeometry( option.rect );
    }

}




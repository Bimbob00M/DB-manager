#include "date_edit_ex.h"

#include <QAction>
#include <QCalendarWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QStyle>
#include <QStyleOptionSpinBox>
#include <QPushButton>

namespace PatientsDBManager
{

    static auto LINE_EDIT_FIELD_NAME = "qt_spinbox_lineedit";

    DateEditEx::DateEditEx( QWidget* parent ) noexcept
        : QDateEdit( parent )
    {
        init();
    }

    DateEditEx::DateEditEx( const QDate& date, QWidget* parent ) noexcept
        : QDateEdit( parent )
    {
        init();
        setDate( date );
    }

    /**
     * \brief returns date, if empty date is invalid
     * \return date, if empty date is invalid
     */
    QDateTime DateEditEx::dateTime() const
    {
        if ( isEmpty() )
            return QDateTime();
         else
            return QDateEdit::dateTime();
    }

    /**
     * \brief returns date, if empty date is invalid
     * \return date, if empty date is invalid
     */
    QDate DateEditEx::date() const
    {
        if ( isEmpty() )
            return QDate();
        else
            return QDateEdit::date();
    }

    /**
     * \brief returns date, if empty date is invalid
     * \return date, if empty date is invalid
     */
    QTime DateEditEx::time() const
    {
        if ( isEmpty() )
            return QTime();
        else
            return QDateEdit::time();
    }

    /**
     * \brief sets a date, if date is invalid a
     * empty date is shown
     */
    void DateEditEx::setDateTime( const QDateTime& dateTime )
    {
        if ( m_nullable && !dateTime.isValid() )
            setNull( true );
        else
        {
            setNull( false );
            QDateEdit::setDateTime( dateTime );
        }
    }

    /**
     * \brief sets a date, if date is invalid a
     * empty date is shown
     */
    void DateEditEx::setDate( const QDate& date )
    {
        if( m_nullable && !date.isValid() )
            setNull( true );
        else
        {
            setNull( false );
            QDateEdit::setDate( date );
        }
    }

    /**
     * \brief sets a date, if date is invalid a
     * empty date is shown
     */
    void DateEditEx::setTime( const QTime& time )
    {
        if ( m_nullable && !time.isValid() )
            setNull( true );
        else
        {
            setNull( false );
            QDateEdit::setTime( time );
        }
    }

    void DateEditEx::setMinimumDateTime( const QDateTime& dateTime )
    {
        if ( m_nullable && !dateTime.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMinimumDateTime( dateTime );
            if( dateTime > this->dateTime() && !isEmpty() )
                setDateTime( dateTime );
        }
    }

    void DateEditEx::setMinimumDate( const QDate& date )
    {
        if ( m_nullable && !date.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMinimumDate( date );
            setNull( isEmpty() );
        }
    }

    void DateEditEx::setMinimumTime( const QTime& time )
    {
        if ( m_nullable && !time.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMinimumTime( time );
            setNull( isEmpty() );
        }
    }

    void DateEditEx::setMaximumDateTime( const QDateTime& dateTime )
    {
        if ( m_nullable && !dateTime.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMaximumDateTime( dateTime );
            setNull( isEmpty() );
        }
    }

    void DateEditEx::setMaximumDate( const QDate& date )
    {
        if ( m_nullable && !date.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMaximumDate( date );
            setNull( isEmpty() );
        }
    }

    void DateEditEx::setMaximumTime( const QTime& time )
    {
        if ( m_nullable && !time.isValid() )
            setNull( true );
        else
        {
            QDateEdit::setMaximumTime( time );
            setNull( isEmpty() );
        }
    }

    void DateEditEx::reset() noexcept
    {
        if( m_nullable )
            setNull( true );
    }

    /**
     * \brief sets weahter the date can be empty
     */
    void DateEditEx::setNullable( bool enable ) noexcept
    {
        m_nullable = enable;

        if ( m_nullable && !m_clearBtn )
        {
            m_clearBtn = new QPushButton( this );
            m_clearBtn->setFlat( true );
            m_clearBtn->setIcon( QIcon( ":/res/edit-clear-icon.png" ) );
            m_clearBtn->setFocusPolicy( Qt::NoFocus );
            m_clearBtn->setFixedSize( 17, m_clearBtn->sizeHint().height() - 6 );
            m_clearBtn->setVisible( !m_null );
            connect( m_clearBtn, &QPushButton::clicked, this, &DateEditEx::clearDate );
        }
        else if ( m_clearBtn )
        {
            disconnect( m_clearBtn , &QPushButton::clicked, this, &DateEditEx::clearDate );
            delete m_clearBtn;
            m_clearBtn = nullptr;
        }
        update();
    }

    void DateEditEx::setDisplayFormat( const QString& format )
    {
        if( !m_null )
            QDateEdit::setDisplayFormat( format );
        else if( auto lineEdit = findChild<QLineEdit*>( LINE_EDIT_FIELD_NAME ) )
            lineEdit->setInputMask( format );
    }

    QSize DateEditEx::sizeHint() const
    {
        const QSize hint = QDateEdit::sizeHint();
        if ( !m_clearBtn )
            return hint;
        return QSize( hint.width() + m_clearBtn->width() + 3, hint.height() );
    }

    QSize DateEditEx::minimumSizeHint() const
    {
        const QSize hint = QDateEdit::minimumSizeHint();
        if ( !m_clearBtn )
            return hint;
        return QSize( hint.width() + m_clearBtn->width() + 3, hint.height() );
    }

    void DateEditEx::showEvent( QShowEvent* event )
    {
        QDateEdit::showEvent( event );
        setNull( m_null ); // force empty string back in
    }

    void DateEditEx::resizeEvent( QResizeEvent* event )
    {
        if ( m_clearBtn )
        {
            QStyleOptionSpinBox opt;
            initStyleOption( &opt );
            opt.subControls = QStyle::SC_SpinBoxUp;

            int left = style()->subControlRect( QStyle::CC_SpinBox,
                                                &opt,
                                                QStyle::SC_SpinBoxUp, this ).left() - m_clearBtn->width() - 3;
            m_clearBtn->move( left, ( height() - m_clearBtn->height() ) / 2 );
        }
        QDateEdit::resizeEvent( event );
    }

    void DateEditEx::paintEvent( QPaintEvent* event )
    {
        QDateEdit::paintEvent( event );
    }

    void DateEditEx::keyPressEvent( QKeyEvent* event )
    {
        if( m_nullable )
        {
            if ( event->key() == Qt::Key_Backspace ||
                 event->key() == Qt::Key_Delete )
            {
                auto lineEdit = findChild<QLineEdit*>( LINE_EDIT_FIELD_NAME );
                if ( lineEdit->selectedText() == lineEdit->text() ) {
                    setDateTime( QDateTime() );
                    event->accept();
                    return;
                }
            }

            if( m_null )
            {
                if ( event->key() == Qt::Key_Tab )
                {
                    QAbstractSpinBox::keyPressEvent( event );
                    return;
                }

                if ( ( event->key() >= Qt::Key_0 ) &&
                     ( event->key() <= Qt::Key_9 ) &&
                     m_null )
                {
                    setDate( QDate::currentDate() );
                    return;
                }
                else if( m_null )
                    return;
            }
        }
        QDateEdit::keyPressEvent( event );
    }

    void DateEditEx::mousePressEvent( QMouseEvent* event )
    {
        QDateEdit::mousePressEvent( event );
        if ( m_nullable && m_null && calendarWidget()->isVisible() )
            setDate( QDate::currentDate() );
    }

    bool DateEditEx::focusNextPrevChild( bool next )
    {
        return isEmpty() ? QAbstractSpinBox::focusNextPrevChild( next )
                         : QDateEdit::focusNextPrevChild( next );
    }

    void DateEditEx::focusOutEvent( QFocusEvent* event )
    {
        QDateEdit::focusOutEvent( event );
        if( isEmpty() )
            setNull( true );
    }

    QValidator::State DateEditEx::validate( QString& input, int& pos) const
    {
        if ( isEmpty() )
            return QValidator::Acceptable;

        return QDateEdit::validate( input, pos );
    }

    bool DateEditEx::init() noexcept
    {
        if( auto clearAction = new ( std::nothrow ) QAction( this ) )
        {
            clearAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_R ) );
            clearAction->setShortcutContext( Qt::WidgetShortcut );
            connect( clearAction, &QAction::triggered, this, &DateEditEx::clearDate );
            this->addAction( clearAction );

            return true;
        }
        return false;
    }

    void DateEditEx::clearDate()
    {
        setNull( true );
    }

    void DateEditEx::setNull( bool enable ) noexcept
    {
        m_null = enable;
        if( m_null )
        {
           auto lineEdit = findChild<QLineEdit*>( LINE_EDIT_FIELD_NAME );
           lineEdit->clear();
        }

        if( m_nullable )
        {
           m_clearBtn->setVisible( !m_null );
        }
    }

}

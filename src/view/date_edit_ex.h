#ifndef DATEEDITEX_H
#define DATEEDITEX_H

#include <QDateEdit>
#include <QFocusEvent>
#include <QLineEdit>
#include <QPushButton>

namespace PatientsDBManager
{
    class DateEditEx : public QDateEdit
    {
        Q_OBJECT
    public:
        explicit DateEditEx( QWidget* parent = nullptr ) noexcept;
        explicit DateEditEx( const QDate& date, QWidget* parent = nullptr ) noexcept;

        QDateTime dateTime() const;
        QDate date() const;
        QTime time() const;

        QSize sizeHint() const override;
        QSize minimumSizeHint() const override;

        bool isEmpty() const noexcept{ return m_nullable && m_null; }

        bool isNullable() const noexcept{ return m_nullable; }
        void setNullable( bool enable ) noexcept;

        void setDisplayFormat( const QString& format );

    public slots:
        void setDateTime( const QDateTime& dateTime );
        void setDate( const QDate& date );
        void setTime( const QTime& time );

        void setMinimumDateTime( const QDateTime& dateTime );
        void setMinimumDate( const QDate& date );
        void setMinimumTime( const QTime& time );

        void setMaximumDateTime( const QDateTime& dateTime );
        void setMaximumDate( const QDate& date );
        void setMaximumTime( const QTime& time );

        void reset() noexcept;

    protected:
        void showEvent( QShowEvent* event ) override;
        void resizeEvent( QResizeEvent* event ) override;
        void paintEvent( QPaintEvent* event ) override;
        void keyPressEvent( QKeyEvent* event ) override;
        void mousePressEvent( QMouseEvent* event ) override;
        bool focusNextPrevChild( bool next ) override;
        void focusOutEvent( QFocusEvent* event ) override;
        QValidator::State validate( QString& input, int& pos ) const override;

    private slots:
        void clearDate();

    private:
        QPushButton* m_clearBtn{ nullptr };

        bool m_null{ false };
        bool m_nullable{ false };

        DateEditEx( const DateEditEx& ) = delete;
        DateEditEx( DateEditEx&& ) = delete;
        DateEditEx& operator=( const DateEditEx& ) = delete;
        DateEditEx& operator=( DateEditEx&& ) = delete;

        bool init() noexcept;
        void setNull( bool enable ) noexcept;

    };
}

#endif // DATEEDITEX_H

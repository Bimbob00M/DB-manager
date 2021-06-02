#include "photo_viewer.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QWheelEvent>

namespace PatientsDBManager
{

    PhotoViewer::PhotoViewer( const QByteArray& binaryImage, QWidget* parent )
        : QDialog( parent )
    {
        if( !init( binaryImage ) )
            close();
    }

    PhotoViewer::PhotoViewer( const QString &title, const QByteArray &binaryImage, QWidget* parent )
        : QDialog( parent )
    {
        if( !init( binaryImage ) )
            close();

        setWindowTitle( title );
    }

    void PhotoViewer::setTitle( const QString& title ) noexcept
    {
        setWindowTitle( title );
    }

    void PhotoViewer::zoomIn() noexcept
    {
        scaleImage( 1.25 );
    }

    void PhotoViewer::zoomOut() noexcept
    {
        scaleImage( 0.8 );
    }

    void PhotoViewer::wheelEvent( QWheelEvent* event )
    {
        if( event->delta() > 0 )
            zoomIn();
        else
            zoomOut();

        event->accept();
    }

    bool PhotoViewer::eventFilter( QObject* o, QEvent* e)
    {
        if( e->type() == QEvent::Wheel )
            return true;
        else
            return QDialog::eventFilter( o, e ); ;
    }

    bool PhotoViewer::init( const QByteArray& binaryImage ) noexcept
    {
        setAttribute( Qt::WA_DeleteOnClose );
        auto image =   new ( std::nothrow ) QPixmap;
        m_imageLabel = new ( std::nothrow ) QLabel( this );
        m_scrollArea = new ( std::nothrow ) QScrollArea( this );

        if( !m_imageLabel ||
            !m_scrollArea ||
            binaryImage.isNull() ||
            binaryImage.isEmpty() )
        {
            return false;
        }

        if( !image->loadFromData( binaryImage ) )
        {
            return false;
        }

        m_imageLabel->setBackgroundRole( QPalette::Base );
        m_imageLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
        m_imageLabel->setScaledContents( true );
        m_imageLabel->setPixmap( *image );

        m_scrollArea->setBackgroundRole( QPalette::Dark );
        m_scrollArea->setWidget( m_imageLabel );
        m_scrollArea->verticalScrollBar()->installEventFilter( this );
        m_scrollArea->horizontalScrollBar()->installEventFilter( this );

        resize( image->size() );
        delete image;

        return setupLayout();
    }

    bool PhotoViewer::setupLayout() noexcept
    {
        auto layout = new ( std::nothrow ) QVBoxLayout;

        if( !layout )
            return false;

        layout->addWidget( m_scrollArea );

        setLayout( layout );
        return true;
    }

    void PhotoViewer::scaleImage( double factor ) noexcept
    {
        m_scaleFactor *= factor;
        m_imageLabel->resize( m_scaleFactor * m_imageLabel->pixmap()->size() );

        adjustScrollBar( m_scrollArea->horizontalScrollBar(), factor );
        adjustScrollBar( m_scrollArea->verticalScrollBar(), factor );
    }

    void PhotoViewer::adjustScrollBar( QScrollBar* scrollBar, double factor ) noexcept
    {
        scrollBar->setValue( int( factor * scrollBar->value() +
                                  ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
    }
}

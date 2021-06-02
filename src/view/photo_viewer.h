#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <QByteArray>
#include <QDialog>
#include <QEvent>
#include <QLabel>
#include <QScrollArea>
#include <QImage>

namespace PatientsDBManager
{
    class PhotoViewer : public QDialog
    {
        Q_OBJECT
    public:
        PhotoViewer( const QByteArray& binaryImage, QWidget* parent = nullptr );
        PhotoViewer( const QString& title, const QByteArray& binaryImage, QWidget* parent = nullptr );

        void setTitle( const QString& title ) noexcept;

    public slots:
        void zoomIn() noexcept;
        void zoomOut() noexcept;

    protected:
        void wheelEvent( QWheelEvent* event ) override;
        bool eventFilter( QObject* o, QEvent* e ) override;

    private:
        QLabel*      m_imageLabel{ nullptr };
        QScrollArea* m_scrollArea{ nullptr };
        double       m_scaleFactor{ 1 };

        bool init( const QByteArray& binaryImage ) noexcept;
        bool setupLayout() noexcept;

        void scaleImage( double factor ) noexcept;
        void adjustScrollBar( QScrollBar* scrollBar, double factor ) noexcept;

    };
}

#endif // PHOTOVIEWER_H

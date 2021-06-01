#include <optional>
#include <utility>

#include <QAbstractButton>
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSqlTableModel>
#include <QTableView>
#include <QVector>

namespace PatientsDBManager::Utility
{
    QByteArray* LoadImage( const QString& fileName )
    {
        QFile file( fileName );
        if( file.open( QIODevice::ReadOnly ) )
        {
            QByteArray* binaryImage = new QByteArray( file.readAll() );
            if( binaryImage->isNull() || binaryImage->isEmpty()  )
            {
                delete binaryImage;
                return nullptr;
            }
            return binaryImage;
        }
        return nullptr;
    }

    void InitImageFileDialog( QFileDialog& dialog, QFileDialog::AcceptMode acceptMode, QFileDialog::FileMode fileMode )
    {
        const auto& picturesLocations = QStandardPaths::standardLocations( QStandardPaths::PicturesLocation );
        dialog.setDirectory( picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last() );

        QStringList mimeTypeFilters{ "image/jpeg" };
        dialog.setMimeTypeFilters( mimeTypeFilters );
        dialog.setAcceptMode( acceptMode );
        dialog.setFileMode( fileMode );
        if ( acceptMode == QFileDialog::AcceptSave )
            dialog.setDefaultSuffix( "jpg" );
    }

}

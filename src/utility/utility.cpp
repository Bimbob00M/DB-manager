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
        const QStringList picturesLocations = QStandardPaths::standardLocations( QStandardPaths::PicturesLocation );
        dialog.setDirectory( picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last() );

        QStringList mimeTypeFilters{ "image/jpeg" };
        dialog.setMimeTypeFilters( mimeTypeFilters );
        dialog.setAcceptMode( acceptMode );
        dialog.setFileMode( fileMode );
        if ( acceptMode == QFileDialog::AcceptSave )
            dialog.setDefaultSuffix( "jpg" );
    }

    QSqlTableModel* GetSqlModel( const QTableView& view )
    {
        if( auto model = view.model() )
        {
            return dynamic_cast<QSqlTableModel*>( model );
        }
        else
        {
            return nullptr;
        }

    }

    void SetEnabledIfDirty( const QSqlTableModel& model, const QVector<QAbstractButton*>& buttons)
    {
        if( model.isDirty() )
        {
            for( auto btn : buttons )
                btn->setEnabled( true );
        }
        else
        {
            for( auto btn : buttons )
                btn->setEnabled( false );
        }
    }

    std::optional< QModelIndexList > GetSelectedRows( const QTableView& view )
    {
        if( view.selectionModel() )
        {
            const auto& selectedRows = view.selectionModel()->selectedRows();
            if( !selectedRows.empty() )
            {
                return { selectedRows };
            }
        }
        return {};
    }

    bool RemoveRows( const QTableView& view )
    {
        if( auto rows = GetSelectedRows( view ) )
        {
            if( auto model = Utility::GetSqlModel( view ) )
            {
                for( const auto& row : rows.value() )
                {
                    if( !model->removeRow( row.row() ) )
                        return false;
                }
            }
            else
                return false;
        }
        return true;
    }

}

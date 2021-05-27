#include "model/database.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

namespace PatientsDBManager
{


    Database::Database( const QString& fileName, QObject* parent ) noexcept
        : QObject( parent )
        , m_fileName( fileName )
    {
        m_db = QSqlDatabase::database( "QSQLITE" );
        if( !m_db.isValid() )
            m_db = QSqlDatabase::addDatabase( "QSQLITE" );
    }

    Database::~Database() noexcept
    {
        close();
    }

    Database::EConnectionResult Database::connect() noexcept
    {        
        if( m_fileName.isEmpty() )
        {
            qDebug() << "Database::connect: filename is cannot be empty";
            return EConnectionResult::INVALID_DB_FILENAME;
        }

        if( QFile::exists( m_fileName ) )
        {
            if( open( m_fileName ) )
            {
                const auto& tables = m_db.tables();

                if( !tables.contains( PHOTOS_SET_TABLE_NAME ) ||
                    !tables.contains( PATIENTS_TABLE_NAME ) )
                {
                    qDebug() << QString( "Database::open: required tables(%1 or %2) are missing in the database" )
                                .arg( PHOTOS_SET_TABLE_NAME )
                                .arg( PATIENTS_TABLE_NAME );
                    return  EConnectionResult::NO_TABLE;
                }

                return EConnectionResult::CONNECTED;
            }
            return EConnectionResult::OPENING_FAILED;
        }
        else
        {
            return restore( m_fileName ) ? EConnectionResult::CONNECTED :
                                           EConnectionResult::RESTORING_FAILED;
        }
    }

    QSqlTableModel *Database::createPatientsModel() const noexcept
    {
        if( auto patientsModel = new ( std::nothrow ) QSqlTableModel( nullptr, m_db ) )
        {
            patientsModel->setTable( PATIENTS_TABLE_NAME );
            patientsModel->setEditStrategy( QSqlTableModel::OnManualSubmit );

            patientsModel->setHeaderData( 0, Qt::Horizontal, "Id" );
            patientsModel->setHeaderData( 1, Qt::Horizontal, "Name" );
            patientsModel->setHeaderData( 2, Qt::Horizontal, "Address" );
            patientsModel->setHeaderData( 3, Qt::Horizontal, "Birth date" );
            patientsModel->setHeaderData( 4, Qt::Horizontal, "Admission date" );
            patientsModel->setHeaderData( 5, Qt::Horizontal, "Discarge date" );

            return patientsModel;
        }
        else
            return nullptr;
    }

    QSqlTableModel *Database::createPhotoSetsModel() const noexcept
    {
        if( auto photoSetsModel = new ( std::nothrow ) QSqlTableModel( nullptr, m_db ) )
        {
            photoSetsModel->setTable( PHOTOS_SET_TABLE_NAME );
            photoSetsModel->setEditStrategy( QSqlTableModel::OnManualSubmit );

            photoSetsModel->setHeaderData( 1, Qt::Horizontal, "Date" );
            photoSetsModel->setHeaderData( 2, Qt::Horizontal, "Photo name" );

            return photoSetsModel;
        }
        else
            return nullptr;
    }

    QString Database::getConnectionResult( EConnectionResult result ) noexcept
    {
        switch ( result )
        {
            case EConnectionResult::CONNECTED:
                return "Connection established";
            case EConnectionResult::INVALID_DB_FILENAME:
                return "Invalid database filename";
            case EConnectionResult::NO_TABLE:
                return QString( "Required tables:\n%1 or %2 are missing in the database" )
                        .arg( PHOTOS_SET_TABLE_NAME )
                        .arg( PATIENTS_TABLE_NAME );
            case EConnectionResult::OPENING_FAILED:
                return "Opening failed";
            case EConnectionResult::RESTORING_FAILED:
                return "Restoring failed";
            default:
                return "Unknown error";
        }
    }

    bool Database::open( const QString& databaseName ) noexcept
    {
        m_db.setDatabaseName( databaseName );
        if( m_db.open() )
        {
            QSqlQuery query( m_db );
            if( !query.exec( "PRAGMA foreign_keys = ON;" ) )
            {
                qDebug() << "Database::open: " + query.lastError().text();
                return false;
            }
            return true;
        }
        else
        {
            qDebug() << "Database::open: " + m_db.lastError().text();
            return false;
        }

        return true;
    }

    bool Database::restore( const QString& databaseName ) noexcept
    {
        if( open( databaseName ) )
        {
            if( !createPhotoSetsTable() || !createPatientsTable() )
                QFile::remove( m_fileName );
            else
                return true;
        }
        return false;
    }

    bool Database::createPatientsTable() noexcept
    {
        QSqlQuery query;
        query.prepare( "CREATE TABLE " + PATIENTS_TABLE_NAME + " ("
                       "'Id'	INTEGER NOT NULL UNIQUE,"
                       "'Name'	TEXT,"
                       "'Address' TEXT,"
                       "'BirthDate'	TEXT,"
                       "'AdmissionDate'	TEXT NOT NULL,"
                       "'DiscargeDate'	TEXT,"
                       "PRIMARY KEY( \"Id\" AUTOINCREMENT) );" );
//        query.bindValue( ":tablename", PATIENTS_TABLE_NAME );

        if( !query.exec() )
        {
            qDebug() << "DataBase::createPatientsTable: " + query.lastError().text();
            return false;
        }
        else
        {
            return true;
        }
    }

    bool Database::createPhotoSetsTable() noexcept
    {
        QSqlQuery query;
        query.prepare( "CREATE TABLE " + PHOTOS_SET_TABLE_NAME + " ("
                       "'Id'	INTEGER NOT NULL,"
                       "'Date'	TEXT NOT NULL,"
                       "'Filename'	TEXT NOT NULL,"
                       "'Photo'	BLOB NOT NULL,"
                       "'Patient_Id' INTEGER NOT NULL,"
                       "FOREIGN KEY(\"Patient_Id\") REFERENCES" +
                       PATIENTS_TABLE_NAME + "(\"Id\") ON DELETE CASCADE ON UPDATE CASCADE,"
                       "PRIMARY KEY(\"Id\") );" );

        //query.bindValue( ":tablename", PHOTOS_PACK_TABLE_NAME );

        if( !query.exec() )
        {
            qDebug() << "DataBase::createPhotoPackTable: " + query.lastError().text();
            return false;
        }
        else
        {
            return true;
        }
    }

    void Database::close() noexcept
    {
        if( m_db.isOpen() )
            m_db.close();
    }

}

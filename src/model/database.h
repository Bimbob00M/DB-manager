#ifndef DATABASE_H
#define DATABASE_H

#include <QDate>
#include <QDebug>
#include <QFile>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QObject>

namespace PatientsDBManager
{
    static const QString PATIENTS_TABLE_NAME = "Patients";
    static const QString PHOTOS_SET_TABLE_NAME = "PhotoSets";

    class Database : public QObject
    {
        Q_OBJECT
    public:
        enum class EConnectionResult : char { CONNECTED, NO_TABLE, OPENING_FAILED, RESTORING_FAILED, INVALID_DB_FILENAME };

        explicit Database( const QString& fileName, QObject* parent = nullptr ) noexcept;
        ~Database() noexcept;

        EConnectionResult connect() noexcept;
        bool isConnected() const noexcept { return m_db.isOpen(); }

        QSqlTableModel* createPatientsModel() const noexcept;
        QSqlTableModel* createPhotoSetsModel() const noexcept;

        QSqlDatabase& getConnection() noexcept { return m_db; }
        const QSqlDatabase& getConnection() const noexcept { return m_db; }

        static QString getConnectionResult( EConnectionResult result ) noexcept;

    private:
        QSqlDatabase m_db;
        QString      m_fileName;

        bool open( const QString &databaseName ) noexcept;
        bool openOrRestore( const QString &databaseName ) noexcept;
        bool restore( const QString& databaseName ) noexcept;
        bool createPatientsTable() noexcept;
        bool createPhotoSetsTable() noexcept;
        void close() noexcept;

    };

}

#endif // DATABASE_H

#ifndef UTILITY_H
#define UTILITY_H

#include <optional>
#include <utility>

#include <QAbstractButton>
#include <QByteArray>
#include <QFileDialog>
#include <QSqlTableModel>
#include <QTableView>
#include <QVector>

namespace PatientsDBManager::Utility
{
    QByteArray* LoadImage( const QString& fileName );

    void InitImageFileDialog( QFileDialog& dialog, QFileDialog::AcceptMode acceptMode, QFileDialog::FileMode fileMode );

}

#endif // UTILITY_H

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

    QSqlTableModel* GetSqlModel( const QTableView& view );

    void SetEnabledIfDirty( const QSqlTableModel& model, const QVector<QAbstractButton*>& buttons );

    std::optional< QModelIndexList > GetSelectedRows( const QTableView& view );

    bool RemoveRows( const QTableView& view );
}

#endif // UTILITY_H

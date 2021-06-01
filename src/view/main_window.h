#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QStackedWidget>
#include <QStringListModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>

#include "table_view_ex.h"
#include "model/database.h"
#include "model/data_types.h"
#include "patient_info_form.h"
#include "photo_viewer.h"

namespace PatientsDBManager
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        MainWindow( const QString& databasePath, QWidget *parent = nullptr );

    signals:
        void pageSwitched( int index );

    private:
        Database m_db;
        int64_t  m_currentPatientId{ 0 };

        QLabel*         m_patientInfoLbl{ nullptr };
        QStackedWidget* m_winPages{ nullptr };

        TableViewEx* m_patientsView{ nullptr };
        QTableView*  m_patientInfoView{ nullptr };
        TableViewEx* m_photoSetView{ nullptr };

        QPushButton* m_addPatientBtn{ nullptr };
        QPushButton* m_removePatientBtn{ nullptr };
        QPushButton* m_updatePatientBtn{ nullptr };

        QPushButton* m_addPhotoBtn{ nullptr };
        QPushButton* m_removePhotoBtn{ nullptr };
        QPushButton* m_updatePhotoBtn{ nullptr };

        QPushButton* m_updateInfoBtn{ nullptr };

        QPushButton* m_returnBtn{ nullptr };

        bool setupLayout() noexcept;
        bool setupControls() noexcept;

        bool setupPatientsView( QSqlTableModel* model ) noexcept;
        bool setupPatientInfoView( QSqlTableModel* model ) noexcept;
        bool setupPhotoSetView( QSqlTableModel* model ) noexcept;

        QTableView* getCurrentView() const noexcept;

        QWidget* createMainPage() const noexcept;
        QWidget* createPatientPage() const noexcept;

        bool update( QTableView* view ) noexcept;
        bool remove( QTableView* view ) noexcept;

    private slots:
        void switchPage( int index ) noexcept;

        void returnToMainPage() noexcept;
        void showPatientPage();
        void openPhotos();

        void updatePatients() noexcept;
        void addPatient() noexcept;
        void removePatients() noexcept;

        void updatePatientInfo() noexcept;

        void updatePhotoSet() noexcept;
        void addPhotos() noexcept;
        void removePhotos() noexcept;
    };

}
#endif // MAINWINDOW_H

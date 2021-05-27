#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QStackedWidget>
#include <QStringListModel>
#include <QSqlTableModel>
#include <QLinkedList>
#include <QTableView>
#include <QPushButton>

#include "sql_table_view.h"
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
        ~MainWindow();

    signals:
        void pageSwitched( int index );

        void patientsChanged();
        void patientInfoChanged();
        void photoSetsChanged();

        void viewTableRightDblClicked();

    private:
        QLinkedList< PhotoViewer* > m_photoViewers;
        Database m_db;
        int64_t  m_currentPatientId{ 0 };

        QLabel*         m_patientInfoLbl{ nullptr };
        QStackedWidget* m_winPages{ nullptr };

        SqlTableView* m_patientsView{ nullptr };
        QTableView* m_patientInfoView{ nullptr };
        SqlTableView* m_photoSetsView{ nullptr };

        QPushButton* m_addPatientBtn{ nullptr };
        QPushButton* m_removePatientBtn{ nullptr };
        QPushButton* m_updatePatientBtn{ nullptr };
        QPushButton* m_submitPatientBtn{ nullptr };
        QPushButton* m_revertPatientBtn{ nullptr };

        QPushButton* m_addPhotoBtn{ nullptr };
        QPushButton* m_removePhotoBtn{ nullptr };
        QPushButton* m_updatePhotoBtn{ nullptr };
        QPushButton* m_submitPhotoBtn{ nullptr };
        QPushButton* m_revertPhotoBtn{ nullptr };        

        QPushButton* m_updateInfoBtn{ nullptr };
        QPushButton* m_submitInfoBtn{ nullptr };
        QPushButton* m_revertInfoBtn{ nullptr };

        QPushButton* m_returnBtn{ nullptr };

        bool setupLayout() noexcept;
        bool setupControls() noexcept;

        bool setupPatientsView( QSqlTableModel* model ) noexcept;
        bool setupPatientInfoView( QSqlTableModel* model ) noexcept;
        bool setupPhotoSetsView( QSqlTableModel* model ) noexcept;

        QTableView* getCurrentView() const noexcept;

        QWidget* createMainPage() const noexcept;
        QWidget* createPatientPage() const noexcept;

        bool update( QTableView* view ) noexcept;
        bool remove( QTableView* view ) noexcept;

        bool submit( QTableView* view ) noexcept;
        bool revert( QTableView* view ) noexcept;

    private slots:
        void returnOnMainPage() noexcept;
        void switchPage( int index ) noexcept;

        void showPatientPage();
        void openPhoto();

        void updatePatients() noexcept;
        void addPatient() noexcept;
        void removePatients() noexcept;

        void updatePatientInfo() noexcept;

        void updatePhotoSets() noexcept;
        void addPhoto() noexcept;
        void removePhotoSets() noexcept;

        void submitPatients() noexcept;
        void revertPatients() noexcept;

        void submitPatientInfo() noexcept;
        void revertPatientInfo() noexcept;

        void submitPhotoSets() noexcept;
        void revertPhotoSets() noexcept;

        void checkPatients() noexcept;
        void checkPatientInfo() noexcept;
        void checkPhotoSets() noexcept;

        void destroyPhotoViewer( PhotoViewer* ) noexcept;
    };

}
#endif // MAINWINDOW_H

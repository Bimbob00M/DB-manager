#include "main_window.h"

#include <QDateTime>
#include <QFileInfo>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSqlRecord>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

#include "add_patient_dlg.h"
#include "photo_viewer.h"
#include "model/horizontal_proxy_model.h"
#include "model/delegates.h"
#include "utility/global.h"
#include "utility/utility.h"

namespace PatientsDBManager
{

    MainWindow::MainWindow( const QString& databasePath, QWidget *parent )
        : QMainWindow( parent )
        , m_db( databasePath )
    {
        bool initFailed = false;

        QSqlTableModel* patientsModel = nullptr;
        QSqlTableModel* photoSetsModel = nullptr;

        auto connectionResult = Database::EConnectionResult::CONNECTED;
        if( !initFailed &&
            ( connectionResult = m_db.connect() ) == Database::EConnectionResult::CONNECTED )
        {
            patientsModel = m_db.createPatientsModel( this );
            photoSetsModel = m_db.createPhotoSetModel( this );

            if( patientsModel &&
                photoSetsModel )
            {
                patientsModel->setEditStrategy( QSqlTableModel::OnFieldChange );
                photoSetsModel->setEditStrategy( QSqlTableModel::OnFieldChange );

                patientsModel->select();
                photoSetsModel->select();

                if( !setupPatientsView( patientsModel ) ||
                   !setupPatientInfoView( patientsModel ) ||
                   !setupPhotoSetView( photoSetsModel ) ||
                   !setupControls() ||
                   !setupLayout() )
                {
                   initFailed = true;
                }
            }
            else
                initFailed = true;
        }
        else
            initFailed = true;

        if( initFailed )
        {
            delete patientsModel;
            delete photoSetsModel;

            QString errorMsg;
            if( connectionResult != Database::EConnectionResult::CONNECTED )
                errorMsg = Database::getConnectionResult( connectionResult );
            else
                errorMsg = "Initialization failed";

            QMessageBox::critical( this,
                                   "Error",
                                   errorMsg,
                                   QMessageBox::Ok );

            exit( 0 );
        }
    }

    void MainWindow::switchPage( int index ) noexcept
    {
        if( m_winPages )
        {
           if( m_winPages->count() && index >= 0 && index < m_winPages->count() )
           {
               m_winPages->setCurrentIndex( index );
               emit pageSwitched( index );
           }
        }
    }

    bool MainWindow::setupLayout() noexcept
    {
        delete m_winPages;

        if( m_winPages = new ( std::nothrow ) QStackedWidget( this ); !m_winPages )
            return false;

        auto mainPage = createMainPage();
        auto patientPage = createPatientPage();

        if( !mainPage || !patientPage )
        {
            delete mainPage;
            delete patientPage;
            return false;
        }

        m_winPages->addWidget( mainPage );
        m_winPages->addWidget( patientPage );
        setCentralWidget( m_winPages );

        return true;
    }

    bool MainWindow::setupControls() noexcept
    {
        auto deleteAllControls = [&]
        {
            delete m_updatePatientBtn;
            delete m_addPatientBtn;
            delete m_removePatientBtn;

            delete m_updatePhotoBtn;
            delete m_addPhotoBtn;
            delete m_removePhotoBtn;

            delete m_updateInfoBtn;

            delete m_returnBtn;
        };

        deleteAllControls();

        m_patientInfoLbl = new ( std::nothrow ) QLabel( this );

        m_updatePatientBtn = new ( std::nothrow ) QPushButton( "Update", this );
        m_addPatientBtn =    new ( std::nothrow ) QPushButton( "Add", this );
        m_removePatientBtn = new ( std::nothrow ) QPushButton( "Remove", this );

        m_updatePhotoBtn = new ( std::nothrow ) QPushButton( "Update", this );
        m_addPhotoBtn =    new ( std::nothrow ) QPushButton( "Add", this );
        m_removePhotoBtn = new ( std::nothrow ) QPushButton( "Remove", this );

        m_updateInfoBtn = new ( std::nothrow ) QPushButton( "Update", this );

        m_returnBtn = new ( std::nothrow ) QPushButton( "←", this );

        if( !m_patientInfoLbl ||
            !m_updatePatientBtn ||
            !m_updatePhotoBtn ||                
            !m_updateInfoBtn ||
            !m_addPatientBtn ||
            !m_addPhotoBtn ||
            !m_removePatientBtn ||
            !m_removePhotoBtn ||
            !m_returnBtn )
        {
            deleteAllControls();

            return false;
        }

        auto font = m_patientInfoLbl->font();
        font.setPointSize( 14 );
        m_patientInfoLbl->setFont( font );

        m_returnBtn->setMaximumWidth( 35 );
        m_returnBtn->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        m_returnBtn->setFlat( true );
        m_returnBtn->setStyleSheet( "QPushButton:hover:!pressed{ border: 1px solid grey; }" );

        connect( m_updatePatientBtn, &QPushButton::clicked, this, &MainWindow::updatePatients );
        connect( m_addPatientBtn, &QPushButton::clicked, this, &MainWindow::addPatient );
        connect( m_removePatientBtn, &QPushButton::clicked, this, &MainWindow::removePatients );

        connect( m_updatePhotoBtn, &QPushButton::clicked, this, &MainWindow::updatePhotoSet );
        connect( m_addPhotoBtn, &QPushButton::clicked, this, &MainWindow::addPhotos );
        connect( m_removePhotoBtn, &QPushButton::clicked, this, &MainWindow::removePhotos );

        connect( m_updateInfoBtn, &QPushButton::clicked, this, &MainWindow::updatePatientInfo );

        connect( m_returnBtn, &QPushButton::clicked, this, &MainWindow::returnToMainPage );

        return true;
    }

    bool MainWindow::setupPatientsView( QSqlTableModel* model ) noexcept
    {
        if( m_patientsView )
            delete m_patientsView;

        if( m_patientsView = new ( std::nothrow ) TableViewEx( this ); !m_patientsView )
            return false;

        m_patientsView->setModel( model );

        m_patientsView->hideColumn( 0 ); // don't show the ID
        m_patientsView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_patientsView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_patientsView->setSelectionMode( QAbstractItemView::ExtendedSelection );
        m_patientsView->horizontalHeader()->setStretchLastSection( true );

        {
            auto nameDelagate = new RegexItemDelegate( Global::NOT_EMPTY_REGEX_PATTERN, this );
            nameDelagate->setToolTip( "'Name' can't be empty" );
            m_patientsView->setItemDelegateForColumn( 1, nameDelagate );
        }

        {
            auto addressDelagate = new RegexItemDelegate( Global::NOT_EMPTY_REGEX_PATTERN, this );
            addressDelagate->setToolTip( "'Address' can't be empty" );
            m_patientsView->setItemDelegateForColumn( 2, addressDelagate );
        }

        m_patientsView->setItemDelegateForColumn( 3, new DateItemDelegate( this ) );

        {
            auto admissionDateDelegate = new DateItemDelegate( this );
            admissionDateDelegate->connectMinimalDateToColumn( 5 );
            admissionDateDelegate->setMinimalDate( true );
            m_patientsView->setItemDelegateForColumn( 4, admissionDateDelegate );
        }

        {
            auto discargeDateDelegate = new DateItemDelegate( this );
            discargeDateDelegate->setNullable( true );
            m_patientsView->setItemDelegateForColumn( 5, discargeDateDelegate );
        }

        connect( m_patientsView, &TableViewEx::activated, this, &MainWindow::showPatientPage );
        connect( m_patientsView, &TableViewEx::rightDoubleClicked, this, &MainWindow::showPatientPage );

        return true;
    }

    bool MainWindow::setupPatientInfoView( QSqlTableModel* model ) noexcept
    {
        if( m_patientInfoView )
            delete m_patientInfoView;

        if( m_patientInfoView = new ( std::nothrow ) QTableView( this ); !m_patientInfoView )
            return false;

        model->setEditStrategy( QSqlTableModel::OnFieldChange );
        model->select();

        m_patientInfoView->horizontalHeader()->hide();

        auto proxyModel = new HorizontalProxyModel;
        proxyModel->setSourceModel( model );
        for( int columnIndex = 0; columnIndex < proxyModel->sourceModel()->columnCount(); ++columnIndex )
        {
            const auto& cellIndex = proxyModel->index( 0, columnIndex );
            proxyModel->setFlags( cellIndex, Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        }
        m_patientInfoView->setModel( proxyModel );

        m_patientInfoView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_patientInfoView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_patientInfoView->setSelectionMode( QAbstractItemView::SingleSelection );
        m_patientInfoView->horizontalHeader()->setStretchLastSection( true );

        {
            auto nameDelagate = new RegexItemDelegate( Global::NOT_EMPTY_REGEX_PATTERN, this );
            nameDelagate->setToolTip( "'Name' can't be empty" );
            m_patientInfoView->setItemDelegateForRow( 1, nameDelagate );
        }

        {
            auto addressDelagate = new RegexItemDelegate( Global::NOT_EMPTY_REGEX_PATTERN, this );
            addressDelagate->setToolTip( "'Address' can't be empty" );
            m_patientInfoView->setItemDelegateForRow( 2, addressDelagate );
        }

        m_patientInfoView->setItemDelegateForRow( 3, new DateItemDelegate( this ) );

        {
            auto admissionDateDelegate = new DateItemDelegate( this );
            admissionDateDelegate->connectMinimalDateToRow( 5 );
            admissionDateDelegate->setMinimalDate( true );
            m_patientInfoView->setItemDelegateForRow( 4, admissionDateDelegate );
        }

        {
            auto discargeDateDelegate = new DateItemDelegate( this );
            discargeDateDelegate->setNullable( true );
            m_patientInfoView->setItemDelegateForRow( 5, discargeDateDelegate );
        }

        return true;
    }

    bool MainWindow::setupPhotoSetView( QSqlTableModel* model ) noexcept
    {
        if( m_photoSetView )
            delete m_photoSetView;

        if( m_photoSetView = new ( std::nothrow ) TableViewEx( this ); !m_photoSetView )
            return false;

        m_photoSetView->setModel( model );
        m_photoSetView->hideColumn( 0 ); // don't show the ID
        m_photoSetView->hideColumn( 3 ); // don't show the binary data
        m_photoSetView->hideColumn( 4 ); // don't show the Patient_Id
        m_photoSetView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_photoSetView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_photoSetView->setSelectionMode( QAbstractItemView::ExtendedSelection );
        m_photoSetView->resizeColumnsToContents();
        m_photoSetView->horizontalHeader()->setStretchLastSection( true );

        m_photoSetView->setItemDelegateForColumn( 1, new DateTimeItemDelegate( this ) );

        {
            auto photoNameDelegate = new RegexItemDelegate( Global::NOT_EMPTY_REGEX_PATTERN, this );
            photoNameDelegate->setToolTip( "'Photo name' cannot be empty" );
            m_photoSetView->setItemDelegateForColumn( 2, photoNameDelegate );
        }

        connect( m_photoSetView, &TableViewEx::rightDoubleClicked, this, &MainWindow::openPhotos );

        return true;
    }

    QTableView* MainWindow::getCurrentView() const noexcept
    {
        if( !m_winPages )
            return nullptr;

        if( m_winPages->currentIndex() == 0 )
            return m_patientsView;
        else
            return m_photoSetView;

    }

    QWidget* MainWindow::createMainPage() const noexcept
    {                
        auto tableCommandPanelLayout = new ( std::nothrow ) QGridLayout;
        auto pageLayout =              new ( std::nothrow ) QVBoxLayout;
        auto mainPage =                new ( std::nothrow ) QWidget;

        if( !tableCommandPanelLayout ||
            !pageLayout ||
            !mainPage )
        {
            delete tableCommandPanelLayout;
            delete pageLayout;
            delete mainPage;
            return nullptr;
        }

        if( auto patientsLbl = new ( std::nothrow ) QLabel( "Patients" ) )
        {
            auto font = patientsLbl->font();
            font.setPointSize( 20 );
            patientsLbl->setFont( font );
            tableCommandPanelLayout->addWidget( patientsLbl, 0, 0, 0, 1, Qt::AlignHCenter );
        }

        auto horizontalSpacer = new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed );
        tableCommandPanelLayout->addItem( horizontalSpacer, 0, 1, 0, 1 );
        auto verticalSpacer = new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Minimum );
        tableCommandPanelLayout->addItem( verticalSpacer, 0, 2, 0, 2 );

        tableCommandPanelLayout->addWidget( m_updatePatientBtn, 1, 2, Qt::AlignCenter );
        tableCommandPanelLayout->addWidget( m_addPatientBtn, 1, 3, Qt::AlignCenter );
        tableCommandPanelLayout->addWidget( m_removePatientBtn, 1, 4, Qt::AlignCenter );

        pageLayout->addLayout( tableCommandPanelLayout );
        pageLayout->addWidget( m_patientsView );

        mainPage->setLayout( pageLayout );

        return mainPage;
    }

    QWidget* MainWindow::createPatientPage() const noexcept
    {
        auto infoLayout =               new ( std::nothrow ) QVBoxLayout;
        auto infoLabelUpdateLayout =    new ( std::nothrow ) QHBoxLayout;
        auto updateAddRemoveLayout =    new ( std::nothrow ) QHBoxLayout;
        auto photosLayout =             new ( std::nothrow ) QVBoxLayout;
        auto pageLayout =               new ( std::nothrow ) QHBoxLayout;
        auto pageMainLayout =           new ( std::nothrow ) QHBoxLayout;
        auto patientPage =              new ( std::nothrow ) QWidget;

        if( !infoLayout ||
            !infoLabelUpdateLayout ||
            !updateAddRemoveLayout ||
            !photosLayout ||
            !pageLayout ||
            !pageMainLayout ||
            !patientPage )
        {
            delete infoLayout;
            delete updateAddRemoveLayout;
            delete photosLayout;
            delete pageLayout;
            delete pageMainLayout;
            delete patientPage;
            return  nullptr;
        }

        updateAddRemoveLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        updateAddRemoveLayout->addWidget( m_updatePhotoBtn );
        updateAddRemoveLayout->addWidget( m_addPhotoBtn );
        updateAddRemoveLayout->addWidget( m_removePhotoBtn );

        photosLayout->addLayout( updateAddRemoveLayout );
        photosLayout->addWidget( m_photoSetView );

        infoLabelUpdateLayout->addWidget( m_patientInfoLbl );
        infoLabelUpdateLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        infoLabelUpdateLayout->addWidget( m_updateInfoBtn );

        infoLayout->addLayout( infoLabelUpdateLayout );
        infoLayout->addWidget( m_patientInfoView );

        pageLayout->addLayout( infoLayout );
        pageLayout->addLayout( photosLayout );
        pageLayout->setContentsMargins( 0, 20, 10, 10 );

        pageMainLayout->addWidget( m_returnBtn );
        pageMainLayout->addLayout( pageLayout );
        pageMainLayout->setContentsMargins( 0, 0, 0, 0 );

        patientPage->setLayout( pageMainLayout );

        return patientPage;
    }

    bool MainWindow::update( QTableView* view ) noexcept
    {
        QString errorMsg;
        if( view )
        {
            if( auto model = dynamic_cast<QSqlTableModel*>( view->model() ) )
            {
                if( !model->select() )
                    errorMsg = model->lastError().text();
                else
                    return true;
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Update error",
                              errorMsg,
                              QMessageBox::Ok );

        return false;
    }

    bool MainWindow::remove( QTableView* view ) noexcept
    {
        if( view )
        {
            auto model = dynamic_cast<QSqlTableModel*>( view->model() );
            if( model && view->selectionModel() )
            {
                const auto& selectedRows = view->selectionModel()->selectedRows();
                for( const auto& row : selectedRows )
                {
                    model->removeRow( row.row() );
                }
                model->select();
                return true;
            }
        }
        QMessageBox::warning( this,
                              "Remove error",
                              "Nullptr error",
                              QMessageBox::Ok );
        return false;
    }


    void MainWindow::updatePatients() noexcept
    {
        update( m_patientsView );
    }

    void MainWindow::addPatient() noexcept
    {
        QString errorMsg;
        if( m_patientsView )
        {
            if( auto model = dynamic_cast<QSqlTableModel*>( m_patientsView->model() ) )
            {
                AddPatientDlg dialog( this );
                if( dialog.exec() == QDialog::Accepted )
                {
                    const auto& p = dialog.getPatient();

                    QSqlRecord newPatientRecord = model->record();
                    newPatientRecord.setValue( "Name", p.get()->name );
                    newPatientRecord.setValue( "Address", p.get()->address );
                    newPatientRecord.setValue( "BirthDate", p.get()->birthDate );
                    newPatientRecord.setValue( "AdmissionDate", p.get()->admissionDate );

                    const auto& discargeDate = p.get()->discargeDate.isEmpty()
                                                            ? Global::EMPTY_CELL_DEFAULT_VALUE
                                                            : p.get()->discargeDate;
                    newPatientRecord.setValue( "DiscargeDate", discargeDate );

                    if( !model->insertRecord( -1, newPatientRecord ) )
                        errorMsg = model->lastError().text();
                    else
                    {
                        model->select();
                        return;
                    }
                }
                else
                    return;
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Add patient error",
                              errorMsg,
                              QMessageBox::Ok );

    }

    void MainWindow::removePatients() noexcept
    {
        remove( m_patientsView );
    }

    void MainWindow::updatePatientInfo() noexcept
    {
        update( m_patientsView );
    }

    void MainWindow::updatePhotoSet() noexcept
    {
        update( m_photoSetView );
    }

    void MainWindow::addPhotos() noexcept
    {
        QString errorMsg;
        int failCount = 0;
        if( m_photoSetView )
        {
            if( auto model = dynamic_cast<QSqlTableModel*>( m_photoSetView->model() ) )
            {
                QFileDialog dialog( this, "Open File" );
                Utility::InitImageFileDialog( dialog, QFileDialog::AcceptOpen, QFileDialog::ExistingFiles );
                if( dialog.exec() == QDialog::Accepted )
                {
                    for( const auto& filePath : dialog.selectedFiles() )
                    {
                        if( auto binaryImage = Utility::LoadImage( filePath ) )
                        {
                            QSqlRecord newRecord = model->record();
                            QFileInfo fileInfo( filePath );
                            auto fileTime = fileInfo.fileTime( QFileDevice::FileBirthTime );
                            if( !fileTime.isValid() )
                                fileTime = QDateTime::currentDateTime();
                            newRecord.setValue( "Date", fileTime.toString( Global::DATE_TIME_FORMAT ) );
                            newRecord.setValue( "Filename", fileInfo.baseName() );
                            newRecord.setValue( "Photo", *binaryImage );
                            newRecord.setValue( "Patient_Id", m_currentPatientId );

                            delete binaryImage;

                            if( !model->insertRecord( -1, newRecord ) )
                                ++failCount;                            
                            else
                                continue;
                        }
                    }
                    model->select();
                    if( failCount == 0 )
                        return;
                }
                else
                    return;
            }
        }

        if( failCount > 0 )
            errorMsg = QString( "%1 photos were not added" ).arg( failCount );
        else
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Add photos error",
                              errorMsg,
                              QMessageBox::Ok );
    }

    void MainWindow::removePhotos() noexcept
    {
        remove( m_photoSetView );
    }

    void MainWindow::returnToMainPage() noexcept
    {
        if( auto model = dynamic_cast<QSqlTableModel*>( m_patientsView->model() ) )
        {
            model->setFilter( "" );
        }
        switchPage( 0 );
    }

    void MainWindow::showPatientPage()
    {
        if( auto model = dynamic_cast<QSqlTableModel*>( m_patientsView->model() ) )
        {
            const auto row = m_patientsView->selectionModel()->currentIndex().row();

            m_currentPatientId = model->index( row, 0 ).data().toLongLong();

            m_patientInfoLbl->setText( QString( "Patient #%1" ).arg( m_currentPatientId ) );

            if( auto photoSetsModel = dynamic_cast<QSqlTableModel*>( m_photoSetView->model() ) )
                photoSetsModel->setFilter( QString( "Patient_Id=%1" ).arg( m_currentPatientId ) );

            model->setFilter( QString( "Id=%1" ).arg( m_currentPatientId ) );

            switchPage( 1 );
        }
    }

    void MainWindow::openPhotos()
    {
        if( auto model = dynamic_cast<QSqlTableModel*>( m_photoSetView->model() ) )
        {
            for( auto modelIndex : m_photoSetView->selectionModel()->selectedRows() )
            {
                const auto& title = model->index( modelIndex.row(), 2 ).data().toString();
                const auto& binaryImage = model->index( modelIndex.row(), 3 ).data().toByteArray();

                // PhotoViewer will free up memory
                ( new PhotoViewer( title, binaryImage ) )->show();
            }
        }
    }
}

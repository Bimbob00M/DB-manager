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
        QSqlTableModel* patientInfoModel = nullptr;
        QSqlTableModel* photoSetsModel = nullptr;

        auto connectionResult = Database::EConnectionResult::OPENING_FAILED;
        if( !initFailed &&
            ( connectionResult = m_db.connect() ) == Database::EConnectionResult::CONNECTED )
        {
            patientsModel = m_db.createPatientsModel();
            patientInfoModel = m_db.createPatientsModel();
            photoSetsModel = m_db.createPhotoSetsModel();

            if( patientsModel &&
                patientInfoModel &&
                photoSetsModel )
            {
               if( !setupPatientsView( patientsModel ) ||
                   !setupPatientInfoView( patientInfoModel ) ||
                   !setupPhotoSetsView( photoSetsModel ) ||
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
            delete patientInfoModel;
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

    MainWindow::~MainWindow()
    {
        for( auto viewer : m_photoViewers )
            delete viewer;
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
            delete m_submitPatientBtn;
            delete m_revertPatientBtn;

            delete m_updatePhotoBtn;
            delete m_addPhotoBtn;
            delete m_removePhotoBtn;
            delete m_submitPhotoBtn;
            delete m_revertPhotoBtn;

            delete m_updateInfoBtn;
            delete m_submitInfoBtn;
            delete m_revertInfoBtn;

            delete m_returnBtn;
        };

        deleteAllControls();

        m_patientInfoLbl = new ( std::nothrow ) QLabel( this );

        m_updatePatientBtn = new ( std::nothrow ) QPushButton( "Update", this );
        m_addPatientBtn =    new ( std::nothrow ) QPushButton( "Add", this );
        m_removePatientBtn = new ( std::nothrow ) QPushButton( "Remove", this );
        m_submitPatientBtn = new ( std::nothrow ) QPushButton( "Submit", this );
        m_revertPatientBtn = new ( std::nothrow ) QPushButton( "Revert", this );

        m_updatePhotoBtn = new ( std::nothrow ) QPushButton( "Update", this );
        m_addPhotoBtn =    new ( std::nothrow ) QPushButton( "Add", this );
        m_removePhotoBtn = new ( std::nothrow ) QPushButton( "Remove", this );
        m_submitPhotoBtn = new ( std::nothrow ) QPushButton( "Submit", this );
        m_revertPhotoBtn = new ( std::nothrow ) QPushButton( "Revert", this );        

        m_updateInfoBtn = new ( std::nothrow ) QPushButton( "Update", this );
        m_submitInfoBtn = new ( std::nothrow ) QPushButton( "Submit", this );
        m_revertInfoBtn = new ( std::nothrow ) QPushButton( "Revert", this );

        m_returnBtn = new ( std::nothrow ) QPushButton( "←", this );

        if( !m_patientInfoLbl ||
            !m_updatePatientBtn ||
            !m_updatePhotoBtn ||                
            !m_updateInfoBtn ||
            !m_addPatientBtn ||
            !m_addPhotoBtn ||
            !m_removePatientBtn ||
            !m_removePhotoBtn ||
            !m_submitPatientBtn ||
            !m_submitPhotoBtn ||
            !m_submitInfoBtn ||
            !m_revertPatientBtn ||
            !m_revertPhotoBtn ||
            !m_revertInfoBtn ||
            !m_returnBtn )
        {
            deleteAllControls();

            return false;
        }

        auto font = m_patientInfoLbl->font();
        font.setPointSize( 14 );
        m_patientInfoLbl->setFont( font );

        m_submitPatientBtn->setEnabled( false );
        m_revertPatientBtn->setEnabled( false );

        m_submitPhotoBtn->setEnabled( false );
        m_revertPhotoBtn->setEnabled( false );

        m_submitInfoBtn->setEnabled( false );
        m_revertInfoBtn->setEnabled( false );

        m_returnBtn->setMaximumWidth( 35 );
        m_returnBtn->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        m_returnBtn->setFlat( true );
        m_returnBtn->setStyleSheet( "QPushButton:hover:!pressed{border: 1px solid grey;}" );

        connect( m_updatePatientBtn, &QPushButton::clicked, this, &MainWindow::updatePatients );
        connect( m_addPatientBtn, &QPushButton::clicked, this, &MainWindow::addPatient );
        connect( m_removePatientBtn, &QPushButton::clicked, this, &MainWindow::removePatients );
        connect( m_submitPatientBtn, &QPushButton::clicked, this, &MainWindow::submitPatients );
        connect( m_revertPatientBtn, &QPushButton::clicked, this, &MainWindow::revertPatients );

        connect( m_updatePhotoBtn, &QPushButton::clicked, this, &MainWindow::updatePhotoSets );
        connect( m_addPhotoBtn, &QPushButton::clicked, this, &MainWindow::addPhoto );
        connect( m_removePhotoBtn, &QPushButton::clicked, this, &MainWindow::removePhotoSets );
        connect( m_submitPhotoBtn, &QPushButton::clicked, this, &MainWindow::submitPhotoSets );
        connect( m_revertPhotoBtn, &QPushButton::clicked, this, &MainWindow::revertPhotoSets );

        connect( m_updateInfoBtn, &QPushButton::clicked, this, &MainWindow::updatePatientInfo );
        connect( m_submitInfoBtn, &QPushButton::clicked, this, &MainWindow::submitPatientInfo );
        connect( m_revertInfoBtn, &QPushButton::clicked, this, &MainWindow::revertPatientInfo );

        connect( m_returnBtn, &QPushButton::clicked, this, &MainWindow::returnOnMainPage );

        return true;
    }

    bool MainWindow::setupPatientsView( QSqlTableModel* model ) noexcept
    {
        if( model )
            model->setParent( this );

        if( m_patientsView )
            delete m_patientsView;

        if( m_patientsView = new ( std::nothrow ) SqlTableView( this ); !m_patientsView )
            return false;

        model->setEditStrategy( QSqlTableModel::OnManualSubmit );
        model->select();

        m_patientsView->setModel( model );

        m_patientsView->hideColumn( 0 ); // don't show the ID
        m_patientsView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_patientsView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_patientsView->setSelectionMode( QAbstractItemView::ExtendedSelection );
        m_patientsView->horizontalHeader()->setStretchLastSection( true );

        m_patientsView->setItemDelegateForColumn( 1, new NotNullItemDelegate( "\"Name\" can't be empty", this ) );
        m_patientsView->setItemDelegateForColumn( 2, new NotNullItemDelegate( "\"Address\" can't be empty", this ) );
        m_patientsView->setItemDelegateForColumn( 3, new DateItemDelegate( this ) );

        auto admissionDateDelegate = new DateItemDelegate( this );
        admissionDateDelegate->connectMinimalDateToColumn( 5 );
        admissionDateDelegate->setMinimalDate( true );
        m_patientsView->setItemDelegateForColumn( 4, admissionDateDelegate );

        auto discargeDateDelegate = new DateItemDelegate( this );
        discargeDateDelegate->setMinimalDate( true );
        m_patientsView->setItemDelegateForColumn( 5, discargeDateDelegate );

        connect( m_patientsView, &SqlTableView::activated, this, &MainWindow::showPatientPage );
        connect( m_patientsView, &SqlTableView::rightDoubleClicked, this, &MainWindow::showPatientPage );

        connect( this, &MainWindow::patientsChanged, this, &MainWindow::checkPatients );
        connect( model, &QSqlTableModel::dataChanged, this, &MainWindow::checkPatients );

        return true;
    }

    bool MainWindow::setupPatientInfoView( QSqlTableModel* model ) noexcept
    {
        if( model )
            model->setParent( this );

        if( m_patientInfoView )
            delete m_patientInfoView;

        if( m_patientInfoView = new ( std::nothrow ) QTableView( this ); !m_patientInfoView )
            return false;

        model->setEditStrategy( QSqlTableModel::OnManualSubmit );
        model->select();

        m_patientInfoView->horizontalHeader()->hide();

        auto proxyModel = new Model::HorizontalProxyModel;
        proxyModel->setSourceModel( model );
        m_patientInfoView->setModel( proxyModel );

        m_patientInfoView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_patientInfoView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_patientInfoView->setSelectionMode( QAbstractItemView::SingleSelection );
        m_patientInfoView->horizontalHeader()->setStretchLastSection( true );

        m_patientInfoView->setItemDelegateForRow( 0, new NotModifiableItemDelegate( this ) );
        m_patientInfoView->setItemDelegateForRow( 1, new NotNullItemDelegate( "\"Name\" cannot be empty", this ) );
        m_patientInfoView->setItemDelegateForRow( 2, new NotNullItemDelegate( "\"Address\" cannot be empty", this ) );
        m_patientInfoView->setItemDelegateForRow( 3, new DateItemDelegate( this ) );

        auto admissionDateDelegate = new DateItemDelegate( this );
        admissionDateDelegate->connectMinimalDateToRow( 5 );
        admissionDateDelegate->setMinimalDate( true );
        m_patientInfoView->setItemDelegateForRow( 4, admissionDateDelegate );

        auto discargeDateDelegate = new DateItemDelegate( this );
        discargeDateDelegate->setMinimalDate( true );
        m_patientInfoView->setItemDelegateForRow( 5, discargeDateDelegate );

        connect( this, &MainWindow::patientInfoChanged, this, &MainWindow::checkPatientInfo );
        connect( model, &QSqlTableModel::dataChanged, this, &MainWindow::checkPatientInfo );

        return true;
    }

    bool MainWindow::setupPhotoSetsView( QSqlTableModel* model ) noexcept
    {
        if( model )
            model->setParent( this );

        if( m_photoSetsView )
            delete m_photoSetsView;

        if( m_photoSetsView = new ( std::nothrow ) SqlTableView( this ); !m_photoSetsView )
            return false;

        model->setEditStrategy( QSqlTableModel::OnManualSubmit );
        model->setHeaderData( 1, Qt::Horizontal, "Date" );
        model->setHeaderData( 2, Qt::Horizontal, "Photo name" );
        model->select();

        m_photoSetsView->setModel( model );
        m_photoSetsView->hideColumn( 0 ); // don't show the ID
        m_photoSetsView->hideColumn( 3 ); // don't show the binary data
        m_photoSetsView->hideColumn( 4 ); // don't show the Patient_Id
        m_photoSetsView->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
        m_photoSetsView->setSelectionBehavior( QAbstractItemView::SelectRows );
        m_photoSetsView->setSelectionMode( QAbstractItemView::ExtendedSelection );
        m_photoSetsView->horizontalHeader()->setStretchLastSection( true );

        m_photoSetsView->setItemDelegateForColumn( 1, new NotModifiableItemDelegate( this ) );
        m_photoSetsView->setItemDelegateForColumn( 2, new NotNullItemDelegate( "\"Photo name\" cannot be empty", this ) );

        connect( m_photoSetsView, &QTableView::activated, this, &MainWindow::openPhoto );
        connect( m_photoSetsView, &SqlTableView::rightDoubleClicked, this, &MainWindow::openPhoto );

        connect( this, &MainWindow::photoSetsChanged, this, &MainWindow::checkPhotoSets );
        connect( model, &QSqlTableModel::dataChanged, this, &MainWindow::checkPhotoSets );

        return true;
    }

    QTableView* MainWindow::getCurrentView() const noexcept
    {
        if( !m_winPages )
            return nullptr;

        if( m_winPages->currentIndex() == 0 )
            return m_patientsView;
        else
            return m_photoSetsView;

    }

    QWidget* MainWindow::createMainPage() const noexcept
    {                
        auto tableCommandPanelLayout = new ( std::nothrow ) QGridLayout;
        auto submitRevertLayout =      new ( std::nothrow ) QHBoxLayout;
        auto pageLayout =              new ( std::nothrow ) QVBoxLayout;
        auto mainPage =                new ( std::nothrow ) QWidget;

        if( !tableCommandPanelLayout ||
            !submitRevertLayout ||
            !pageLayout ||
            !mainPage )
        {
            delete tableCommandPanelLayout;
            delete submitRevertLayout;
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

        submitRevertLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        submitRevertLayout->addWidget( m_submitPatientBtn );
        submitRevertLayout->addWidget( m_revertPatientBtn );

        pageLayout->addLayout( tableCommandPanelLayout );
        pageLayout->addWidget( m_patientsView );
        pageLayout->addLayout( submitRevertLayout );

        mainPage->setLayout( pageLayout );

        return mainPage;
    }

    QWidget* MainWindow::createPatientPage() const noexcept
    {
        auto infoLayout =               new ( std::nothrow ) QVBoxLayout;
        auto infoLabelUpdateLayout =    new ( std::nothrow ) QHBoxLayout;
        auto infoSubmitRevertLayout =   new ( std::nothrow ) QHBoxLayout;
        auto photosSubmitRevertLayout = new ( std::nothrow ) QHBoxLayout;
        auto updateAddRemoveLayout =    new ( std::nothrow ) QHBoxLayout;
        auto photosLayout =             new ( std::nothrow ) QVBoxLayout;
        auto pageLayout =               new ( std::nothrow ) QHBoxLayout;
        auto pageMainLayout =           new ( std::nothrow ) QHBoxLayout;
        auto patientPage =              new ( std::nothrow ) QWidget;

        if( !infoLayout ||
            !infoLabelUpdateLayout ||
            !infoSubmitRevertLayout ||
            !photosSubmitRevertLayout ||
            !updateAddRemoveLayout ||
            !photosLayout ||
            !pageLayout ||
            !pageMainLayout ||
            !patientPage )
        {
            delete infoLayout;
            delete infoSubmitRevertLayout;
            delete photosSubmitRevertLayout;
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

        photosSubmitRevertLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        photosSubmitRevertLayout->addWidget( m_submitPhotoBtn );
        photosSubmitRevertLayout->addWidget( m_revertPhotoBtn );

        photosLayout->addLayout( updateAddRemoveLayout );
        photosLayout->addWidget( m_photoSetsView );
        photosLayout->addLayout( photosSubmitRevertLayout );

        infoLabelUpdateLayout->addWidget( m_patientInfoLbl );
        infoLabelUpdateLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        infoLabelUpdateLayout->addWidget( m_updateInfoBtn );

        infoSubmitRevertLayout->addSpacerItem(
                    new ( std::nothrow ) QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
        infoSubmitRevertLayout->addWidget( m_submitInfoBtn );
        infoSubmitRevertLayout->addWidget( m_revertInfoBtn );

        infoLayout->addLayout( infoLabelUpdateLayout );
        infoLayout->addWidget( m_patientInfoView );
        infoLayout->addLayout( infoSubmitRevertLayout );

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
            if( auto model = Utility::GetSqlModel( *view ) )
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
        QString errorMsg;
        if( view )
        {
            if( !Utility::RemoveRows( *view ) )
            {
                if( auto model = Utility::GetSqlModel( *view ) )
                    errorMsg = model->lastError().text();
            }
            else
                return true;
        }
        else
            errorMsg = "Nullptr error";

        if( !errorMsg.isEmpty() )
            QMessageBox::warning( this,
                                  "Remove error",
                                  errorMsg,
                                  QMessageBox::Ok );
        return false;
    }

    bool MainWindow::submit( QTableView* view ) noexcept
    {
        QString errorMsg;
        if( view )
        {
            if( auto model = Utility::GetSqlModel( *view ) )
            {
                if( !model->submitAll() )
                {
                    errorMsg = model->lastError().text();
                    model->revertAll();
                }
                else
                    return true;
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Submit error",
                              errorMsg,
                              QMessageBox::Ok );

        return false;
    }

    bool MainWindow::revert( QTableView* view ) noexcept
    {
        QString errorMsg;
        if( view )
        {
            if( auto model = Utility::GetSqlModel( *view ) )
            {
                model->revertAll();
                return true;
            }
        }
        errorMsg = "Nullptr error";

        if( !errorMsg.isEmpty() )
            QMessageBox::warning( this,
                                  "Revert error",
                                  errorMsg,
                                  QMessageBox::Ok );
        return false;
    }

    void MainWindow::updatePatients() noexcept
    {
        if( update( m_patientsView ) )
            emit patientsChanged();
    }

    void MainWindow::addPatient() noexcept
    {
        QString errorMsg;
        if( m_patientsView )
        {
            if( auto model = Utility::GetSqlModel( *m_patientsView ) )
            {
                AddPatientDlg dialog( this );
                auto r = dialog.exec();
                if( r == QDialog::Accepted )
                {
                    const auto& p = dialog.getPatient();

                    QSqlRecord newPatientRecord = model->record();
                    newPatientRecord.setValue( "Name", p.get()->name );
                    newPatientRecord.setValue( "Address", p.get()->address );
                    newPatientRecord.setValue( "BirthDate", p.get()->birthDate );
                    newPatientRecord.setValue( "AdmissionDate", p.get()->admissionDate );
                    newPatientRecord.setValue( "DiscargeDate", p.get()->discargeDate );

                    if( !model->insertRecord( -1, newPatientRecord ) )
                        errorMsg = model->lastError().text();
                    else
                    {
                        emit patientsChanged();
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
        if( remove( m_patientsView ) )
            emit patientsChanged();
    }

    void MainWindow::updatePatientInfo() noexcept
    {
        QString errorMsg;
        if( m_patientInfoView )
        {
            if( auto proxyModel = dynamic_cast<Model::HorizontalProxyModel*>( m_patientInfoView->model() ) )
            {
                if( auto model = dynamic_cast<QSqlTableModel*>( proxyModel->sourceModel() ) )
                {
                    if( !model->select() )
                        errorMsg = model->lastError().text();
                    else
                    {
                        emit patientInfoChanged();
                        return;
                    }
                }
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Update error",
                              errorMsg,
                              QMessageBox::Ok );

    }

    void MainWindow::updatePhotoSets() noexcept
    {
        if( update( m_photoSetsView ) )
            emit photoSetsChanged();
    }

    void MainWindow::addPhoto() noexcept
    {
        QString errorMsg;
        if( m_photoSetsView )
        {
            if( auto model = Utility::GetSqlModel( *m_photoSetsView ) )
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
                            {
                                errorMsg = model->lastError().text();
                            }
                            else
                                continue;
                        }
                    }
                    emit patientsChanged();
                    if( errorMsg.isEmpty() )
                        return;
                }
                else
                    return;
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Add photos error",
                              errorMsg,
                              QMessageBox::Ok );
    }

    void MainWindow::removePhotoSets() noexcept
    {
        if( remove( m_photoSetsView ) )
            emit photoSetsChanged();
    }

    void MainWindow::returnOnMainPage() noexcept
    {
        updatePatients();
        switchPage( 0 );
    }

    void MainWindow::showPatientPage()
    {
        if( auto model = Utility::GetSqlModel( *m_patientsView ) )
        {
            const auto row = m_patientsView->selectionModel()->currentIndex().row();

            m_currentPatientId = model->index( row, 0 ).data().toLongLong();

            m_patientInfoLbl->setText( QString( "Patient #%1" ).arg( m_currentPatientId ) );

            if( auto photoSetsModel = Utility::GetSqlModel( *m_photoSetsView ) )
                photoSetsModel->setFilter( QString( "Patient_Id=%1" ).arg( m_currentPatientId ) );

            if( auto proxyModel = dynamic_cast<Model::HorizontalProxyModel*>( m_patientInfoView->model() ) )
            {
                if( auto patientInfoModel = dynamic_cast<QSqlTableModel*>( proxyModel->sourceModel() ) )
                {
                    patientInfoModel->setFilter( QString( "Id=%1" ).arg( m_currentPatientId ) );
                }
            }

            switchPage( 1 );
        }
    }

    void MainWindow::openPhoto()
    {
        if( auto model = Utility::GetSqlModel( *m_photoSetsView ) )
        {
            const int row = m_photoSetsView->selectionModel()->currentIndex().row();

            const auto& title = model->index( row, 2 ).data().toString();
            const auto& binaryImage = model->index( row, 3 ).data().toByteArray();

            m_photoViewers.append( new PhotoViewer( title, binaryImage ) );
            connect( m_photoViewers.last(), &PhotoViewer::destroyMe, this, &MainWindow::destroyPhotoViewer );
            m_photoViewers.last()->show();
        }
    }

    void MainWindow::submitPatients() noexcept
    {
        if( submit( m_patientsView ) )
            emit patientsChanged();
    }

    void MainWindow::revertPatients() noexcept
    {
        if( revert( m_patientsView ) )
            emit patientsChanged();
    }

    void MainWindow::submitPatientInfo() noexcept
    {
        QString errorMsg;
        if( m_patientInfoView )
        {
            if( auto proxyModel = dynamic_cast<Model::HorizontalProxyModel*>( m_patientInfoView->model() ) )
            {
                if( auto model = dynamic_cast<QSqlTableModel*>( proxyModel->sourceModel() ) )
                {
                    if( !model->submitAll() )
                    {
                        errorMsg = model->lastError().text();
                        model->revertAll();
                    }
                    else
                    {
                        emit patientInfoChanged();
                        return;
                    }
                }
            }
        }

        if( errorMsg.isEmpty() )
            errorMsg = "Nullptr error";

        QMessageBox::warning( this,
                              "Submit error",
                              errorMsg,
                              QMessageBox::Ok );
    }

    void MainWindow::revertPatientInfo() noexcept
    {
        if( m_patientInfoView )
        {
            if( auto proxyModel = dynamic_cast<Model::HorizontalProxyModel*>( m_patientInfoView->model() ) )
            {
                if( auto model = dynamic_cast<QSqlTableModel*>( proxyModel->sourceModel() ) )
                {
                    model->revertAll();
                    emit patientInfoChanged();
                    return;
                }
            }
        }

        QMessageBox::warning( this,
                              "Submit error",
                              "Nullptr error",
                              QMessageBox::Ok );
    }

    void MainWindow::submitPhotoSets() noexcept
    {
        if( submit( m_photoSetsView ) )
            emit photoSetsChanged();
    }

    void MainWindow::revertPhotoSets() noexcept
    {
        if( revert( m_photoSetsView ) )
            emit photoSetsChanged();
    }

    void MainWindow::checkPatients() noexcept
    {
        if( auto model = Utility::GetSqlModel( *m_patientsView ) )
        {
            Utility::SetEnabledIfDirty( *model, { m_submitPatientBtn, m_revertPatientBtn } );
        }
    }

    void MainWindow::checkPatientInfo() noexcept
    {
        if( auto proxyModel = dynamic_cast<Model::HorizontalProxyModel*>( m_patientInfoView->model() ) )
        {
            if( auto model = dynamic_cast<QSqlTableModel*>( proxyModel->sourceModel() ) )
            {
                Utility::SetEnabledIfDirty( *model, { m_submitInfoBtn, m_revertInfoBtn } );
            }
        }
    }

    void MainWindow::checkPhotoSets() noexcept
    {
        if( auto model = Utility::GetSqlModel( *m_photoSetsView ) )
        {
            Utility::SetEnabledIfDirty( *model, { m_submitPhotoBtn, m_revertPhotoBtn } );
        }
    }

    void MainWindow::destroyPhotoViewer( PhotoViewer* viewer ) noexcept
    {
        m_photoViewers.removeAll( viewer );
        delete viewer;
    }
}

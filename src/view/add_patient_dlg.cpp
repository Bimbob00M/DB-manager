#include "add_patient_dlg.h"

#include<utility>

#include <QCalendarWidget>
#include <QDateTime>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "utility/global.h"

namespace PatientsDBManager
{
    AddPatientDlg::AddPatientDlg( QWidget* parent ) noexcept
        : QDialog( parent )
        , m_patientInfo( "New patient info", this )
        , m_dialogBtn( QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal, this )
        , m_patient()
    {
        if( !m_patientInfo.isValid() ||
            !setupLayout() )
        {
            qDebug() << "AddPatientDlg: init failed";
            reject();
        }

        layout()->setSizeConstraint( QLayout::SetFixedSize );
        setWindowTitle( "Add patient" );
        const auto& now = QDateTime::currentDateTime();
        m_patientInfo.setAdmissionDate( now );
        m_patientInfo.setDiscargeDate( now );

        connect( &m_dialogBtn, &QDialogButtonBox::clicked, this, &AddPatientDlg::onDialogBtnClicked );
    }

    std::unique_ptr<Patient> AddPatientDlg::getPatient() const noexcept
    {
        return std::make_unique<Patient>( m_patient );
    }

    bool AddPatientDlg::setupLayout() noexcept
    {
        auto mainLayout = new ( std::nothrow ) QVBoxLayout( this );

        if( !mainLayout )
            return false;

        mainLayout->addWidget( &m_patientInfo );
        mainLayout->addWidget( &m_dialogBtn );

        setLayout( mainLayout );
        return true;
    }

    void AddPatientDlg::onDialogBtnClicked( QAbstractButton* button )
    {
        auto btn = static_cast<QPushButton*>( button );
        if( btn == m_dialogBtn.button( QDialogButtonBox::Cancel ) )
        {
            emit canceled();
            reject();
        }
        else if( btn == m_dialogBtn.button( QDialogButtonBox::Ok ) )
        {
            if( m_patientInfo.getNameField()->text().isEmpty() ||
                m_patientInfo.getAddressField()->text().isEmpty() )
            {
                QString fieldName;
                if( m_patientInfo.getNameField()->text().isEmpty() )
                    fieldName = "\"Name\"";
                else
                    fieldName = "\"Address\"";

                QMessageBox::warning( this,
                                      "Incorrect data",
                                      fieldName + " field cannot be empty",
                                      QMessageBox::Ok );
                return;
            }

            using namespace Global;
            m_patient = Patient( m_patientInfo.getNameField()->text(),
                                  m_patientInfo.getAddressField()->text(),
                                  m_patientInfo.getBirthDateField()->date().toString( DATE_FORMAT ),
                                  m_patientInfo.getAdmissionDateField()->date().toString( DATE_FORMAT ),
                                  m_patientInfo.getDiscargeDateField()->date().toString( DATE_FORMAT ) ) ;

            emit confirmed( m_patient );
            accept();
        }
    }
}

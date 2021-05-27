#include "patient_info_form.h"

#include <QCalendarWidget>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QRegExpValidator>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "utility/global.h"

namespace PatientsDBManager
{

    PatientInfoForm::PatientInfoForm( QWidget* parent ) noexcept
        : QWidget( parent )
    {
        init();
    }

    PatientInfoForm::PatientInfoForm(const QString &infoGrouptitle, QWidget *parent) noexcept
        : QWidget( parent )
    {
        init();
        setInfoGroupTitle( infoGrouptitle );
    }

    void PatientInfoForm::setInfoGroupTitle( const QString& title ) noexcept
    {
        m_patientInfoGroup->setTitle( title );
    }

    bool PatientInfoForm::init() noexcept
    {
        if( !setupControls() ||
                !setupLayout() )
        {
            m_validationFlag = false;
            return false;
        }
        m_validationFlag = true;
        return true;
    }

    bool PatientInfoForm::setupControls() noexcept
    {
        m_nameField =          new ( std::nothrow ) QLineEdit;
        m_addressField =       new ( std::nothrow ) QLineEdit;
        m_birthDateField =     new ( std::nothrow ) QDateEdit;
        m_admissionDateField = new ( std::nothrow ) QDateEdit;
        m_discargeDateField =  new ( std::nothrow ) QDateEdit;
        m_patientInfoGroup =   new ( std::nothrow ) QGroupBox;

        if( !m_nameField ||
            !m_addressField ||
            !m_birthDateField ||
            !m_addressField ||
            !m_discargeDateField ||
            !m_patientInfoGroup )
        {
            delete m_nameField;
            delete m_addressField;
            delete m_birthDateField;
            delete m_addressField;
            delete m_discargeDateField;
            delete m_patientInfoGroup;
            return false;
        }

        m_birthDateField->setCalendarPopup( true );
        m_admissionDateField->setCalendarPopup( true );
        m_discargeDateField->setCalendarPopup( true );

        QSizePolicy fieldsPolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

        const auto& today = QDate::currentDate();
        m_admissionDateField->setMinimumDate( today );
        m_discargeDateField->setMinimumDate( today );

        m_nameField->setSizePolicy( fieldsPolicy );
        m_addressField->setSizePolicy( fieldsPolicy );
        m_birthDateField->setSizePolicy( fieldsPolicy );
        m_admissionDateField ->setSizePolicy( fieldsPolicy );
        m_discargeDateField->setSizePolicy( fieldsPolicy );

        connect( m_admissionDateField, &QDateEdit::dateChanged, m_discargeDateField, &QDateEdit::setMinimumDate );

        return true;
    }

    bool PatientInfoForm::setupLayout() noexcept
    {
        auto nameLbl =          new ( std::nothrow ) QLabel( "&Name:" );
        auto addressLbl =       new ( std::nothrow ) QLabel( "&Address:" );
        auto birthDateLbl =     new ( std::nothrow ) QLabel( "&Birth date:" );
        auto admissionDateLbl = new ( std::nothrow ) QLabel( "&Admission date:" );
        auto discargeDateLbl =  new ( std::nothrow ) QLabel( "&Discarge date:" );

        auto patientFormLayout = new ( std::nothrow ) QFormLayout;
        auto mainLayout =        new ( std::nothrow ) QVBoxLayout;

        if( !nameLbl ||
            !addressLbl ||
            !birthDateLbl ||
            !admissionDateLbl ||
            !discargeDateLbl ||
            !patientFormLayout ||
            !mainLayout )
        {
            delete nameLbl;
            delete addressLbl;
            delete addressLbl;
            delete admissionDateLbl;
            delete discargeDateLbl;

            delete patientFormLayout;
            delete mainLayout;

            return false;
        }

        nameLbl->setBuddy( m_nameField );
        addressLbl->setBuddy( m_addressField );
        birthDateLbl->setBuddy( m_birthDateField );
        admissionDateLbl->setBuddy( m_admissionDateField  );
        discargeDateLbl->setBuddy( m_discargeDateField );

        patientFormLayout->addRow( nameLbl, m_nameField );
        patientFormLayout->addRow( addressLbl , m_addressField );
        patientFormLayout->addRow( birthDateLbl, m_birthDateField );
        patientFormLayout->addRow( admissionDateLbl, m_admissionDateField  );
        patientFormLayout->addRow( discargeDateLbl, m_discargeDateField );

        m_patientInfoGroup ->setLayout( patientFormLayout );
        mainLayout->addWidget( m_patientInfoGroup );
        setLayout( mainLayout );

        return true;
    }    
}

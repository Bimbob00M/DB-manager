#ifndef PATIENTINFO_H
#define PATIENTINFO_H

#include <optional>

#include <QDateEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QWidget>

namespace PatientsDBManager
{
    class PatientInfoForm : public QWidget
    {
        Q_OBJECT
    public:
        PatientInfoForm( QWidget* parent = nullptr ) noexcept;
        PatientInfoForm( const QString& infoGrouptitle, QWidget* parent = nullptr ) noexcept;

        QLineEdit*       getNameField() noexcept { return m_nameField; }
        const QLineEdit* getNameField() const noexcept { return m_nameField; }

        QLineEdit*       getAddressField() noexcept { return m_addressField; }
        const QLineEdit* getAddressField() const noexcept { return m_addressField; }

        QDateEdit*       getBirthDateField() noexcept { return m_birthDateField; }
        const QDateEdit* getBirthDateField() const noexcept { return m_birthDateField; }

        QDateEdit*       getAdmissionDateField() noexcept { return m_admissionDateField; }
        const QDateEdit* getAdmissionDateField() const noexcept { return m_admissionDateField; }

        QDateEdit*       getDiscargeDateField() noexcept { return m_discargeDateField; }
        const QDateEdit* getDiscargeDateField() const noexcept { return m_discargeDateField; }

        bool isValid() const noexcept { return m_validationFlag; }

        void setInfoGroupTitle( const QString& title ) noexcept;
        void setName( const QString& name) noexcept{ m_nameField->setText( name ); }
        void setAddress( const QString& address ) noexcept{ m_addressField->setText( address ); }
        void setBirthDate( const QDate& date ) noexcept{ m_birthDateField->setDate( date ); }
        void setAdmissionDate( const QDateTime& date ) noexcept{ m_admissionDateField->setDateTime( date ); }
        void setDiscargeDate( const QDateTime& date ) noexcept{ m_discargeDateField->setDateTime( date ); }

    private:
        QLineEdit* m_nameField{ nullptr };
        QLineEdit* m_addressField{ nullptr };
        QDateEdit* m_birthDateField{ nullptr };
        QDateEdit* m_admissionDateField{ nullptr };
        QDateEdit* m_discargeDateField{ nullptr };
        QGroupBox* m_patientInfoGroup{ nullptr };

        bool m_validationFlag{ false };

        bool init() noexcept;
        bool setupControls() noexcept;
        bool setupLayout() noexcept;

    };
}



#endif // PATIENTINFO_H

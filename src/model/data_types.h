#ifndef PATIENT_H
#define PATIENT_H

#include <QDate>
#include <QDateTime>

namespace PatientsDBManager
{
    struct Patient
    {
    public:
        QString name;
        QString address;
        QString birthDate;
        QString admissionDate;
        QString discargeDate;

        Patient() noexcept;
        Patient( const QString& name,
                 const QString& address,
                 const QString& birthDate,
                 const QString& admissionDate,
                 const QString& discargeDate ) noexcept;

        Patient( const Patient& patient );
        Patient& operator=( const Patient& patient );

        Patient( Patient&& patient );
        Patient& operator=( Patient&& patient );
    };
}


#endif // PATIENT_H

#include "data_types.h"

#include <utility>

namespace PatientsDBManager
{
    Patient::Patient() noexcept
    {}

    Patient::Patient( const QString& name,
                      const QString& address,
                      const QString& birthDate,
                      const QString& admissionDate,
                      const QString& discargeDate ) noexcept
        : name{ name }
        , address{ address }
        , birthDate{ birthDate }
        , admissionDate{ admissionDate }
        , discargeDate{ discargeDate }
    {}

    Patient::Patient( const Patient& p )
        : Patient( p.name, p.address, p.birthDate, p.admissionDate, p.discargeDate )
    {}

    Patient& Patient::operator=( const Patient& p )
    {
        if( this == &p )
            return *this;

        name = p.name;
        address = p.address;
        birthDate = p.birthDate;
        admissionDate = p.admissionDate;
        discargeDate = p.discargeDate;

        return *this;
    }

    Patient::Patient( Patient&& p )
        : name( std::move( p.name ) )
        , address( std::move( p.address ) )
        , birthDate( std::move( p.birthDate ) )
        , admissionDate( std::move( p.admissionDate ) )
        , discargeDate( std::move( p.discargeDate ) )
    {}

    Patient& Patient::operator=( Patient&& p )
    {
        if( this == &p )
            return *this;

        name = std::move( p.name );
        address = std::move( p.address );
        birthDate = std::move( p.birthDate );
        admissionDate = std::move( p.admissionDate );
        discargeDate = std::move( p.discargeDate );

        return *this;
    }
}

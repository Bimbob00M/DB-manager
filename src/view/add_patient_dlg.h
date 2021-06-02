    #ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <memory>

#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

#include "model/data_types.h"
#include "patient_info_form.h"

namespace PatientsDBManager
{
    class AddPatientDlg : public QDialog
    {
        Q_OBJECT
    public:
        AddPatientDlg( QWidget* parent = nullptr ) noexcept;

        const Patient& getPatient() const noexcept{ return m_patient; }

    signals:
        void confirmed( const Patient& patient );
        void canceled();

    private:
        PatientInfoForm  m_patientInfo;
        QDialogButtonBox m_dialogBtn;

        Patient m_patient;

        bool setupLayout() noexcept;

    private slots:
        void onDialogBtnClicked( QAbstractButton* button );
    };
}
#endif // ADDDIALOG_H

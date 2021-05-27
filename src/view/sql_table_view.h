#ifndef SQLTABLEVIEW_H
#define SQLTABLEVIEW_H

#include <QTableView>

namespace PatientsDBManager
{
    class SqlTableView : public QTableView
    {
        Q_OBJECT
    public:
        SqlTableView( QWidget* parent = nullptr ) noexcept;

    signals:
        void leftDoubleClicked();
        void rightDoubleClicked();

    protected:
        void mouseDoubleClickEvent( QMouseEvent* event ) override;
    };
} //namespace PatientsDBManager

#endif // SQLTABLEVIEW_H

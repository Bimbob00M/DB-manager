#include "sql_table_view.h"

#include <QMouseEvent>

namespace PatientsDBManager
{
    SqlTableView::SqlTableView( QWidget* parent ) noexcept
        : QTableView( parent )
    {}

    void SqlTableView::mouseDoubleClickEvent( QMouseEvent* event )
    {
        if ( event->button() == Qt::LeftButton )
            emit leftDoubleClicked();
        else if( event->button() == Qt::RightButton )
            emit rightDoubleClicked();

        QTableView::mouseDoubleClickEvent( event );
    }
} //namespace PatientsDBManager

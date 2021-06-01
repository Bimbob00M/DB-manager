#include "table_view_ex.h"

#include <QMouseEvent>

namespace PatientsDBManager
{
    TableViewEx::TableViewEx( QWidget* parent ) noexcept
        : QTableView( parent )
    {}

    void TableViewEx::mouseDoubleClickEvent( QMouseEvent* event )
    {
        if ( event->button() == Qt::LeftButton )
            emit leftDoubleClicked();
        else if( event->button() == Qt::RightButton )
            emit rightDoubleClicked();

        QTableView::mouseDoubleClickEvent( event );
    }
} //namespace PatientsDBManager

#include <QApplication>
#include <QMessageBox>

#include "view/main_window.h"

int main( int argc, char *argv[] )
{
    QApplication a(argc, argv);

    if( argc != 2 )
    {
        QMessageBox::critical( nullptr,
                               "Arguments error",
                               "Wrong number of arguments passed.\n"
                               "You must specify the path to the database.",
                               QMessageBox::Ok );
        return 0;
    }

    PatientsDBManager::MainWindow w( argv[1] );
    w.show();
    return a.exec();
}

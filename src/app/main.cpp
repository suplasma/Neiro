#include <QApplication>

#include <form.h>

#include <integration/app-connector.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    service::integration::AppConnector appConnector(app);
    Form form;
    form.show();
    return app.exec();
}

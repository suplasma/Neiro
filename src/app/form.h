#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include <neiro.h>

namespace Ui {
class Form;
}

class Form : public QWidget {
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private slots:
    void on_startBtn_clicked();
    void on_stopBtn_clicked();
    void on_testBtn_clicked();
    void on_requestBtn_clicked();
    void on_clearBtn_clicked();
    void on_runningBtn_clicked();
    void on_originBtn_clicked();
    void on_aspirationBtn_clicked();
    void on_testBtn_2_clicked();
    void on_initBtn_clicked();

private:
    Ui::Form *ui;
    Neiro neiro;
};

#endif // FORM_H

#include "form.h"

#include <QDebug>
#include <QFileDialog>

#include <thread>

#include "ui_form.h"

Form::Form(QWidget *parent) : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->stopBtn->setVisible(false);
    ui->startBtn->setVisible(false);
    ui->runningBtn->setVisible(false);
    ui->requestBtn->setVisible(false);
    ui->testBtn->setVisible(false);
}

Form::~Form()
{
    delete ui;
}

void Form::on_startBtn_clicked()
{
    neiro.start2();
    ui->stopBtn->setVisible(true);
    ui->startBtn->setVisible(false);
}

void Form::on_stopBtn_clicked()
{
    neiro.stop();
    ui->stopBtn->setVisible(false);
    ui->startBtn->setVisible(true);
}

void Form::on_testBtn_clicked()
{
    neiro.test();
}

void Form::on_requestBtn_clicked()
{
    neiro.requestPicture(ui->test->text()).save(ui->result->text());
}

void Form::on_clearBtn_clicked()
{
    neiro.clear();
}

void Form::on_runningBtn_clicked()
{
    std::thread th([this]() {
        forever
        {
            neiro.running();
            ui->label->setPixmap(QPixmap::fromImage(neiro.requestPicture().scaled(4, 2)));
        }
    });

    th.detach();
}

void Form::on_originBtn_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Ориганильный файл", "../data",
                                               "Изображение(*.png *.jpg)");
    if (!str.isEmpty()) {
        ui->origin->setText(QString("../data/%1").arg(str.split("/").last()));
    }
}

void Form::on_aspirationBtn_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Преобразованный файл", "../data",
                                               "Изображение(*.png *.jpg)");
    if (!str.isEmpty()) {
        ui->aspiration->setText(QString("../data/%1").arg(str.split("/").last()));
    }
}

void Form::on_testBtn_2_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Тестовый файл", "../data",
                                               "Изображение(*.png *.jpg)");
    if (!str.isEmpty()) {
        ui->test->setText(QString("../data/%1").arg(str.split("/").last()));
    }
}

void Form::on_initBtn_clicked()
{
    neiro.init(ui->origin->text(), ui->aspiration->text());
    ui->startBtn->setVisible(true);
    ui->runningBtn->setVisible(true);
    ui->requestBtn->setVisible(true);
    ui->testBtn->setVisible(true);
}

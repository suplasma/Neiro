#ifndef NEIRO_H
#define NEIRO_H

#include <QLabel>
#include <QRandomGenerator>
#include <QVector>

class Neiro {
public:
    Neiro();
    void init(); //Инициализация
    void init(const QString &fileOrigin, const QString &fileAspiration); //Инициализация
    void init(const QString &file); //Инициализация
    void start();                   //Старт обучения
    void start2();                  //Старт обучения
    void stop();                    //Стоп
    void test();                    //Тест
    QVector<double> request(const QVector<double> &req);
    QImage requestPicture(const QString &file);
    QImage requestPicture();
    void clear();
    void running();

private:
    struct Neiron {   //Нейрон
        double val;   //Значение
        double error; //Ошибка
    };

    void initLayers(QVector<int> sizes);  //Создать слои
    void initWeights(QVector<int> sizes); //Создать веса
    void getTask(QVector<Neiron> &layer, QVector<double> &aspiration, int i); //Загрузить пример
    void forwards(const QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                  QVector<Neiron> &layerSecond); //Работа нейронов
    void findError(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                   const QVector<Neiron> &layerSecond); //Расчет ошибки
    void findError2(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                    const QVector<Neiron> &layerSecond); //Расчет ошибки
    QString findError(QVector<Neiron> &layer,
                      const QVector<double> &aspiration); //Расчет ошибки
    QString findError(QVector<Neiron> &layer, const QVector<double> &aspiration,
                      double &error); //Расчет ошибки
    void backWards(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                   const QVector<Neiron> &layerSecond); //Корректирование весов
    void backWards2(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                    const QVector<Neiron> &layerSecond); //Корректирование весов
    double activation(double x);                         //Функция активации
    double fluxion(double x);                            //Производная
    void readData(QVector<int> &sizes);                  //Читает данные
    void readDataPicture(QVector<int> &sizes, const QString &fileName); //Читает данные
    void readDataPicture(QVector<int> &sizes, const QString &fileOrigin,
                         const QString &fileAspiration); //Читает данные
    void readWeights();                                  //Читает веса
    void saveWeights();                                  //Сохраняет веса

    QVector<QVector<Neiron>> layers; //Слои [слой][нейрон]
    QVector<double> aspiration;      //Правильные ответы
    QVector<QVector<QVector<double>>>
        weights; //Веса [слой][нейрон из слоя][нейрон из следующего слоя]
    QVector<QVector<Neiron>> fileLayers;
    QVector<QVector<double>> fileAspirations;

    bool isLearning; //Идет ли обучение
    double learningRate;
    QRandomGenerator generator;
};

#endif // NEIRO_H

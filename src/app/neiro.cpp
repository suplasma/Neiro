#include "neiro.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <thread>

#include <math.h>

Neiro::Neiro() {}

void Neiro::init()
{
    QVector<int> sizes;
    readData(sizes);
    initLayers(sizes);

    for (int i = 0; i < sizes.last(); ++i) {
        aspiration.append(0);
    }

    initWeights(sizes);
    readWeights();
}

void Neiro::init(const QString &fileOrigin, const QString &fileAspiration)
{
    QVector<int> sizes;
    readDataPicture(sizes, fileOrigin, fileAspiration);
    initLayers(sizes);

    aspiration.clear();
    for (int i = 0; i < sizes.last(); ++i) {
        aspiration.append(0);
    }

    initWeights(sizes);
    readWeights();
}

void Neiro::init(const QString &file)
{
    QVector<int> sizes;
    readDataPicture(sizes, file);
    initLayers(sizes);

    aspiration.clear();
    for (int i = 0; i < sizes.last(); ++i) {
        aspiration.append(0);
    }

    initWeights(sizes);
    readWeights();
}

void Neiro::start()
{
    isLearning = true;
    std::thread th([this]() {
        while (isLearning) {
            getTask(layers.first(), aspiration, generator.generate() % fileLayers.size());

            for (int i = 0; i < weights.size(); ++i) {
                forwards(layers[i], weights[i], layers[i + 1]);
            }

            qDebug() << findError(layers.last(), aspiration);
            for (int i = weights.size() - 1; i > 0; --i) {
                findError(layers[i], weights[i], layers[i + 1]);
            }

            for (int i = weights.size() - 1; i >= 0; --i) {
                backWards(layers[i], weights[i], layers[i + 1]);
            }
        }
    });
    th.detach();
}

void Neiro::start2()
{
    isLearning = true;
    std::thread th([this]() {
        while (isLearning) {
            getTask(layers.first(), aspiration, generator.generate() % fileLayers.size());

            for (int i = 0; i < weights.size(); ++i) {
                forwards(layers[i], weights[i], layers[i + 1]);
            }

            qDebug() << findError(layers.last(), aspiration);
            for (int i = weights.size() - 1; i > 0; --i) {
                findError2(layers[i], weights[i], layers[i + 1]);
            }

            for (int i = weights.size() - 1; i >= 0; --i) {
                backWards2(layers[i], weights[i], layers[i + 1]);
            }
        }
    });
    th.detach();
}

void Neiro::stop()
{
    isLearning = false;
    saveWeights();
}

void Neiro::test()
{
    double res = 0;
    for (int j = 0; j < fileLayers.size(); ++j) {
        getTask(layers.first(), aspiration, j);

        for (int i = 0; i < weights.size(); ++i) {
            forwards(layers[i], weights[i], layers[i + 1]);
        }

        double error = 0;
        QString str;
        str += findError(layers.last(), aspiration, error);
        str += QString(" %1").arg(error);
        res += error;
    }
    qDebug() << res;
}

QImage Neiro::requestPicture(const QString &file)
{
    QPixmap pix(file);
    QImage image = pix.toImage();
    for (int w = 0; w < image.width(); ++w)
        for (int h = 0; h < image.height(); ++h) {
            layers.first()[0].val = image.pixelColor(w, h).redF();
            layers.first()[1].val = image.pixelColor(w, h).greenF();
            layers.first()[2].val = image.pixelColor(w, h).blueF();

            for (int i = 0; i < weights.size(); ++i) {
                forwards(layers[i], weights[i], layers[i + 1]);
            }
            QColor color;
            color.setRedF(layers.last()[0].val);
            color.setGreenF(layers.last()[1].val);
            color.setBlueF(layers.last()[2].val);
            image.setPixelColor(w, h, color);
        }
    return image;
}

QImage Neiro::requestPicture()
{
    QImage image(4, 2, QImage::Format::Format_RGB32);
    for (int w = 0; w < image.width(); ++w)
        for (int h = 0; h < image.height(); ++h) {
            layers.first()[0].val = w;
            layers.first()[1].val = h;

            for (int i = 0; i < weights.size(); ++i) {
                forwards(layers[i], weights[i], layers[i + 1]);
            }
            QColor color;
            color.setRedF(layers.last()[0].val);
            color.setGreenF(layers.last()[1].val);
            color.setBlueF(layers.last()[2].val);
            image.setPixelColor(w, h, color);
        }
    return image;
}

void Neiro::clear()
{
    QFile file("../data/weights.txt");
    file.open(QFile::WriteOnly);
    file.close();
}

void Neiro::running()
{
    getTask(layers.first(), aspiration, generator.generate() % fileLayers.size());

    for (int i = 0; i < weights.size(); ++i) {
        forwards(layers[i], weights[i], layers[i + 1]);
    }

    qDebug() << findError(layers.last(), aspiration);
    for (int i = weights.size() - 1; i > 0; --i) {
        findError(layers[i], weights[i], layers[i + 1]);
    }

    for (int i = weights.size() - 1; i >= 0; --i) {
        backWards(layers[i], weights[i], layers[i + 1]);
    }
}

QVector<double> Neiro::request(const QVector<double> &req)
{
    QVector<double> res;
    for (int i = 0; i < layers.first().size(); ++i) {
        layers.first()[i].val = req[i];
    }
    for (int i = 0; i < weights.size(); ++i) {
        forwards(layers[i], weights[i], layers[i + 1]);
    }
    for (int i = 0; i < layers.last().size(); ++i) {
        res.append(layers.last()[i].val);
    }

    return res;
}

void Neiro::initLayers(QVector<int> sizes)
{
    layers.clear();
    for (int size : sizes) {
        layers.append(QVector<Neiron>());
        for (int i = 0; i < size; ++i) {
            layers.last().append(Neiron());
        }
        layers.last()[size - 1].val = 1;
    }
}

void Neiro::initWeights(QVector<int> sizes)
{
    weights.clear();
    for (int j = 1; j < sizes.size(); ++j) {
        weights.append(QVector<QVector<double>>());
        for (int i = 0; i < sizes[j - 1]; ++i) {
            weights.last().append(QVector<double>());
            for (int t = 0; t < sizes[j] - 1; ++t) {
                weights.last().last().append(0);
            }
        }
    }

    for (QVector<double> &weigth : weights.last()) {
        weigth.append(0);
    }
}

void Neiro::getTask(QVector<Neiron> &layer, QVector<double> &aspiration, int i)
{
    for (int j = 0; j < fileLayers[i].size(); ++j) {
        layer[j] = fileLayers[i][j];
    }

    for (int j = 0; j < fileAspirations[i].size(); ++j) {
        aspiration[j] = fileAspirations[i][j];
    }
}

void Neiro::forwards(const QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                     QVector<Neiron> &layerSecond)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        layerSecond[i2].val = 0;
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            layerSecond[i2].val += layerFirst[i1].val * weight[i1][i2];
        }
        layerSecond[i2].val = activation(layerSecond[i2].val);
    }
}

void Neiro::findError(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                      const QVector<Neiron> &layerSecond)
{
    for (int i1 = 0; i1 < weight.size() - 1; ++i1) {
        layerFirst[i1].error = 0;
        for (int i2 = 0; i2 < weight.first().size(); ++i2) {
            layerFirst[i1].error += weight[i1][i2] * layerSecond[i2].error;
        }
    }
}

QString Neiro::findError(QVector<Neiro::Neiron> &layer, const QVector<double> &aspiration)
{
    QString str;
    for (int i = 0; i < layer.size(); ++i) {
        layer[i].error = aspiration[i] - layer[i].val;
        str += QString("%1 %2, ").arg(aspiration[i]).arg(layer[i].val);
    }
    return str;
}

QString Neiro::findError(QVector<Neiron> &layer, const QVector<double> &aspiration,
                         double &error)
{
    QString str;
    for (int i = 0; i < layer.size(); ++i) {
        layer[i].error = aspiration[i] - layer[i].val;
        error += layer[i].error * layer[i].error;
        str += QString("%1 %2, ").arg(aspiration[i]).arg(layer[i].val);
    }
    return str;
}

void Neiro::backWards2(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                       const QVector<Neiron> &layerSecond)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            weight[i1][i2] += learningRate * layerSecond[i2].error * layerFirst[i1].val;
        }
    }
}

double Neiro::activation(double x)
{
    return 1 / (1 + exp(-x));
}

double Neiro::fluxion(double x)
{
    return x * (1 - x);
}

void Neiro::findError2(QVector<Neiron> &layerFirst, const QVector<QVector<double>> &weight,
                       const QVector<Neiron> &layerSecond)
{
    for (int i1 = 0; i1 < weight.size() - 1; ++i1) {
        layerFirst[i1].error = 0;
        for (int i2 = 0; i2 < weight.first().size(); ++i2) {
            layerFirst[i1].error += weight[i1][i2] * layerSecond[i2].error;
        }
        layerFirst[i1].error *= fluxion(layerFirst[i1].val);
    }
}

void Neiro::backWards(const QVector<Neiron> &layerFirst, QVector<QVector<double>> &weight,
                      const QVector<Neiron> &layerSecond)
{
    for (int i2 = 0; i2 < weight.first().size(); ++i2) {
        for (int i1 = 0; i1 < weight.size(); ++i1) {
            weight[i1][i2] += learningRate * layerSecond[i2].error *
                              fluxion(layerSecond[i2].val) * layerFirst[i1].val;
        }
    }
}

void Neiro::readData(QVector<int> &sizes)
{
    QFile file("../data/data.txt");
    file.open(QFile::ReadOnly);

    QString rate = file.readLine();
    learningRate = rate.toDouble();
    QString str = file.readLine();
    auto split = str.split(" ");
    for (auto el : split) {
        sizes.append(el.toInt());
    }

    while (!file.atEnd()) {
        QString str = file.readLine();
        auto split = str.split(" | ");
        QString strLayers = split.first();
        QString strAspirations = split.last();

        fileLayers.append(QVector<Neiron>());
        for (auto layer : strLayers.split(" ")) {
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = layer.toDouble();
        }

        fileAspirations.append(QVector<double>());
        for (auto aspiration : strAspirations.split(" ")) {
            fileAspirations.last().append(aspiration.toDouble());
        }
    }
    file.close();
}

void Neiro::readDataPicture(QVector<int> &sizes, const QString &fileName)
{
    QFile file("../data/data.txt");
    file.open(QFile::ReadOnly);

    QString rate = file.readLine();
    learningRate = rate.toDouble();
    QString str = file.readLine();
    file.close();
    auto split = str.split(" ");
    for (auto el : split) {
        sizes.append(el.toInt());
    }

    fileLayers.clear();
    fileAspirations.clear();

    QPixmap pixLayers(fileName);
    QImage imageLayers = pixLayers.toImage();
    for (int i = 0; i < imageLayers.width(); i++) {
        for (int j = 0; j < imageLayers.height(); j++) {
            fileLayers.append(QVector<Neiron>());
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = i;
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = j;

            fileAspirations.append(QVector<double>());
            fileAspirations.last().append(imageLayers.pixelColor(i, j).redF());
            fileAspirations.last().append(imageLayers.pixelColor(i, j).greenF());
            fileAspirations.last().append(imageLayers.pixelColor(i, j).blueF());
        }
    }
}

void Neiro::readDataPicture(QVector<int> &sizes, const QString &fileOrigin,
                            const QString &fileAspiration)
{
    QFile file("../data/data.txt");
    file.open(QFile::ReadOnly);

    QString rate = file.readLine();
    learningRate = rate.toDouble();
    QString str = file.readLine();
    file.close();
    auto split = str.split(" ");
    for (auto el : split) {
        sizes.append(el.toInt());
    }

    fileLayers.clear();
    fileAspirations.clear();

    QPixmap pixLayers(fileOrigin);
    QImage imageLayers = pixLayers.toImage();
    QPixmap pixAspiration(fileAspiration);
    QImage imageAspiration = pixAspiration.toImage();
    for (int i = 0; i < imageLayers.width(); i++) {
        for (int j = 0; j < imageLayers.height(); j++) {
            fileLayers.append(QVector<Neiron>());
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = imageLayers.pixelColor(i, j).redF();
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = imageLayers.pixelColor(i, j).greenF();
            fileLayers.last().append(Neiron());
            fileLayers.last().last().val = imageLayers.pixelColor(i, j).blueF();
            fileLayers.last().append(Neiron());

            fileAspirations.append(QVector<double>());
            fileAspirations.last().append(imageAspiration.pixelColor(i, j).redF());
            fileAspirations.last().append(imageAspiration.pixelColor(i, j).greenF());
            fileAspirations.last().append(imageAspiration.pixelColor(i, j).blueF());
        }
    }
}

void Neiro::readWeights()
{
    QFile file("../data/weights.txt");
    file.open(QFile::ReadOnly);
    QJsonArray a1 = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (int i = 0; i < a1.size(); ++i) {
        QJsonArray a2 = a1[i].toArray();
        for (int j = 0; j < a2.size(); ++j) {
            QJsonArray a3 = a2[j].toArray();
            for (int t = 0; t < a3.size(); ++t) {
                weights[i][j][t] = a3[t].toDouble();
            }
        }
    }
}

void Neiro::saveWeights()
{
    QJsonArray a1;
    for (auto layer : weights) {
        QJsonArray a2;
        for (auto neiron : layer) {
            QJsonArray a3;
            for (auto weight : neiron) {
                a3.append(weight);
            }
            a2.append(a3);
        }
        a1.append(a2);
    }

    QFile file("../data/weights.txt");
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument(a1).toJson());
    file.close();
}

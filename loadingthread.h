#ifndef LOADINGTHREAD_H
#define LOADINGTHREAD_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include "blistpicture.h"
#include <cv.hpp>
using namespace cv;

class LoadingThread : public QThread
{
    Q_OBJECT
public:
    explicit LoadingThread(QDir dir,QVector<BlistPicture*> *pictures,QVector<QTime> *times,QVector<double> *current,Rect ROI,bool loadExel,QObject *parent = 0);
    QDir dir;
    QVector<BlistPicture*> *pictures;
    bool loadExel;
    QVector<QTime>* times;
    QVector<double> *current;
    Rect ROI;

    void run();

    void loadBlistPictures(QDir dir);
    void calculateDifferense();
    void loadExels(QDir dir);
signals:
    void loadingStatus(QString text);
    void setProgress(int value);
public slots:
};

#endif // LOADINGTHREAD_H

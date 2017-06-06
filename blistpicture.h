#ifndef BLISTPICTURE_H
#define BLISTPICTURE_H

#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QTime>
#include <QRegExp>
#include <cv.hpp>

using namespace cv;
class BlistPicture : public QObject
{
    Q_OBJECT
public:
    BlistPicture(QFileInfo fileInfo, QObject *parent = 0);
    bool operator==(BlistPicture const &a);
    bool operator!=(BlistPicture const &a);
    bool operator<(BlistPicture const &a);
    bool operator>(BlistPicture const &a);
    bool operator<=(BlistPicture const &a);
    bool operator>=(BlistPicture const &a);

    bool operator <(const BlistPicture *a);

    void calculateBrightness();
    QFileInfo fileInfo;
    QTime time;
    Mat pic;
    BlistPicture *startPicture = nullptr;
    double brightnessDifference;
signals:

public slots:
};

#endif // BLISTPICTURE_H

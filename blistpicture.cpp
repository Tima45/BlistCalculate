#include "blistpicture.h"

BlistPicture::BlistPicture(QFileInfo fileInfo, QObject *parent) : QObject(parent)
{
    this->fileInfo = fileInfo;



    QRegExp tampleFileName("Image_0_*.jpg");
    tampleFileName.setPatternSyntax(QRegExp::Wildcard);

    if(tampleFileName.exactMatch(this->fileInfo.fileName())){
        pic = imread(this->fileInfo.filePath().toStdString(),0);

        QString fileName = this->fileInfo.fileName();
        fileName.chop(4);
        int toRemove = fileName.count() - 6;
        fileName.remove(0,toRemove);
        QString h = fileName;
        QString m = fileName;
        QString s = fileName;
        h.chop(4);
        m.remove(0,2);
        m.chop(2);
        s.remove(0,4);
        time = QTime(h.toInt(),m.toInt(),s.toInt());
    }
}

bool BlistPicture::operator ==(const BlistPicture &a)
{
    return a.time == this->time;
}

bool BlistPicture::operator !=(const BlistPicture &a)
{
    return a.time != this->time;
}

bool BlistPicture::operator <(const BlistPicture &a)
{
    return this->time < a.time;
}

bool BlistPicture::operator >(const BlistPicture &a)
{
    return this->time > a.time;
}

bool BlistPicture::operator <=(const BlistPicture &a)
{
    return this->time <= a.time;
}

bool BlistPicture::operator >=(const BlistPicture &a)
{
    return this->time >= a.time;
}

bool BlistPicture::operator <(const BlistPicture *a)
{
    return this->time < a->time;
}

void BlistPicture::calculateBrightness()
{
    if(startPicture != nullptr){
        if(startPicture->pic.data){
            Mat result;

            subtract(startPicture->pic,pic,result);

            brightnessDifference = 0;
            for(int i = 0;i < result.cols;i++){
                for(int j = 0;j < result.rows;j++){
                   uchar a = result.data[result.cols * j + i];
                   brightnessDifference += (a/255.0);
               }
            }

        }
    }
}

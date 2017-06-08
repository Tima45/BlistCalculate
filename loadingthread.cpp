#include "loadingthread.h"

LoadingThread::LoadingThread(QDir dir, QVector<BlistPicture*> *pictures, QVector<QTime> *times, QVector<double> *current, Rect ROI, bool loadExel, QObject *parent) : QThread(parent)
{
    this->dir = dir;
    this->pictures = pictures;
    this->loadExel = loadExel;

    this->times = times;
    this->current = current;
    this->ROI = ROI;
}

void LoadingThread::run()
{

    loadBlistPictures(dir);
    if(loadExel){
        loadExels(dir);
    }
    calculateDifferense();
}

void LoadingThread::loadBlistPictures(QDir dir)
{

    for(int i = 0; i < pictures->count(); i++){
        pictures->at(i)->deleteLater();
    }
    pictures->clear();

    emit loadingStatus("Загрузка картинок...");
    double currentProgress = 0;

    QRegExp tampleFileName("Image_0_*.jpg");
    tampleFileName.setPatternSyntax(QRegExp::Wildcard);

    QFileInfoList list = dir.entryInfoList();
    double delta = 100.0/list.count();
    for(int i = 0; i < list.count(); i++){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && tampleFileName.exactMatch(list.at(i).fileName())){
            BlistPicture *newBlistPicture = new BlistPicture(list.at(i));
            pictures->append(newBlistPicture);
            currentProgress +=delta;
            emit setProgress((int)currentProgress);
        }
    }
    emit setProgress(0);
}

template<class T>
bool dereferencedLessThan(T * o1, T * o2) {
    return *o1 < *o2;
}

void LoadingThread::calculateDifferense()
{
    emit loadingStatus("Вычитание картинок...");
    double currentProgress = 0;
    double delta = 100.0/pictures->count();

    if(!pictures->isEmpty()){
        std::sort(pictures->begin(),pictures->end(),dereferencedLessThan<BlistPicture>);
        for(int i = 0; i < pictures->count(); i++){
            if(ROI.width != 0 && ROI.height != 0 && ROI.x+ROI.width < pictures->at(i)->pic.cols && ROI.y+ROI.height < pictures->at(i)->pic.rows){
                pictures->at(i)->pic = pictures->at(i)->pic(ROI);
            }

            pictures->at(i)->startPicture = pictures->at(0);
            pictures->at(i)->calculateBrightness();
            currentProgress +=delta;
            emit setProgress((int)currentProgress);
        }
    }
    emit setProgress(0);
}

void LoadingThread::loadExels(QDir dir)
{
    emit loadingStatus("Загрузка экселек...");
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();
    times->clear();
    current->clear();
    double currentProgress = 0;

    int c = 0;
    for(int i = 0; i <list.count() ; i++){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && list.at(i).fileName().endsWith(".xls")){
            c++;
        }
    }

    double delta = 100.0/c;
    for(int i = list.count()-1; i >= 0 ; i--){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && list.at(i).fileName().endsWith(".xls")){
            QFile f(list.at(i).filePath());
            if(f.open(QIODevice::ReadOnly)){
                    QByteArray all = f.readAll();
                    QByteArrayList lines = all.split(0x0d);
                    for(int l = 0; l < lines.count(); l++){
                        QByteArrayList cells = lines[l].split(0x09);
                        if(cells.count() >= 17){
                            double current1 = cells[17].toDouble();
                            QByteArrayList timeList = cells[0].split(':');
                            if(timeList.count() >= 3){
                                QTime time(timeList[0].toDouble(),timeList[1].toDouble(),timeList[2].toDouble());
                                if(current1 > 0){
                                    current->append(current1);
                                    times->append(time);

                                }
                            }
                        }
                    }
                    f.close();
                    currentProgress += delta;
                    emit setProgress((int)currentProgress);
            }
        }
    }
    emit setProgress(0);
}

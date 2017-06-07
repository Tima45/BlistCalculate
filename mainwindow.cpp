#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tampleFileName = QRegExp("Image_0_*.jpg");
    tampleFileName.setPatternSyntax(QRegExp::Wildcard);
    initPlot();
}

void MainWindow::processPictures(QString path)
{
    QDir dir(path);
    loadBlistPictures(dir);
    if(ui->checkBox->isChecked()){
        loadExels(dir);
    }
    calculateDifferense();

    drawDifference();
}

void MainWindow::loadBlistPictures(QDir dir)
{
    for(int i = 0; i < blistPictures.count(); i++){
        blistPictures.at(i)->deleteLater();
    }
    blistPictures.clear();

    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && tampleFileName.exactMatch(list.at(i).fileName())){
            BlistPicture *newBlistPicture = new BlistPicture(list.at(i));
            blistPictures.append(newBlistPicture);
        }
    }

}
template<class T>
bool dereferencedLessThan(T * o1, T * o2) {
    return *o1 < *o2;
}
void MainWindow::calculateDifferense()
{
    if(!blistPictures.isEmpty()){
        std::sort(blistPictures.begin(),blistPictures.end(),dereferencedLessThan<BlistPicture>);
        for(int i = 0; i < blistPictures.count(); i++){
            blistPictures.at(i)->startPicture = blistPictures[0];
            blistPictures.at(i)->calculateBrightness();
        }
    }
}

void MainWindow::loadExels(QDir dir)
{
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();
    times.clear();
    current.clear();
    double testmAh = 0;
    for(int i = list.count()-1; i >= 0 ; i--){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && list.at(i).fileName().endsWith(".xls")){

            ui->progressBar->setValue(rand()%100);

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
                                    current.append(current1);
                                    times.append(time);

                                }
                            }
                        }
                    }


                    f.close();
            }
        }
    }
}

double MainWindow::calculateIncfluensTo(QTime time)
{
    double mAh = 0;
    if(!times.isEmpty() && !current.isEmpty()){
        if(time < times.first()){
            return 0;
        }
        for(int i = 0; i < times.count()-1; i++){
                mAh += current.at(i);
            if(times.at(i) <= time && times.at(i+1) >= time){
                mAh += current.at(i);
                break;
            }
        }
        mAh /= (3600*1000);
    }
    return mAh;
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPlot()
{
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal);



    ui->plot->yAxis->setLabel("Разница ярости");
    standartTicker = ui->plot->xAxis->ticker();
}

void MainWindow::drawDifference()
{
    if(!blistPictures.isEmpty()){
        QVector<double> y,x;
        for(int i = 0; i < blistPictures.count(); i++){
            y.append(blistPictures.at(i)->brightnessDifference);
            if(ui->checkBox->isChecked()){
                x.append(calculateIncfluensTo(blistPictures.at(i)->time));
            }else{
                x.append(QTime(0,0,0).secsTo(blistPictures.at(i)->time));
            }
        }
        ui->plot->clearGraphs();

        if(!ui->checkBox->isChecked()){
            QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
            timeTicker->setTimeFormat("%h:%m:%s");
            ui->plot->xAxis->setTicker(timeTicker);
            ui->plot->xAxis->setLabel("t");
        }else{
            ui->plot->xAxis->setTicker(standartTicker);
            ui->plot->xAxis->setLabel("mAh");
        }
        QCPGraph *diffGraph = ui->plot->addGraph();
        diffGraph->setPen(QPen(QColor(Qt::blue)));
        diffGraph->setData(x,y);

        ui->plot->rescaleAxes();
        ui->plot->replot();
        ui->progressBar->setValue(0);

        for(int i = 0; i < blistPictures.count(); i++){
            blistPictures.at(i)->deleteLater();
        }
        blistPictures.clear();
    }
}

void MainWindow::on_selectFolderButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Выбрать папку",ui->pathLabel->text());
    ui->pathLabel->setText(path);
    if(path != ""){

        processPictures(path);
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString s = QFileDialog::getSaveFileName(this,"Сохранить скриншот",ui->pathLabel->text(),"png");
    ui->plot->grab().save(s+".png","png");
}

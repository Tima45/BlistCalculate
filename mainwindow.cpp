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
        qSort(blistPictures.begin(),blistPictures.end(),dereferencedLessThan<BlistPicture>);
        for(int i = 0; i < blistPictures.count(); i++){
            blistPictures.at(i)->startPicture = blistPictures[0];
            blistPictures.at(i)->calculateBrightness();
        }
    }
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




    ui->plot->xAxis->setLabel("Время");
    ui->plot->yAxis->setLabel("Разница ярости");

}

void MainWindow::drawDifference()
{
    if(!blistPictures.isEmpty()){
        QVector<double> y,x;
        for(int i = 0; i < blistPictures.count(); i++){
            y.append(blistPictures.at(i)->brightnessDifference);
            x.append(QTime(0,0,0).secsTo(blistPictures.at(i)->time));
        }
        ui->plot->clearGraphs();

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        ui->plot->xAxis->setTicker(timeTicker);

        QCPGraph *diffGraph = ui->plot->addGraph();
        diffGraph->setPen(QPen(QColor(Qt::blue)));

        diffGraph->setData(x,y);
        ui->plot->rescaleAxes();
        ui->plot->replot();
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

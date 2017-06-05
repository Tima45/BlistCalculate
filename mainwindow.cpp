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
    QFileInfo firstFile = findFirstFile(dir);
    if(firstFile.fileName() != ""){
        startPicture = imread(firstFile.filePath().toStdString(),0);
    }
    calculateDifferense(dir);
    drawDifference();
}

void MainWindow::calculateDifferense(QDir dir)
{
    if(startPicture.data){
        pictureDifference.clear();

        QFileInfoList list = dir.entryInfoList();
        for(int i = 0; i < list.count(); i++){
            if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && tampleFileName.exactMatch(list.at(i).fileName())){
                Mat pic = imread(list.at(i).filePath().toStdString(),0);
                if(pic.data){
                    pictureDifference.append(diffPicPic(startPicture,pic));
                }
            }
        }
    }
}

double MainWindow::calculateIntegralBright(Mat pic)
{
    double value = 0;
    for(int i = 0;i < pic.cols;i++){
        for(int j = 0;j < pic.rows;j++){
                uchar a = pic.data[pic.cols * j + i];
                value += a;
            }
      }
    return value;
}

QFileInfo MainWindow::findFirstFile(QDir dir)
{
    int minimumTimeIndex = -1;
    QTime minimumTime(23,59,59);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && tampleFileName.exactMatch(list.at(i).fileName())){
            QString fileName = list.at(i).fileName();
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
            QTime t(h.toInt(),m.toInt(),s.toInt());
            if(t < minimumTime){
                minimumTime = t;
                minimumTimeIndex = i;
            }

        }
    }
    if(minimumTimeIndex != -1){
        return list.at(minimumTimeIndex);
    }
    return QFileInfo();
}

double MainWindow::diffPicPic(Mat pic1, Mat pic2)
{
    Mat result;
    subtract(pic1,pic2,result);
    return calculateIntegralBright(result);
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

    diffGraph = ui->plot->addGraph();
    diffGraph->setPen(QPen(QColor(Qt::blue)));
}

void MainWindow::drawDifference()
{
    if(!pictureDifference.isEmpty()){
        QVector<double> x;
        for(int i = 0; i < pictureDifference.count(); i++){
            x.append(i);
        }
        diffGraph->clearData();
        diffGraph->setData(x,pictureDifference);
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

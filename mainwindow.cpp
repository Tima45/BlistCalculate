#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlot();
}

void MainWindow::processPictures(QString path)
{
    QDir dir(path);
    Rect ROI(ui->xBox->value(),ui->yBox->value(),ui->wBox->value(),ui->hBox->value());
    loadingThread = new LoadingThread(dir,&blistPictures,&times,&current,ROI,ui->checkBox->isChecked());
    connect(loadingThread,SIGNAL(loadingStatus(QString)),this,SLOT(loadingStatusChanged(QString)));
    connect(loadingThread,SIGNAL(finished()),this,SLOT(loadingEnded()));
    connect(loadingThread,SIGNAL(setProgress(int)),ui->progressBar,SLOT(setValue(int)));
    loadingThread->start();
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

void MainWindow::deletePictures()
{
    ui->statusLabel->setText("Удаление картинок...");
    for(int i = 0; i < blistPictures.count(); i++){
        delete blistPictures[i];
    }
    blistPictures.clear();
    ui->statusLabel->setText("");
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
    ui->statusLabel->setText("Отрисовка...");
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
            ui->plot->xAxis->setLabel("Время");
        }else{
            ui->plot->xAxis->setTicker(standartTicker);
            ui->plot->xAxis->setLabel("Плотность тока(mA/h)");
        }
        QCPGraph *diffGraph = ui->plot->addGraph();
        diffGraph->setPen(QPen(QColor(Qt::blue)));
        diffGraph->setData(x,y);

        ui->plot->rescaleAxes();
        ui->plot->replot();
        ui->progressBar->setValue(0);

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

void MainWindow::loadingStatusChanged(QString text)
{
    ui->statusLabel->setText(text);
}

void MainWindow::loadingEnded()
{
    drawDifference();
    deletePictures();
    loadingThread->deleteLater();
}

void MainWindow::on_pushButton_clicked()
{
    QString s = QFileDialog::getSaveFileName(this,"Сохранить скриншот",ui->pathLabel->text(),"png");
    ui->plot->grab().save(s+".png","png");
}

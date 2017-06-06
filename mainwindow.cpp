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
        qSort(blistPictures.begin(),blistPictures.end(),dereferencedLessThan<BlistPicture>);
        for(int i = 1; i < blistPictures.count(); i++){
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
    for(int i = list.count()-1; i >= 0 ; i--){
        if(list.at(i).fileName() != "." && list.at(i).fileName() != ".." && list.at(i).fileName().endsWith(".xls")){
            ui->progressBar->setValue(rand()%100);
            QAxObject* excel = new QAxObject("Excel.Application", 0);
            QAxObject* workbooks = excel->querySubObject("Workbooks");
            QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", list.at(i).filePath());
            QAxObject* sheets = workbook->querySubObject("Worksheets");
            QAxObject* sheet = sheets->querySubObject("Item(int)", 1);

            QAxObject* usedRange = sheet->querySubObject("UsedRange");
            QAxObject* rows = usedRange->querySubObject("Rows");
            int countRows = rows->property("Count").toInt();

            for(int j = 1; j < countRows; j++){
                QAxObject* cellTime = sheet->querySubObject("Cells(int,int)",j+1,1);
                int time = (cellTime->property("Value").toDouble()*1440.0)*60*1000;


                QAxObject* cellCurrent = sheet->querySubObject("Cells(int,int)",j+1,18);
                double current1 = cellCurrent->property("Value").toDouble();
                if(current1 > 100){
                    current.append(current1);
                    times.append(QTime::fromMSecsSinceStartOfDay(time));
                }
            }
            ui->progressBar->setValue(rand()%100);

            workbook->dynamicCall("Close()");
            excel->dynamicCall("Quit()");
            excel->deleteLater();
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

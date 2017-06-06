#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include "plot/qcustomplot.h"
#include <cv.hpp>
#include <QTime>
#include <QRegExp>
#include "blistpicture.h"



namespace Ui {
class MainWindow;
}
using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void processPictures(QString path);
    void loadBlistPictures(QDir dir);
    void calculateDifferense();



    void initPlot();
    void drawDifference();
private slots:
    void on_selectFolderButton_clicked();

private:
    Ui::MainWindow *ui;
    QVector<BlistPicture*> blistPictures;


    QRegExp tampleFileName;
    Mat startPicture;

    QVector<double> pictureDifference;
};

#endif // MAINWINDOW_H

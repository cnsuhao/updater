#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadworker.h"
#include "aria2/src/includes/aria2/aria2.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(startUpdate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startUpdate(void)
{
    ui->textBrowser->append("Starting update\n");
    DownloadWorker* worker = new DownloadWorker();
    worker->addUri("http://cdn.unvanquished.net/current.torrent");
    worker->moveToThread(&thread);
    connect(&thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(onDownloadEvent(int)), this, SLOT(onDownloadEvent(int)));
    connect(worker, SIGNAL(downloadSpeedChanged(int)), this, SLOT(setDownloadSpeed(int)));
    connect(worker, SIGNAL(uploadSpeedChanged(int)), this, SLOT(setUploadSpeed(int)));
    connect(worker, SIGNAL(totalSizeChanged(int)), this, SLOT(setTotalSize(int)));
    connect(worker, SIGNAL(completedSizeChanged(int)), this, SLOT(setCompletedSize(int)));
    connect(&thread, SIGNAL(started()), worker, SLOT(download()));
    thread.start();
}

void MainWindow::setDownloadSpeed(int speed)
{
    ui->downloadSpeed->setText(sizeToString(speed) + "/sec");
}

void MainWindow::setUploadSpeed(int speed)
{
    ui->uploadSpeed->setText(sizeToString(speed) + "/sec");
}

void MainWindow::setTotalSize(int size)
{
    ui->totalSize->setText(sizeToString(size));
}

void MainWindow::setCompletedSize(int size)
{
    ui->completedSize->setText(sizeToString(size));
}

void MainWindow::onDownloadEvent(int event)
{
    if (event == aria2::EVENT_ON_DOWNLOAD_START) {
        ui->textBrowser->append("Download started\n");
        return;
    }
    if (event == aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE) {
        ui->textBrowser->append("yay\n");
        thread.quit();
        thread.wait();
    }
}

QString MainWindow::sizeToString(int size)
{
    static QString sizes[] = { "Bytes", "KiB", "MiB", "GiB" };
    const int num_sizes = 4;
    int i = 0;
    while (size > 1024 && i++ < 4) {
        size /= 1024;
    }

    return QString::number(size) + " " + sizes[std::min(i, num_sizes - 1)];
}

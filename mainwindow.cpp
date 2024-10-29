#include "mainwindow.h"
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QtCore/QCoreApplication>
#include "JlCompress.h"

MainWindow::MainWindow()
{
    // 设置窗口大小
    setFixedSize(680, 500);

    // 创建用于提示拖拽的标签
    dragLabel = new QLabel(this);
    dragLabel->setText("请将ZIP文件拖拽到此区域");
    dragLabel->setAlignment(Qt::AlignCenter);
    dragLabel->setGeometry(0, 0, width(), height()*0.3);

    // 创建日志输出控件
    logTextEdit = new QTextEdit(this);
    logTextEdit->setGeometry(0, dragLabel->geometry().height(), width(), height()*0.7);
    logTextEdit->setReadOnly(true);

    // 启用拖放功能
    setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().at(0).isLocalFile() && event->mimeData()->urls().at(0).toLocalFile().endsWith(".zip")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString filePath = urlList.at(0).toLocalFile();
        // 处理ZIP文件路径
        installFromZip(filePath);
    }
}

void MainWindow::installFromZip(const QString &zipFilePath)
{
    logTextEdit->clear();

    // 获取临时目录路径用于解压文件
    QString tempDirPath = QDir::tempPath() + "/zip_extract";
    QDir dir(tempDirPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }
    dir.mkpath(tempDirPath);
    logTextEdit->append("创建临时解压目录: " + tempDirPath);

    // 解压ZIP文件
    QString fileNameWithSuffix = QFileInfo(zipFilePath).fileName();
    QString fileName = fileNameWithSuffix.left(fileNameWithSuffix.lastIndexOf('.'));
    logTextEdit->append("解压文件目录名称: " + fileName);

    // 解压文件
    logTextEdit->append("开始解压文件: " + zipFilePath);
    JlCompress::extractDir(zipFilePath, tempDirPath);
    logTextEdit->append("解压成功");

    // 执行脚本
    executeInstallScript(tempDirPath + "/" + fileName);

    // 删除文件
    if (dir.exists()) {
        dir.removeRecursively();
    }
}

void MainWindow::executeInstallScript(const QString& filePath)
{
    QProcess process;
    connect(&process, &QProcess::readyReadStandardOutput, this, [&]() {
        QByteArray output = process.readAllStandardOutput();
        logTextEdit->append(QString::fromUtf8(output));
    });
    connect(&process, &QProcess::readyReadStandardError, this, [&]() {
        QByteArray errorOutput = process.readAllStandardError();
        logTextEdit->append(QString("Error: ") + QString::fromUtf8(errorOutput));
    });

    QString scriptPath = QCoreApplication::applicationDirPath() + "/hdc_cli/install.sh";

    logTextEdit->append("执行脚本: " + scriptPath);
    process.start(scriptPath, QStringList() << filePath);

    if (!process.waitForStarted()) {
        qDebug() << "Failed to start the script.";
        return;
    }

    if (!process.waitForFinished()) {
        qDebug() << "Script execution timed out.";
        return;
    }

    QByteArray finalOutput = process.readAllStandardOutput();
    QByteArray finalErrorOutput = process.readAllStandardError();

    logTextEdit->append(QString::fromUtf8(finalOutput));
    logTextEdit->append(QString("Error: ") + QString::fromUtf8(finalErrorOutput));
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QProcess>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QLabel *dragLabel;
    QTextEdit *logTextEdit;
    void installFromZip(const QString &zipFilePath);
    void executeInstallScript(const QString& filePath);
};

#endif // MAINWINDOW_H

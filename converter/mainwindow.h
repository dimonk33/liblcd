#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QByteArray;
class QCheckBox;
class QFontDialog;
class QLabel;
class QLineEdit;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void btnExportClicked();
    void aboutClicked();

private:
    void createGui();
    QString renderGlyph(const QString &utf8, const QImage &image);
    QFontDialog *fontSelector;

    QLabel *lblDisplay;

    QCheckBox *chkUpperAZ;
    QCheckBox *chkLowerAZ;
    QCheckBox *chkNumbers;
    QLineEdit *ldtCharacters;

};

#endif // MAINWINDOW_H

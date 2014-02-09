/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UVC Ingenieure http://uvc.de/
 * Author: Max Holtzberg <mh@uvc.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QtWidgets>

#include "mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    createGui();
}

void MainWindow::createGui()
{
    setWindowIcon(QIcon(":/images/icon.png"));
    setWindowTitle(tr("UVC Font Converter"));

    menuBar()->addAction(tr("&About"), this, SLOT(aboutClicked()));

    fontSelector = new QFontDialog();
    fontSelector->setCurrentFont(QFont("Arial", 12));
    fontSelector->setOptions(QFontDialog::NoButtons);

    chkLowerAZ = new QCheckBox("a-z");
    chkUpperAZ = new QCheckBox("A-Z");
    chkNumbers = new QCheckBox("0-9");
    ldtCharacters = new QLineEdit();
    ldtCharacters->setText("a");
    lblDisplay = new QLabel();

    QGridLayout *ltCharset = new QGridLayout();
    ltCharset->addWidget(chkLowerAZ, 0, 0);
    ltCharset->addWidget(chkUpperAZ, 1, 0);
    ltCharset->addWidget(chkNumbers, 2, 0);
    ltCharset->addWidget(new QLabel(tr("Custom Characters")), 3, 0);
    ltCharset->addWidget(ldtCharacters, 3, 1);
    ltCharset->addWidget(lblDisplay, 4, 0);

    QGroupBox *grpCharset = new QGroupBox(tr("Character Set"));
    grpCharset->setLayout(ltCharset);

    QHBoxLayout *ltButtons = new QHBoxLayout();
    QPushButton *btnExport = new QPushButton(tr("Export"));
    connect(btnExport, SIGNAL(clicked()),
            this, SLOT(btnExportClicked()));
    ltButtons->addStretch();
    ltButtons->addWidget(btnExport);

    QVBoxLayout *ltMain = new QVBoxLayout();
    ltMain->addWidget(fontSelector);
    ltMain->addWidget(grpCharset);
    ltMain->addLayout(ltButtons);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(ltMain);
    setCentralWidget(mainWidget);
}

QString MainWindow::renderGlyph(const QString &utf8, const QImage &image)
{
    QByteArray bitmap;
    int counter = 0;
    uchar b = 0;

    QString glyph = QString(
            "static const struct glib_bitmap glyph_%1 = {\n"
            "    .width  = %2,\n"
            "    .height = %3,\n"
            "    .data   = {\n        ")
            .arg(utf8)
            .arg(image.width())
            .arg(image.height());

    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {

            b <<= 1;
            b |= !image.pixelIndex(x, y);

            if (++counter % 8 == 0) {
                bitmap.append(b);
                b = 0;
            }
        }
        printf("\n");
    }

    // Append last byte if necessary
    if (counter % 8 != 0) {
        // Remove the gap
        b <<= 8 - (counter % 8);
        bitmap.append(b);
    }

    bitmap = bitmap.toHex();

    for (int i = 0; i < bitmap.size(); i += 2) {
        glyph.append(QString("0x%1%2")
                     .arg(bitmap[i])
                     .arg(bitmap[i+1]));

        if (i < (bitmap.size() - 2)) {
            if ((i + 2) % 16 == 0)
                glyph.append(",\n        ");
            else
                glyph.append(", ");
        }

    }

    glyph += "\n   }\n};\n\n";

    return glyph;
}

void MainWindow::btnExportClicked()
{
    QList<QChar> charset;

    if (chkLowerAZ->isChecked())
        for (char c = 'a'; c <= 'z'; c++)
            charset.append(c);

    if (chkUpperAZ->isChecked())
        for (char c = 'A'; c <= 'Z'; c++)
            charset.append(c);

    if (chkNumbers->isChecked())
        for (char c = '0'; c <= '9'; c++)
            charset.append(c);

    QString str = ldtCharacters->text();
    for (QString::const_iterator itr(str.begin()); itr != str.end(); itr++)
        if (!charset.contains(*itr) && *itr != ' ')
            charset.append(*itr);

    qSort(charset);

    // Render characters
    QFont font = fontSelector->currentFont();
    QFontMetrics metrics = QFontMetrics(font);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");

    qDebug() << "charset:" << charset << font.family();

    QString glyphs;
    QString fontName = QString("font_%1%2")
            .arg(font.family().toLower())
            .arg(font.pointSize())
            .replace(' ', '_');

    QString fontstruct = QString(
            "const struct glib_font %1 = {\n"
            "    .charcount = %2,\n"
            "    .glyphs    = {\n        ")
            .arg(fontName)
            .arg(charset.size());

    QListIterator<QChar> itr(charset);
    while (itr.hasNext()) {
        QChar c = itr.next();

        QRect boundingRect = metrics.boundingRect(c);

        QImage image = QImage(boundingRect.width(), boundingRect.height(), QImage::Format_Mono);
        // QImage image = QImage(100, 100, QImage::Format_Mono);

        image.fill(Qt::color1);

        QPainter p;
        p.begin(&image);
        p.setFont(font);
        p.setWindow(metrics.boundingRect(c));
        p.drawText(0, 0, c);
        p.end();

        QString utf8 = codec->fromUnicode(c).toHex();

        glyphs += renderGlyph(utf8, image);
        fontstruct += QString("{.utf8 = 0x%1, .x = %2, .y = %3, .bitmap = &glyph_%1}")
                .arg(utf8)
                .arg(boundingRect.x())
                .arg(boundingRect.y());


        fontstruct += ",\n        ";
    }

    // Add space character
    fontstruct += QString("{.utf8 = 0x20, .x = %1, .y = 0, .bitmap = NULL}")
            .arg(metrics.width(' '));


    fontstruct += "\n    }\n};\n";

    glyphs = "#include \"glib.h\"\n\n" + glyphs + "\n" + fontstruct;


    QString filename = QFileDialog::getSaveFileName(this,
            tr("Save Font"), fontName + ".c", tr("Source Files (*.c *.cpp)"));

    if (!filename.isEmpty()) {
        if (!(filename.endsWith(".c") || filename.endsWith(".cpp")))
            filename += ".c";

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << glyphs;
        file.close();
    }
}

void MainWindow::aboutClicked()
{
    AboutDialog dlg;
    dlg.exec();
}

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

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowIcon(QIcon(":/images/uvc.ico"));
    setWindowTitle(tr("About UVC Font Converter"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    const QString description = tr(
        "<h3>UVC Font Converter</h3>"
        "Simple tool to convert system fonts to C embeddable source files."
        "<br/>"
        "<br/>"
        "Copyright UVC Ingenieure <a href='http://uvc.de/'>http://uvc.de/</a>"
        "<br/>"
        "<br/>");

    QLabel *lblAbout = new QLabel(description);
    lblAbout->setWordWrap(true);
    lblAbout->setOpenExternalLinks(true);
    lblAbout->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton *closeButton = buttonBox->button(QDialogButtonBox::Close);
    buttonBox->addButton(closeButton, QDialogButtonBox::ButtonRole(
                             QDialogButtonBox::RejectRole | QDialogButtonBox::AcceptRole));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *ltMain = new QVBoxLayout();
    ltMain->addWidget(lblAbout);
    ltMain->addWidget(buttonBox);

    setLayout(ltMain);
}

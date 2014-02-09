#include <QtWidgets>


#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowIcon(QIcon(":/images/icon.png"));
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

/********************************************************************************
** Form generated from reading UI file 'qcan_socket_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef QCAN_SOCKET_DIALOG_H
#define QCAN_SOCKET_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_QCanSocketDialog
{
public:
    QDialogButtonBox *pclButtonBox;
    QGroupBox *pclGroupBoxM;
    QLabel *pclLblHostM;
    QLabel *pclLblCanChannelM;
    QLabel *pclLblCanDetailM;
    QLabel *pclLblHostAddressM;
    QComboBox *pclCbxCanChannelM;
    QLineEdit *pclEdtHostAddressM;
    QComboBox *pclCbxHostM;
    QLabel *pclLblCanInfoM;

    void setupUi(QDialog *QCanSocketDialog)
    {
        if (QCanSocketDialog->objectName().isEmpty())
            QCanSocketDialog->setObjectName(QStringLiteral("QCanSocketDialog"));
        QCanSocketDialog->resize(380, 251);
        pclButtonBox = new QDialogButtonBox(QCanSocketDialog);
        pclButtonBox->setObjectName(QStringLiteral("pclButtonBox"));
        pclButtonBox->setGeometry(QRect(10, 210, 361, 32));
        pclButtonBox->setOrientation(Qt::Horizontal);
        pclButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        pclGroupBoxM = new QGroupBox(QCanSocketDialog);
        pclGroupBoxM->setObjectName(QStringLiteral("pclGroupBoxM"));
        pclGroupBoxM->setGeometry(QRect(10, 10, 361, 191));
        pclLblHostM = new QLabel(pclGroupBoxM);
        pclLblHostM->setObjectName(QStringLiteral("pclLblHostM"));
        pclLblHostM->setGeometry(QRect(10, 10, 140, 20));
        pclLblHostM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pclLblCanChannelM = new QLabel(pclGroupBoxM);
        pclLblCanChannelM->setObjectName(QStringLiteral("pclLblCanChannelM"));
        pclLblCanChannelM->setGeometry(QRect(10, 70, 140, 20));
        pclLblCanChannelM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pclLblCanDetailM = new QLabel(pclGroupBoxM);
        pclLblCanDetailM->setObjectName(QStringLiteral("pclLblCanDetailM"));
        pclLblCanDetailM->setGeometry(QRect(10, 100, 140, 20));
        pclLblCanDetailM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pclLblHostAddressM = new QLabel(pclGroupBoxM);
        pclLblHostAddressM->setObjectName(QStringLiteral("pclLblHostAddressM"));
        pclLblHostAddressM->setGeometry(QRect(10, 40, 140, 20));
        pclLblHostAddressM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pclCbxCanChannelM = new QComboBox(pclGroupBoxM);
        pclCbxCanChannelM->setObjectName(QStringLiteral("pclCbxCanChannelM"));
        pclCbxCanChannelM->setGeometry(QRect(170, 70, 150, 26));
        pclEdtHostAddressM = new QLineEdit(pclGroupBoxM);
        pclEdtHostAddressM->setObjectName(QStringLiteral("pclEdtHostAddressM"));
        pclEdtHostAddressM->setGeometry(QRect(170, 42, 145, 21));
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        pclEdtHostAddressM->setFont(font);
        pclEdtHostAddressM->setAlignment(Qt::AlignCenter);
        pclCbxHostM = new QComboBox(pclGroupBoxM);
        pclCbxHostM->setObjectName(QStringLiteral("pclCbxHostM"));
        pclCbxHostM->setGeometry(QRect(170, 10, 150, 26));
        pclLblCanInfoM = new QLabel(pclGroupBoxM);
        pclLblCanInfoM->setObjectName(QStringLiteral("pclLblCanInfoM"));
        pclLblCanInfoM->setGeometry(QRect(170, 102, 148, 60));
        pclLblCanInfoM->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        QWidget::setTabOrder(pclCbxHostM, pclEdtHostAddressM);
        QWidget::setTabOrder(pclEdtHostAddressM, pclCbxCanChannelM);

        retranslateUi(QCanSocketDialog);
        QObject::connect(pclButtonBox, SIGNAL(accepted()), QCanSocketDialog, SLOT(accept()));
        QObject::connect(pclButtonBox, SIGNAL(rejected()), QCanSocketDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QCanSocketDialog);
    } // setupUi

    void retranslateUi(QDialog *QCanSocketDialog)
    {
        QCanSocketDialog->setWindowTitle(QApplication::translate("QCanSocketDialog", "Connect to CAN interface", 0));
        pclGroupBoxM->setTitle(QString());
        pclLblHostM->setText(QApplication::translate("QCanSocketDialog", "Connect to:", 0));
        pclLblCanChannelM->setText(QApplication::translate("QCanSocketDialog", "CAN channel:", 0));
        pclLblCanDetailM->setText(QApplication::translate("QCanSocketDialog", "CAN details:", 0));
        pclLblHostAddressM->setText(QApplication::translate("QCanSocketDialog", "IPv4-Address:", 0));
        pclCbxCanChannelM->clear();
        pclCbxCanChannelM->insertItems(0, QStringList()
         << QApplication::translate("QCanSocketDialog", "CAN 1", 0)
         << QApplication::translate("QCanSocketDialog", "CAN 2", 0)
         << QApplication::translate("QCanSocketDialog", "CAN 3", 0)
         << QApplication::translate("QCanSocketDialog", "CAN 4", 0)
        );
        pclEdtHostAddressM->setInputMask(QApplication::translate("QCanSocketDialog", "000.000.000.000", 0));
        pclCbxHostM->clear();
        pclCbxHostM->insertItems(0, QStringList()
         << QApplication::translate("QCanSocketDialog", "Local host", 0)
         << QApplication::translate("QCanSocketDialog", "Remote server", 0)
        );
        pclLblCanInfoM->setText(QApplication::translate("QCanSocketDialog", "CAN details", 0));
    } // retranslateUi

};

namespace Ui {
    class QCanSocketDialog: public Ui_QCanSocketDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // QCAN_SOCKET_DIALOG_H

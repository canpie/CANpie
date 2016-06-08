//============================================================================//
// File:          qcan_qui.cpp                                                //
// Description:   GUI to handle with plugins and test them                    //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//


#include <QtWidgets>
#include <qcan_gui.hpp>

//! [0]
QCanGui::QCanGui()
{
    createGUI();
    setLayout(layout);
    setWindowTitle("Echo Plugin Example");

    if (!loadPlugin()) {
        QMessageBox::information(this, "Error", "Could not load the plugin");
        lineEdit->setEnabled(false);
        button->setEnabled(false);
    }
}
//! [0]

//! [1]
void QCanGui::sendEcho()
{
    QString text = qCanInterface->echo(lineEdit->text());
    label->setText(text);
}
//! [1]

void QCanGui::receiveError(int32_t slErrorV)
{
//    label->setText("HI");
}


void QCanGui::changePlugin(int slIdxV)
{
    QPluginLoader pluginLoader(aclDirListP.at(slIdxV));
    QObject *plugin = pluginLoader.instance();
    if (plugin) {
        qCanInterface = qobject_cast<QCanInterface *>(plugin);
    }
}



//! [2]
void QCanGui::createGUI()
{
    lineEdit = new QLineEdit;
    label = new QLabel;
    comboBox = new QComboBox;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    button = new QPushButton(tr("Send Message"));

    connect(lineEdit, SIGNAL(editingFinished()),
            this, SLOT(sendEcho()));
    connect(button, SIGNAL(clicked()),
            this, SLOT(sendEcho()));

    layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Plugin:")), 0, 0);
    layout->addWidget(comboBox, 0, 1);
    layout->addWidget(new QLabel(tr("Message:")), 1, 0);
    layout->addWidget(lineEdit, 1, 1);
    layout->addWidget(new QLabel(tr("Answer:")), 2, 0);
    layout->addWidget(label, 2, 1);
    layout->addWidget(button, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
}
//! [2]

//! [3]
bool QCanGui::loadPlugin()
{
    QStringList clSelectItemListT;
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    clSelectItemListT.clear();
    aclDirListP.clear();
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader( pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            qCanInterface = qobject_cast<QCanInterface *>(plugin);
            if (qCanInterface)
            {
                clSelectItemListT.append(fileName);
                aclDirListP.append(pluginsDir.absoluteFilePath(fileName));
//                plugin->connect(plugin, SIGNAL(errorOccurred(int32_t) ),
//                                this,          SLOT(  receiveError(int32_t) ) );
//                connect(plugin, SIGNAL(errorOccurred() ),
//                        this,   SLOT(  receiveError() ) );


//                return true;
            }
        }
    }

    if (clSelectItemListT.isEmpty())
    {
        return false;
    }

    comboBox->addItems(clSelectItemListT);
    comboBox->setCurrentIndex(clSelectItemListT.count()-1);

    connect(comboBox, SIGNAL(activated(int)),
            this, SLOT(changePlugin(int)));

    return true;
}
//! [3]

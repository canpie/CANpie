

#include <QIcon>
#include <QWidget>
#include <QDir>
#include <QCanInterface>
#include <QCanPlugin>
#include <QMenu>

//-----------------------------------------------------------------------------
/*!
** \class CdtDeviceArea
** \brief Device area in main window
**
** This class defines the areas for selecting the device in the main window.
*/
class QCanInterfaceWidget : public QWidget
{
   Q_OBJECT

public:

   #define QCAN_IF_VCAN_NAME "Virtual CAN bus"
   #define QCAN_IF_VCAN_ICON ":images/mc_network_vcan_256.png"

   QCanInterfaceWidget(uint8_t ubIdxV);

   /*!
    * \brief setIcon
    * \param clIconR - Icon
    */
   void setIcon(QIcon clIconR);
   void setPluginPath(QDir);

   /*!
    * \brief setInterface
    * \param clNameV - Name of a plugin
    * \param ubChannelV - Channel number
    * \return Returns true if given plugin could be set
    */
   bool setInterface(QString clNameV);

   /*!
    * \brief pluginName
    * \return Name of acutaly used plugin
    */
   QString name(void);

//   /*!
//    * \brief pluginChannel
//    * \return Selected channel number of plugin
//    */
//   uint8_t pluginChannel(void);

private:

   uint8_t ubInterfaceIdxP;
   QIcon    clIconP;
   QDir     clPluginPathP;

   /*!
    * \brief qCanInterfaceP
    * This pointer points to the actual selected interface.
    * Otherwise this value is set to NULL.
    */
   QCanInterface *pclQCanInterfaceP;


   QList<QCanPlugin *> apclQCanPluginP;

protected:
   void mousePressEvent(QMouseEvent *event);
   void paintEvent(QPaintEvent *event);
   bool loadPlugin(void);
   //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
   //           QWidget *widget);

private slots:

public slots:



signals:
   void clicked(bool checked = false );
   void interfaceChanged(uint8_t ubIdxV, QCanInterface *pclIfV);

};

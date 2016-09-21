

#include <QIcon>
#include <QWidget>
#include <QDir>
#include <QCanInterface>

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
   #define QCAN_IF_VCAN_ICON ":images/network-vcan.png"
   QCanInterfaceWidget();

   /*!
    * \brief setIcon
    * \param clIconR - Icon
    */
   void setIcon(QIcon clIconR);
   void setPluginPath(QDir);

   /*!
    * \brief setPlugin
    * \param clNameV - Name of a plugin
    * \param ubChannelV - Channel number
    * \return Returns true if given plugin could be set
    */
   bool setPlugin(QString clNameV, uint8_t ubChannelV = 0);

   /*!
    * \brief pluginName
    * \return Name of acutaly used plugin
    */
   QString pluginName(void);

   /*!
    * \brief pluginChannel
    * \return Selected channel number of plugin
    */
   uint8_t pluginChannel(void);

private:

   QIcon    clIconP;
   QDir     clPluginPathP;

   /*!
    * \brief qCanInterfaceP
    * This pointer points to the actual selected interface.
    * Otherwise this value is set to 0.
    */
   QCanInterface *pclQCanInterfaceP;

   QList<QString> aclPluginNameListP;
   QList<QString> aclPluginListP;
   QList<QIcon>   aclIconListP;
   uint8_t        ubPluginChannelP = 0;

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
   void interfaceChanged(QCanInterface *pclIfV);

};

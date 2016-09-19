

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

   QCanInterfaceWidget();

   void setIcon(QIcon & clIconR);
   void setPluginPath(QDir);

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

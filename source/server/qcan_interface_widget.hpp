

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
   QCanInterface *qCanInterface;
   QStringList clPluginItemListP;
   QList<QString> aclPluginListP;

protected:
   void mousePressEvent(QMouseEvent *event);
   void paintEvent(QPaintEvent *event);
   bool loadPlugin(void);
   //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
   //           QWidget *widget);

private slots:


public slots:



signals:
   void  clicked(bool checked = false );

};

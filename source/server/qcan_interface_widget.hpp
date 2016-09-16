

#include <QIcon>
#include <QWidget>

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

   void     setIcon(QIcon & clIconR);

private:

   QIcon    clIconP;

protected:
   void mousePressEvent(QMouseEvent *event);
   void paintEvent(QPaintEvent *event);

   //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
   //           QWidget *widget);

private slots:


public slots:

signals:
   void  clicked(bool checked = false );

};

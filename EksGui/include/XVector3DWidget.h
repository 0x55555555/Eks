#ifndef VECTOR3DWIDGET_H
#define VECTOR3DWIDGET_H

#include "XGuiGlobal.h"
#include "QtHelper/XQWidget.h"
#include "Math/XMathVector.h"

class QVBoxLayout;
class XFloatWidget;

class EKSGUI_EXPORT XVector3DWidget : public QWidget
    {
Q_OBJECT
public:
    XVector3DWidget( QWidget *parent = 0, Eks::Vector3D val=Eks::Vector3D::Zero(), QStringList labels=QStringList());
    XVector3DWidget( Eks::Vector3D val, Eks::Vector3D min, Eks::Vector3D max, QStringList labels=QStringList(), QWidget *parent = 0);

    void setMaximum( Eks::Vector3D );
    Eks::Vector3D maximum() const;
    void setMinimum( Eks::Vector3D );
    Eks::Vector3D minimum() const;
    Eks::Vector3D range() const;
    Eks::Vector3D value() const;

    bool isReadOnly() const;

Q_SIGNALS:
    void valueChanged( Eks::Vector3D );

public Q_SLOTS:
    void setValue( Eks::Vector3D );
    void setReadOnly(bool);

protected:
    void keyPressEvent( QKeyEvent * );
    void keyReleaseEvent( QKeyEvent * );

private Q_SLOTS:
    void setValues();
private:
    XFloatWidget *_x, *_y, *_z;
    bool _setting;
    };

#endif // VECTORWIDGET_H

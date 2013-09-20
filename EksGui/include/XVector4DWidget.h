#ifndef VECTOR4DWIDGET_H
#define VECTOR4DWIDGET_H

#include "XGuiGlobal.h"
#include "QtHelper/XQWidget"
#include "XMathVector"

class QVBoxLayout;
class XFloatWidget;

class EKSGUI_EXPORT XVector4DWidget : public QWidget
    {
Q_OBJECT
public:
    XVector4DWidget( QWidget *parent = 0, const Eks::Vector4D &val=Eks::Vector4D::Zero(), QStringList labels=QStringList());
    XVector4DWidget( const Eks::Vector4D &val, const Eks::Vector4D &min, const Eks::Vector4D &max, QStringList labels=QStringList(), QWidget *parent = 0);

    void setMaximum( const Eks::Vector4D & );
    Eks::Vector4D maximum() const;
    void setMinimum( const Eks::Vector4D & );
    Eks::Vector4D minimum() const;
    Eks::Vector4D range() const;
    Eks::Vector4D value() const;

    bool isReadOnly() const;

Q_SIGNALS:
    void valueChanged( const Eks::Vector4D & );

public Q_SLOTS:
    void setValue( const Eks::Vector4D & );
    void setReadOnly(bool);

protected:
    void keyPressEvent( QKeyEvent * );
    void keyReleaseEvent( QKeyEvent * );

private Q_SLOTS:
    void setValues();
private:
    XFloatWidget *_x, *_y, *_z, *_w;
    bool _setting;
    };

#endif // VECTORWIDGET_H

#include "XVector3DWidget"
#include "QVBoxLayout"
#include "XFloatWidget"
#include "QLabel"

#include "cmath"

#include "QKeyEvent"

XVector3DWidget::XVector3DWidget( QWidget *parent, XVector3D val, QStringList labels) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), -HUGE_VAL, HUGE_VAL, this ) ),
        _y( new XFloatWidget( val.y(), -HUGE_VAL, HUGE_VAL, this ) ),
        _z( new XFloatWidget( val.z(), -HUGE_VAL, HUGE_VAL, this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 3 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        lLayout->addWidget( new QLabel( labels[2] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->addWidget( _z );
    _layout->setSpacing( 2 );

    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _z, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

XVector3DWidget::XVector3DWidget( XVector3D val, XVector3D min, XVector3D max, QStringList labels, QWidget *parent ) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), min.x(), max.x(), this ) ),
        _y( new XFloatWidget( val.y(), min.y(), max.y(), this ) ),
        _z( new XFloatWidget( val.z(), min.z(), max.z(), this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 3 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        lLayout->addWidget( new QLabel( labels[2] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->addWidget( _z );
    _layout->setSpacing( 2 );

    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _z, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

bool XVector3DWidget::isReadOnly() const
    {
    return _x->isReadOnly();
    }

void XVector3DWidget::setReadOnly(bool t)
    {
    _x->setReadOnly(t);
    _y->setReadOnly(t);
    _z->setReadOnly(t);
    }

void XVector3DWidget::setMaximum( XVector3D in )
    {
    _x->setMaximum( in.x() );
    _y->setMaximum( in.y() );
    _z->setMaximum( in.z() );
    }

XVector3D XVector3DWidget::maximum() const
    {
    return XVector3D( _x->maximum(), _y->maximum(), _z->maximum() );
    }

void XVector3DWidget::setMinimum( XVector3D in )
    {
    _x->setMinimum( in.x() );
    _y->setMinimum( in.y() );
    _z->setMinimum( in.z() );
    }

XVector3D XVector3DWidget::minimum() const
    {
    return XVector3D( _x->minimum(), _y->minimum(), _z->minimum() );
    }

XVector3D XVector3DWidget::range() const
    {
    return maximum() - minimum();
    }

XVector3D XVector3DWidget::value() const
    {
    return XVector3D( _x->value(), _y->value(), _z->value() );
    }

void XVector3DWidget::setValue( XVector3D in )
    {
    if( !_setting )
        {
        _setting = true;
        _x->setValue( in.x() );
        _y->setValue( in.y() );
        _z->setValue( in.z() );
        emit valueChanged( in );
        _setting = false;
        }
    }

void XVector3DWidget::setValues()
    {
    setValue( XVector3D( _x->value(), _y->value(),  _z->value() ) );
    }

void XVector3DWidget::keyPressEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        connect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        connect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        connect( _z, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

void XVector3DWidget::keyReleaseEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        disconnect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        disconnect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        disconnect( _z, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

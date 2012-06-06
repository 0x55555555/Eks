#include "XVector4DWidget"
#include "QVBoxLayout"
#include "XFloatWidget"
#include "QLabel"

#include "cmath"

#include "QKeyEvent"

XVector4DWidget::XVector4DWidget( QWidget *parent, const XVector4D &val, QStringList labels ) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), -HUGE_VAL, HUGE_VAL, this ) ),
        _y( new XFloatWidget( val.y(), -HUGE_VAL, HUGE_VAL, this ) ),
        _z( new XFloatWidget( val.z(), -HUGE_VAL, HUGE_VAL, this ) ),
        _w( new XFloatWidget( val.w(), -HUGE_VAL, HUGE_VAL, this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 4 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        lLayout->addWidget( new QLabel( labels[2] ) );
        lLayout->addWidget( new QLabel( labels[3] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->addWidget( _z );
    _layout->addWidget( _w );
    _layout->setSpacing( 2 );

    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _z, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _w, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

XVector4DWidget::XVector4DWidget( const XVector4D &val, const XVector4D &min, const XVector4D &max, QStringList labels, QWidget *parent ) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), min.x(), max.x(), this ) ),
        _y( new XFloatWidget( val.y(), min.y(), max.y(), this ) ),
        _z( new XFloatWidget( val.z(), min.z(), max.z(), this ) ),
        _w( new XFloatWidget( val.w(), min.w(), max.w(), this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 4 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        lLayout->addWidget( new QLabel( labels[2] ) );
        lLayout->addWidget( new QLabel( labels[3] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->addWidget( _z );
    _layout->addWidget( _w );
    _layout->setSpacing( 2 );


    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _z, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _w, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

bool XVector4DWidget::isReadOnly() const
    {
    return _x->isReadOnly();
    }

void XVector4DWidget::setReadOnly(bool t)
    {
    _x->setReadOnly(t);
    _y->setReadOnly(t);
    _z->setReadOnly(t);
    _w->setReadOnly(t);
    }

void XVector4DWidget::setMaximum( const XVector4D &in )
    {
    _x->setMaximum( in.x() );
    _y->setMaximum( in.y() );
    _z->setMaximum( in.z() );
    _w->setMaximum( in.w() );
    }

XVector4D XVector4DWidget::maximum() const
    {
    return XVector4D( _x->maximum(), _y->maximum(), _z->maximum(), _w->maximum() );
    }

void XVector4DWidget::setMinimum( const XVector4D &in )
    {
    _x->setMinimum( in.x() );
    _y->setMinimum( in.y() );
    _z->setMinimum( in.z() );
    _w->setMinimum( in.w() );
    }

XVector4D XVector4DWidget::minimum() const
    {
    return XVector4D( _x->minimum(), _y->minimum(), _z->minimum(), _w->minimum() );
    }

XVector4D XVector4DWidget::range() const
    {
    return maximum() - minimum();
    }

XVector4D XVector4DWidget::value() const
    {
    return XVector4D( _x->value(), _y->value(), _z->value(), _w->value() );
    }

void XVector4DWidget::setValue( const XVector4D &in )
    {
    if( !_setting )
        {
        _setting = true;
        _x->setValue( in.x() );
        _y->setValue( in.y() );
        _z->setValue( in.z() );
        _w->setValue( in.w() );
        emit valueChanged( in );
        _setting = false;
        }
    }

void XVector4DWidget::setValues()
    {
    setValue( XVector4D( _x->value(), _y->value(), _z->value(), _w->value() ) );
    }

void XVector4DWidget::keyPressEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        connect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        connect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        connect( _z, SIGNAL(valueChanged(double)), _w, SLOT(setValue(double)));
        connect( _w, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

void XVector4DWidget::keyReleaseEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        disconnect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        disconnect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        disconnect( _z, SIGNAL(valueChanged(double)), _w, SLOT(setValue(double)));
        disconnect( _w, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

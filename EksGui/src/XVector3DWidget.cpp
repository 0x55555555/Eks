#include "Eks::Vector3DWidget"
#include "QVBoxLayout"
#include "XFloatWidget"
#include "QLabel"

#include "cmath"

#include "QKeyEvent"

Eks::Vector3DWidget::Eks::Vector3DWidget( QWidget *parent, Eks::Vector3D val, QStringList labels) : QWidget( parent ),
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

Eks::Vector3DWidget::Eks::Vector3DWidget( Eks::Vector3D val, Eks::Vector3D min, Eks::Vector3D max, QStringList labels, QWidget *parent ) : QWidget( parent ),
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

bool Eks::Vector3DWidget::isReadOnly() const
    {
    return _x->isReadOnly();
    }

void Eks::Vector3DWidget::setReadOnly(bool t)
    {
    _x->setReadOnly(t);
    _y->setReadOnly(t);
    _z->setReadOnly(t);
    }

void Eks::Vector3DWidget::setMaximum( Eks::Vector3D in )
    {
    _x->setMaximum( in.x() );
    _y->setMaximum( in.y() );
    _z->setMaximum( in.z() );
    }

Eks::Vector3D Eks::Vector3DWidget::maximum() const
    {
    return Eks::Vector3D( _x->maximum(), _y->maximum(), _z->maximum() );
    }

void Eks::Vector3DWidget::setMinimum( Eks::Vector3D in )
    {
    _x->setMinimum( in.x() );
    _y->setMinimum( in.y() );
    _z->setMinimum( in.z() );
    }

Eks::Vector3D Eks::Vector3DWidget::minimum() const
    {
    return Eks::Vector3D( _x->minimum(), _y->minimum(), _z->minimum() );
    }

Eks::Vector3D Eks::Vector3DWidget::range() const
    {
    return maximum() - minimum();
    }

Eks::Vector3D Eks::Vector3DWidget::value() const
    {
    return Eks::Vector3D( _x->value(), _y->value(), _z->value() );
    }

void Eks::Vector3DWidget::setValue( Eks::Vector3D in )
    {
    if( !_setting )
        {
        _setting = true;
        _x->setValue( in.x() );
        _y->setValue( in.y() );
        _z->setValue( in.z() );
        Q_EMIT valueChanged( in );
        _setting = false;
        }
    }

void Eks::Vector3DWidget::setValues()
    {
    setValue( Eks::Vector3D( _x->value(), _y->value(),  _z->value() ) );
    }

void Eks::Vector3DWidget::keyPressEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        connect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        connect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        connect( _z, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

void Eks::Vector3DWidget::keyReleaseEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        disconnect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        disconnect( _y, SIGNAL(valueChanged(double)), _z, SLOT(setValue(double)));
        disconnect( _z, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

#include "XVector2DWidget"
#include "QVBoxLayout"
#include "XFloatWidget"
#include "QLabel"

#include "cmath"

#include "QKeyEvent"

XVector2DWidget::XVector2DWidget( QWidget *parent, XVector2D val, QStringList labels) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), -HUGE_VAL, HUGE_VAL, this ) ),
        _y( new XFloatWidget( val.y(), -HUGE_VAL, HUGE_VAL, this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 2 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->setSpacing( 2 );

    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

XVector2DWidget::XVector2DWidget( XVector2D val, XVector2D min, XVector2D max, QStringList labels, QWidget *parent ) : QWidget( parent ),
        _x( new XFloatWidget( val.x(), min.x(), max.x(), this ) ),
        _y( new XFloatWidget( val.y(), min.y(), max.y(), this ) ),
        _setting( false )
    {
    QHBoxLayout *pLayout( new QHBoxLayout( this ) );
    pLayout->setContentsMargins( 0, 0, 0, 0 );

    if( labels.size() >= 2 )
        {
        QVBoxLayout *lLayout( new QVBoxLayout( ) );
        pLayout->addLayout( lLayout );
        lLayout->addWidget( new QLabel( labels[0] ) );
        lLayout->addWidget( new QLabel( labels[1] ) );
        }

    QVBoxLayout *_layout( new QVBoxLayout( ) );
    pLayout->addLayout( _layout );
    _layout->addWidget( _x );
    _layout->addWidget( _y );
    _layout->setSpacing( 2 );

    connect( _x, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    connect( _y, SIGNAL(valueChanged(double)), this, SLOT(setValues()) );
    }

bool XVector2DWidget::isReadOnly() const
    {
    return _x->isReadOnly();
    }

void XVector2DWidget::setReadOnly(bool t)
    {
    _x->setReadOnly(t);
    _y->setReadOnly(t);
    }

void XVector2DWidget::setMaximum( XVector2D in )
    {
    _x->setMaximum( in.x() );
    _y->setMaximum( in.y() );
    }

XVector2D XVector2DWidget::maximum() const
    {
    return XVector2D( _x->maximum(), _y->maximum() );
    }

void XVector2DWidget::setMinimum( XVector2D in )
    {
    _x->setMinimum( in.x() );
    _y->setMinimum( in.y() );
    }

XVector2D XVector2DWidget::minimum() const
    {
    return XVector2D( _x->minimum(), _y->minimum() );
    }

XVector2D XVector2DWidget::range() const
    {
    return maximum() - minimum();
    }

XVector2D XVector2DWidget::value() const
    {
    return XVector2D( _x->value(), _y->value() );
    }

void XVector2DWidget::setValue( XVector2D in )
    {
    if( !_setting )
        {
        _setting = true;
        _x->setValue( in.x() );
        _y->setValue( in.y() );
        emit valueChanged( in );
        _setting = false;
        }
    }

void XVector2DWidget::setValues()
    {
    setValue( XVector2D( _x->value(), _y->value() ) );
    }

void XVector2DWidget::keyPressEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        connect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        connect( _y, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

void XVector2DWidget::keyReleaseEvent( QKeyEvent *event )
    {
    if( event->key() == Qt::Key_Shift )
        {
        disconnect( _x, SIGNAL(valueChanged(double)), _y, SLOT(setValue(double)));
        disconnect( _y, SIGNAL(valueChanged(double)), _x, SLOT(setValue(double)));
        }
    }

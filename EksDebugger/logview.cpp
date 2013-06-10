#include "logview.h"
#include "QtCore/QAbstractItemModel"
#include "XDebugLogger.h"
#include "QtWidgets/QGraphicsItem"
#include "QtWidgets/QGraphicsTextItem"
#include "QtWidgets/QGraphicsSceneMouseEvent"
#include "QtWidgets/QStyleOptionGraphicsItem"
#include "QtGui/QWheelEvent"
#include "QtCore/QDebug"
#include "QtGui/QPen"

class ThreadItem;
static const float durationHeight = 30.0f;
static const float threadPad = 5;
static const float infoPad = 3;
static const float timelinePad = 10;
static const float updateTimeInterval = 100;
static const float timelineTextDrop = 30;

class ThreadsItem : public QGraphicsItem
  {
public:
  ThreadsItem(LogView *log)
      : _threads(Eks::Core::defaultAllocator()),
        _threadList(Eks::Core::defaultAllocator()),
        _log(log)
    {
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF();
    }

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    }

  ThreadItem *getThreadItem(quint64 t);
  const Eks::Vector<ThreadItem *> &threads() { return _threadList; }

private:
  Eks::Vector<ThreadItem *> _threadList;
  Eks::UnorderedMap<quint64, ThreadItem *> _threads;
  LogView *_log;
  };

class InfoItem : public QGraphicsTextItem
  {
public:
  InfoItem(EventItem *i)
    {
    setHtml(formatItem(i));
    }

  QString formatItem(EventItem *i)
    {
    QString location;

    if(i->location().file != "")
      {
      location = "<i>" + i->location().file + ", " +
                 i->location().function + ", on line " +
                 QString::number(i->location().line) + "</i><br>";
      }

    QString time = i->formattedTime();

    QString txt = location +
                  time +
                  "<br><b>" + i->display() + "</b>";
    return txt;
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QGraphicsTextItem::boundingRect().adjusted(-infoPad-2, -infoPad-2, infoPad+2, infoPad+2);
    }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) X_OVERRIDE
    {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::lightGray);

    auto rct = QGraphicsTextItem::boundingRect().adjusted(-infoPad, -infoPad, infoPad, infoPad);
    painter->drawRoundedRect(rct, infoPad, infoPad);

    QGraphicsTextItem::paint(painter, option, widget);
    }
  };

class DurationItem : public EventItem
  {
XProperties:
  XROByRefProperty(Eks::Time, start);
  XROByRefProperty(Eks::Time, end);

public:
  DurationItem(QGraphicsItem *item, ThreadItem *i);

  void setStartAndEnd(const Eks::Time &t)
    {
    _start = _end = t;
    if(isVisible())
      {
      prepareGeometryChange();
      }
    }

  void setEnd(const Eks::Time &t)
    {
    _end = t;
    if(isVisible())
      {
      prepareGeometryChange();
      }
    }

  QString formattedTime()
    {
    return QString::number(relativeTime(start()).milliseconds()) + "ms to " +
           QString::number(relativeTime(end()).milliseconds()) + "ms";
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    float t = timeToX(start());
    QRectF r(t, 0.0f, timeToX(end()) - t, durationHeight);

    return r.united(childrenBoundingRect());
    }

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    float t = timeToX(start());
    QRectF r(t, 0.0f, timeToX(end()) - t, durationHeight);

    if(isSelected())
      {
      QPen pen(Qt::red, 2.0f);
      p->setPen(pen);
      }
    else
      {
      p->setPen(Qt::black);
      }

    p->setBrush(Qt::white);
    p->drawRect(r);
    }
  };

class MomentItem : public EventItem
  {
XProperties:
  XROProperty(ThreadItem *, thread);
  XROByRefProperty(Eks::Time, time);

public:
  MomentItem(QGraphicsItem *item, ThreadItem *i);

  void setTime(const Eks::Time &t)
    {
    _time = t;
    if(isVisible())
      {
      prepareGeometryChange();
      }
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF(timeToX(time()), 0.0f, 1.0f, durationHeight);
    }

  QString formattedTime()
    {
    return QString::number(relativeTime(time()).milliseconds()) + "ms";
    }

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    QRectF r(timeToX(time()), 0.0f, 1.0f, durationHeight);

    if(isSelected())
      {
      QPen pen(Qt::red, 2.0f);
      p->setPen(pen);
      }
    else
      {
      p->setPen(Qt::black);
      }

    p->setBrush(Qt::white);
    p->drawRect(r);
    }
  };

ThreadItem::ThreadItem(LogView *l, QGraphicsItem *parent)
    : QGraphicsObject(parent),
      _log(l),
      _currentTime(Eks::Time::now())
  {

  }

float ThreadItem::timeToX(const Eks::Time &t) const
  {
  return _log->timeToX(t);
  }

void ThreadItem::setCurrentTime(const Eks::Time &t)
  {
  _currentTime = t;

  xForeach(auto d, _openDurations)
    {
    d->setEnd(t);
    }

  prepareGeometryChange();
  }

MomentItem *ThreadItem::addMoment(const Eks::Time &t)
  {
  QGraphicsItem *last = this;
  if(_openDurations.size())
    {
    last = _openDurations.back();
    }

  auto i = new MomentItem(last, this);
  i->setTime(t);

  return i;
  }

DurationItem *ThreadItem::addDuration(const Eks::Time &start)
  {
  QGraphicsItem *last = this;
  if(_openDurations.size())
    {
    last = _openDurations.back();
    }

  auto d = new DurationItem(last, this);
  _durationItems.insert(std::pair<Eks::Time, DurationItem*>(start, d));
  _openDurations.push_back(d);
  d->setStartAndEnd(start);
  d->setY(-durationHeight);
  return d;
  }

void ThreadItem::endDuration(DurationItem *e, const Eks::Time &time)
  {
  e->setEnd(time);
  auto it = std::find(_openDurations.begin(), _openDurations.end(), e);
  if(it == _openDurations.end())
    {
    return;
    }

  if((e->end() - e->start()).milliseconds() < 1)
    {
    e->setVisible(false);
    }

  _openDurations.erase(it);
  }

void ThreadItem::selectEvent(EventItem *item, const QPointF &pos)
  {
  log()->selectEvent(item, pos);
  }

QRectF ThreadItem::boundingRect() const
  {
  QRectF r(childrenBoundingRect());

  r.adjust(-threadPad, -threadPad, threadPad, threadPad);

  r.setLeft(timeToX(_log->start()));
  r.setRight(timeToX(currentTime()));

  return r;
  }

void ThreadItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
  {
  QRectF r(childrenBoundingRect());
  r.setLeft(timeToX(_log->start()));
  r.setRight(timeToX(currentTime()));

  r.adjust(-threadPad, -threadPad, threadPad, threadPad);

  p->setPen(Qt::red);
  p->setBrush(Qt::white);
  p->drawRoundedRect(r, threadPad, threadPad);
  }

void ThreadItem::timeConversionChanged()
  {
  prepareGeometryChange();
  }

void EventItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
  {
  thread()->selectEvent(this, e->scenePos());
  }

Eks::Time EventItem::relativeTime(const Eks::Time &t) const
  {
  auto &&s = thread()->log()->start();
  return (t - s);
  }

ThreadItem *ThreadsItem::getThreadItem(quint64 t)
  {
  auto item = _threads[t];
  if(!item)
    {
    item = new ThreadItem(_log, this);
    _threads[t] = item;
    _threadList << item;

    QObject::connect(_log, SIGNAL(timeConversionChanged()), item, SLOT(timeConversionChanged()));
    }

  return item;
  }

TimelineItem::TimelineItem(LogView *log) : _log(log)
  {
  _pendingLayoutThread = false;

  _threads = new ThreadsItem(log);
  _threads->setParentItem(this);

  setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
  }

QRectF TimelineItem::boundingRect() const
  {
  auto r = childrenBoundingRect().adjusted(-timelinePad-2, -timelinePad-2, timelinePad+2, timelinePad+2);
  r.setBottom(r.bottom() + timelineTextDrop);
  return r;
  }

void TimelineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *i, QWidget *)
  {
  QPen pen(Qt::black, 2.0f);

  painter->setPen(pen);
  painter->setBrush(Qt::white);

  auto rct = childrenBoundingRect().adjusted(-timelinePad, -timelinePad, timelinePad, timelinePad);
  painter->drawRoundedRect(rct, infoPad, infoPad);


  QRectF exposed = i->exposedRect;

  auto xPos = _log->xOffset();

  auto beginX = exposed.left() - xPos;
  auto endX = exposed.right() - xPos;

  auto begin = _log->timeFromX(beginX, false);
  auto end = _log->timeFromX(endX, false);

  float scale = 1.0f;
  float xMs = end.milliseconds() - begin.milliseconds();

  float adjustedDist = xMs;
  while(adjustedDist > 1.0f)
    {
    scale *= 10.0f;
    adjustedDist /= 10.0f;
    }
  while(adjustedDist < 0.1f)
    {
    scale /= 10.0f;
    adjustedDist *= 10.0f;
    }

  float clip = scale / 10.0f;
  float startPosition = floorf(xMax(0.0, beginX) / clip) * clip + xPos;
  float increment = _log->timeToXNoOffset(Eks::Time::fromMilliseconds(clip));

  painter->setPen(Qt::black);
  painter->setBrush(Qt::black);

  const int secMode = scale > 1000 ? 3 :
                      scale > 100  ? 2 :
                      scale > 10   ? 1 :
                                     0;

  auto pos = startPosition;
  for(int i = 0; i < 10; ++i, pos += increment)
    {
    auto t = _log->timeFromX(pos, false);
    float x = pos;

    auto sec = t.seconds();
    auto msec = t.milliseconds();
    auto microsec = t.microseconds();
    auto nsec = t.nanoseconds();

    QString formattedTime;
    if(secMode == 3)
      {
      formattedTime = QString::number(sec) + "s";
      }
    else if(secMode == 2)
      {
      formattedTime = QString::number(msec) + "ms";
      }
    else if(secMode == 1)
      {
      formattedTime = QString::number(microsec) + QString::fromUtf8("\xC2\xB5s");
      }
    else
      {
      formattedTime = QString::number(nsec) + "ns";
      }

    painter->drawLine(x, -timelinePad, x, timelineTextDrop);
    painter->drawText(x+2, timelineTextDrop, formattedTime);
    }
  }

ThreadsItem *TimelineItem::threads()
  {
  return _threads;
  }

void TimelineItem::layoutThreads()
  {
  if(!_pendingLayoutThread)
    {
    _pendingLayoutThread = true;
    startTimer(0);
    }
  }

void TimelineItem::timerEvent(QTimerEvent *)
  {
  if(_pendingLayoutThread)
    {
    _pendingLayoutThread = false;
    int usedY = 0;

    xForeach(auto th, threads()->threads())
      {
      auto bnds = th->boundingRect();

      th->setY(usedY);

      usedY -= bnds.height();
      th->setX(0);
      }
    }
  }

void TimelineItem::setCurrentTime(const Eks::Time &)
  {
  xForeach(auto th, _threads->threads())
    {
    th->setCurrentTime(Eks::Time::now());
    }
  }

void TimelineItem::timeConversionChanged()
  {
  prepareGeometryChange();
  }

EventItem::EventItem(QGraphicsItem *parent, ThreadItem *t) : QGraphicsObject(parent), _thread(t)
  {
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setAcceptedMouseButtons(Qt::LeftButton);
  }

void EventItem::mousePressEvent(QGraphicsSceneMouseEvent *)
  {
  }

void EventItem::timeConversionChanged()
  {
  prepareGeometryChange();
  }

DurationItem::DurationItem(QGraphicsItem *item, ThreadItem *i) : EventItem(item, i)
  {
  }

MomentItem::MomentItem(QGraphicsItem *item, ThreadItem *i) : EventItem(item, i)
  {
  }

float EventItem::timeToX(const Eks::Time &t) const
  {
  return _thread->timeToX(t);
  }


LogView::LogView(QAbstractItemModel *model)
  : _selected(0),
    _info(0),
    _scale(1.0f),
    _offset(0.0f),
    _lastDragX(0.0f)
  {
  setObjectName("Log");

  _min.set(X_INT64_MAX, X_INT64_MAX);
  _max.set(X_INT64_MIN, X_INT64_MIN);

  setRenderHint(QPainter::Antialiasing);

  connect(
    model,
    &QAbstractItemModel::rowsInserted,
    [this, model](QModelIndex parent, int rowStart, int rowEnd)
      {
      xAssert(rowEnd == rowStart);
      (void)rowEnd;
      auto idx = model->index(rowStart, 0, parent);
      auto display = model->data(idx, Qt::DisplayRole).toString();
      auto thread = model->data(idx, Eks::DebugLogger::Thread).toULongLong();
      auto loc = model->data(idx, Eks::DebugLogger::Location).value<Location>();

      auto d = model->data(idx, Eks::DebugLogger::MomentTime);
      if (d.isValid())
        {
        auto &&t = d.value<Eks::Time>();
        addMoment(idx, t, display, thread, loc);
        }
      else
        {
        auto &&d = model->data(idx, Eks::DebugLogger::StartTime).value<Eks::Time>();
        addDuration(idx, d, display, thread, loc);
        }
      }
    );

  connect(
    model,
    &QAbstractItemModel::dataChanged,
    [this, model](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &)
      {
      xAssert(topLeft == bottomRight);
      (void)bottomRight;

      auto d = model->data(topLeft, Eks::DebugLogger::EndTime).value<Eks::Time>();

      updateEnd(topLeft, d);
      }
    );

  startTimer(updateTimeInterval);

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  _timelineRoot = new TimelineItem(this);
  _scene.addItem(_timelineRoot);

  connect(this, SIGNAL(timeConversionChanged()), _timelineRoot, SLOT(timeConversionChanged()));

  setScene(&_scene);
  }

void LogView::timerEvent(QTimerEvent *)
  {
  //_timelineRoot->setCurrentTime(Eks::Time::now());
  }

float LogView::timeToX(const Eks::Time &t) const
  {
  return _offset + (_scale * (t - _min).milliseconds());
  }

float LogView::timeToXNoOffset(const Eks::Time &t) const
  {
  return t.milliseconds() * _scale;
  }

Eks::Time LogView::timeFromX(float x, bool offset) const
  {
  float scaledX = (x - _offset) / _scale;
  auto t = Eks::Time::fromMilliseconds(scaledX);

  if (offset)
    {
    return t + _min;
    }

  return t;
  }

void LogView::addDuration(
    const QModelIndex &id,
    const Eks::Time &t,
    const QString &disp,
    const quint64 thr,
    const Location &l)
  {
  auto thread = _timelineRoot->threads()->getThreadItem(thr);
  auto duration = thread->addDuration(t);

  connect(this, SIGNAL(timeConversionChanged()), duration, SLOT(timeConversionChanged()));

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  duration->setLocation(l);
  duration->setDisplay(disp);

  _openEvents[id] = duration;
  _timelineRoot->layoutThreads();
  }

void LogView::addMoment(
    const QModelIndex &,
    const Eks::Time &t,
    const QString &disp,
    const quint64 thr,
    const Location &l)
  {
  auto thread = _timelineRoot->threads()->getThreadItem(thr);
  auto moment = thread->addMoment(t);

  connect(this, SIGNAL(timeConversionChanged()), moment, SLOT(timeConversionChanged()));

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  moment->setLocation(l);
  moment->setDisplay(disp);
  _timelineRoot->layoutThreads();
  }

void LogView::updateEnd(
    const QModelIndex &id,
    const Eks::Time &t)
  {
  auto item = _openEvents[id];
  xAssert(item);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  auto thread = item->thread();
  thread->endDuration(item, t);

  _openEvents.remove(id);
  _timelineRoot->layoutThreads();
  }

void LogView::wheelEvent(QWheelEvent *event)
  {
  QPoint numDegrees = event->angleDelta() / 8;

  if (!numDegrees.isNull())
    {
    float scrollY = 1.0f + ((numDegrees.x() + numDegrees.y()) / 150.0f);
    _scale *= scrollY;

    auto xOffset = event->pos().x();

    //_offset = ((_offset - xOffset) * scrollY) + xOffset;

    emit timeConversionChanged();
    }

  event->accept();
  }

void LogView::mousePressEvent(QMouseEvent *event)
  {
  auto item = itemAt(event->pos());
  _dragging = !item;

  if(_dragging)
    {
    _lastDragX = event->pos().x();
    selectEvent(0, QPointF(0, 0));
    }
  else
    {
    QGraphicsView::mousePressEvent(event);
    }
  }

void LogView::mouseMoveEvent(QMouseEvent *event)
  {
  if(_dragging)
    {

    //_offset += event->pos().x() - _lastDragX;
    _lastDragX = event->pos().x();
    //emit timeConversionChanged();
    }
  else
    {
    QGraphicsView::mouseMoveEvent(event);
    }
  }

void LogView::mouseReleaseEvent(QMouseEvent *event)
  {
  if(_dragging)
    {
    _dragging = false;
    }
  else
    {
    QGraphicsView::mouseReleaseEvent(event);
    }
  }

const Eks::Time &LogView::start()
  {
  return _min;
  }

void LogView::selectEvent(EventItem *item, const QPointF &scenePos)
  {
  if(_selected)
    {
    _selected->setSelected(false);
    delete _info;
    }

  _selected = item;

  if(_selected)
    {
    _selected->setSelected(true);

    _info = new InfoItem(_selected);
    _scene.addItem(_info);
    _info->setPos(scenePos);
    _info->setZValue(10.0f);
    }
  }


#include "logview.h"
#include "QtCore/QAbstractItemModel"
#include "XDebugLogger.h"
#include "QtWidgets/QGraphicsItem"
#include "QtWidgets/QGraphicsTextItem"
#include "QtWidgets/QGraphicsSceneMouseEvent"
#include "QtWidgets/QStyleOptionGraphicsItem"
#include "QtWidgets/QScrollBar"
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
static const float maxContainerMs = 500;

class ThreadsItem : public QGraphicsItem
  {
public:
  ThreadsItem(Eks::AllocatorBase *alloc, LogView *log)
      : _threads(alloc),
        _threadList(alloc),
        _allocator(alloc),
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
  Eks::AllocatorBase *_allocator;
  };

class InfoItem : public QGraphicsTextItem
  {
public:
  InfoItem(const LogView *thr, EventItem *i)
    {
    setHtml(formatItem(thr, i));
    }

  QString formatItem(const LogView *thr, EventItem *i)
    {
    QString location;

    if(i->location() && i->location()->file() != "")
      {
      location = "<i>" + i->location()->file() + ", " +
                 i->location()->function() + ", on line " +
                 QString::number(i->location()->line()) + "</i><br>";
      }

    QString time = i->formattedTime(thr);

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
  DurationItem()
    {
    }

  void setStartAndEnd(const Eks::Time &t)
    {
    _start = _end = t;
    }

  void setEnd(const Eks::Time &t)
    {
    _end = t;
    }

  QString formattedTime(const LogView *thr) X_OVERRIDE
    {
    return QString::number(relativeTime(thr, start()).milliseconds()) + "ms to " +
           QString::number(relativeTime(thr, end()).milliseconds()) + "ms";
    }

  void paint(const ThreadItem *thr, QPainter *p, const Eks::Time &begin, const Eks::Time &e) X_OVERRIDE
    {
    xAssertFail();
    (void)begin;(void)e;
    float t = thr->timeToX(start());
    QRectF r(t, 0.0f, thr->timeToX(end()) - t, durationHeight);

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

private:
  X_DISABLE_COPY(DurationItem)
  };

class MomentItem : public EventItem
  {
XProperties:
  XROByRefProperty(Eks::Time, time);

public:
  MomentItem()
    {
    }

  void setTime(const Eks::Time &t)
    {
    _time = t;
    }

  QString formattedTime(const LogView *t)
    {
    return QString::number(relativeTime(t, time()).milliseconds()) + "ms";
    }

  void paint(const ThreadItem *t, QPainter *p, const Eks::Time &begin, const Eks::Time &end) X_OVERRIDE
    {
    xAssertFail();
    (void)begin;(void)end;

    QRectF r(t->timeToX(time()), 0.0f, 1.0f, durationHeight);

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

private:
  X_DISABLE_COPY(MomentItem)
  };

ThreadItem::ThreadItem(Eks::AllocatorBase *alloc, LogView *l, QGraphicsItem *parent)
    : QGraphicsObject(parent),
      _log(l),
      _currentTime(Eks::Time::now()),
      _containers(alloc),
      _currentContainer(nullptr),
      _allocator(alloc),
      _openDurations(alloc),
      _durationAlloc(alloc, Eks::ResourceDescriptionTypeHelper<DurationItem>::createFor()),
      _momentAlloc(alloc, Eks::ResourceDescriptionTypeHelper<MomentItem>::createFor()),
      _maxDurationEvents(0)
  {
  setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
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
  EventContainer *cont = getCurrentContainer(t);

  auto i = _momentAlloc.createShared<MomentItem>();
  i->setTime(t);

  cont->addMoment(i);

  clearCache(i);

  return i;
  }

DurationItem *ThreadItem::addDuration(const Eks::Time &start)
  {
  EventContainer *cont = getCurrentContainer(start);

  auto d = _durationAlloc.createShared<DurationItem>();
  cont->addDuration(d);

  _openDurations << d;
  d->setStartAndEnd(start);

  _maxDurationEvents = xMax(_maxDurationEvents, _openDurations.size());

  clearCache(d);

  return d;
  }

EventContainer *ThreadItem::getCurrentContainer(const Eks::Time &now)
  {
  if(!_currentContainer || (now - _currentContainer->start()).milliseconds() > maxContainerMs)
    {
    auto &elem = _containers.createBack();

    elem = _allocator->createUnique<EventContainer>(_allocator);
    _currentContainer = elem;

    xForeach(auto ev, _openDurations)
      {
      _currentContainer->addDuration(ev);
      }

    _currentContainer->setStart(now);
    }

  return _currentContainer;
  }

void ThreadItem::endDuration(DurationItem *e, const Eks::Time &time)
  {
  e->setEnd(time);
  _openDurations.removeAll(Eks::SharedPointer<DurationItem>(e));

  clearCache(e);
  }

void ThreadItem::cacheAndRenderBetween(QPainter *p, const Eks::Time &begin, const Eks::Time &end)
  {
  Eks::Time renderPosition = begin;

  while(renderPosition < end)
    {
    xForeach(const ImageCache& item, _cachedImages)
      {
      if(item.begin <= renderPosition && item.end > renderPosition)
        {
        p->drawImage(item.image);
        renderPosition = item.end;
        continue;
        }
      }
    }
  }

void ThreadItem::clearCache()
  {
  _cachedImages.clear();
  }

void ThreadItem::selectEvent(EventItem *item, const QPointF &pos)
  {
  log()->selectEvent(item, pos);
  }

QRectF ThreadItem::boundingRect() const
  {
  QRectF r;

  r.setBottom(0);
  r.setTop(durationHeight * _maxDurationEvents);

  r.setLeft(timeToX(_log->start()));
  r.setRight(timeToX(currentTime()));

  r.adjust(-threadPad, -threadPad, threadPad, threadPad);

  return r;
  }

void ThreadItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *)
  {
  QRectF r(childrenBoundingRect());
  r.setLeft(timeToX(_log->start()));
  r.setRight(timeToX(currentTime()));

  auto exposed = option->exposedRect;
  auto left = _log->timeFromX(exposed.left(), false);
  auto right = _log->timeFromX(exposed.right(), false);

  cacheAndRenderBetween(p, left, right);

  r.adjust(-threadPad, -threadPad, threadPad, threadPad);

  p->setPen(Qt::red);
  p->setBrush(Qt::white);
  p->drawRoundedRect(r, threadPad, threadPad);
  }

void ThreadItem::timeConversionChanged()
  {
  prepareGeometryChange();
  }

EventContainer::EventContainer(Eks::AllocatorBase *alloc)
    : _durationChildren(alloc),
      _momentChildren(alloc)
  {
  }

void EventContainer::addMoment(const Eks::SharedPointer<MomentItem> &item)
  {
  _momentChildren.pushBack(Eks::SharedPointer<MomentItem>(item));
  }

void EventContainer::addDuration(const Eks::SharedPointer<DurationItem> &item)
  {
  _durationChildren.pushBack(Eks::SharedPointer<DurationItem>(item));
  }

EventItem::EventItem() : _location(0), _isSelected(false)
  {
  }

Eks::Time EventItem::relativeTime(const LogView *thr, const Eks::Time &t) const
  {
  auto &&s = thr->start();
  return (t - s);
  }

ThreadItem *ThreadsItem::getThreadItem(quint64 t)
  {
  auto item = _threads[t];
  if(!item)
    {
    item = new ThreadItem(_allocator, _log, this);
    _threads[t] = item;
    _threadList << item;

    QObject::connect(_log, SIGNAL(timeConversionChanged()), item, SLOT(timeConversionChanged()));
    }

  return item;
  }

TimelineItem::TimelineItem(Eks::AllocatorBase *alloc, LogView *log) : _log(log)
  {
  _pendingLayoutThread = false;

  _threads = new ThreadsItem(alloc, log);
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
    startTimer(25);
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


LogView::LogView(QObject *model)
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
    qobject_cast<Eks::DebugLoggerData*>(model),
    &Eks::DebugLoggerData::eventCreated,
    [this, model](const Eks::Time &time,
        Eks::ThreadEventLogger::EventType type,
        xuint64 thread,
        const QString &display,
        const xsize durationId,
        const Eks::DebugLogger::DebugLocationWithData *loc)
      {
      if (type == Eks::ThreadEventLogger::EventType::Moment)
        {
        addMoment(time, display, thread, loc);
        }
      else
        {
        addDuration(durationId, time, display, thread, loc);
        }
      }
    );

  connect(
    qobject_cast<Eks::DebugLoggerData*>(model),
    &Eks::DebugLoggerData::eventEndUpdated,
    [this, model](const xsize id, const xsize thread, const Eks::Time &endTime)
      {
      updateEnd(id, thread, endTime);
      }
    );

  startTimer(updateTimeInterval);

  _timelineRoot = new TimelineItem(Eks::Core::defaultAllocator(), this);
  _scene.addItem(_timelineRoot);

  connect(this, SIGNAL(timeConversionChanged()), _timelineRoot, SLOT(timeConversionChanged()));

  setScene(&_scene);
  }

void LogView::timerEvent(QTimerEvent *)
  {
  _timelineRoot->setCurrentTime(Eks::Time::now());

  setSceneRect(_scene.itemsBoundingRect());
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
    const xsize id,
    const Eks::Time &t,
    const QString &disp,
    const xuint64 thr,
    const Location *l)
  {
  _min = xMin(_min, t);
  _max = xMax(_max, t);

  auto thread = _timelineRoot->threads()->getThreadItem(thr);
  auto duration = thread->addDuration(t);


  duration->setLocation(l);
  duration->setDisplay(disp);

  OpenEvent ev(thr, id);

  _openEvents[ev] = ActiveDuration(thread, duration);
  _timelineRoot->layoutThreads();
  }

void LogView::addMoment(
    const Eks::Time &t,
    const QString &disp,
    const xuint64 thr,
    const Location *l)
  {
  auto thread = _timelineRoot->threads()->getThreadItem(thr);
  auto moment = thread->addMoment(t);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  moment->setLocation(l);
  moment->setDisplay(disp);
  _timelineRoot->layoutThreads();
  }

void LogView::updateEnd(
    const xsize id,
    const xuint64 thr,
    const Eks::Time &t)
  {
  OpenEvent ev(thr, id);
  auto item = _openEvents[ev];
  _openEvents.remove(ev);

  xAssert(item.duration);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  item.thread->endDuration(item.duration, t);

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

    (void)xOffset;
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
    auto bar = verticalScrollBar();

    bar->setValue(bar->value() + event->pos().x() - _lastDragX);

    _lastDragX = event->pos().x();
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

const Eks::Time &LogView::start() const
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

    _info = new InfoItem(this, _selected);
    _scene.addItem(_info);
    _info->setPos(scenePos);
    _info->setZValue(10.0f);
    }
  }


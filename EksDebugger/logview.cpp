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
static const int cachedImageWidth = 256;

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

  float getZeroX(QGraphicsItem *to);

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

  bool endTime(Eks::Time &e) X_OVERRIDE
    {
    e = end();
    return true;
    }

  QString formattedTime(const LogView *thr) X_OVERRIDE
    {
    return QString::number(relativeTime(thr, start()).milliseconds()) + "ms to " +
           QString::number(relativeTime(thr, end()).milliseconds()) + "ms";
    }

  void paint(const ThreadItem *thr, QPainter *p) X_OVERRIDE
    {
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

  bool endTime(Eks::Time &) X_OVERRIDE
    {
    return false;
    }

  QString formattedTime(const LogView *t)
    {
    return QString::number(relativeTime(t, time()).milliseconds()) + "ms";
    }

  void paint(const ThreadItem *t, QPainter *p) X_OVERRIDE
    {
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
      _maxDurationEvents(0),
      _currentEventCache(alloc)
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

  xForeach(auto &d, _openDurations)
    {
    d.item->setEnd(t);
    }

  prepareGeometryChange();
  }

MomentItem *ThreadItem::addMoment(const Eks::Time &t)
  {
  EventContainer *cont = getCurrentContainer(t);

  auto i = _momentAlloc.createShared<MomentItem>();
  i->setTime(t);

  cont->addMoment(i);

  clearCurrentContainerCache();

  return i;
  }

DurationItem *ThreadItem::addDuration(xsize id, const Eks::Time &start)
  {
  EventContainer *cont = getCurrentContainer(start);

  auto d = _durationAlloc.createShared<DurationItem>();
  cont->addDuration(d);

  auto &created = _openDurations.createBack();
  created.item = d;
  created.id = id;

  d->setStartAndEnd(start);

  _maxDurationEvents = xMax(_maxDurationEvents, _openDurations.size());

  clearCurrentContainerCache();

  return d;
  }

void ThreadItem::endDuration(xsize id, const Eks::Time &time)
  {
  DurationItem *e = nullptr;

  for(xsize i = _openDurations.size()-1; i != X_SIZE_SENTINEL; --i)
    {
    auto &dur = _openDurations[i];
    if(dur.id == id)
      {
      e = dur.item;
      _openDurations.removeAt(i);
      break;
      }
    }

  if(!e)
    {
    xAssertFail();
    }
  e->setEnd(time);

  clearCurrentContainerCache();
  }

EventContainer *ThreadItem::getCurrentContainer(const Eks::Time &now)
  {
  if(!_currentContainer || (now - _currentContainer->start()).milliseconds() > maxContainerMs)
    {
    if(_currentContainer)
      {
      clearCurrentContainerCache();
      }

    auto &elem = _containers.createBack();

    elem = _allocator->createUnique<EventContainer>(_allocator);
    _currentContainer = elem;

    xForeach(auto ev, _openDurations)
      {
      _currentContainer->addDuration(ev.item);
      }

    _currentContainer->setStart(now);
    }

  return _currentContainer;
  }

void ThreadItem::cacheAndRenderBetween(QPainter *p, const Eks::Time &begin, const Eks::Time &end)
  {
  Eks::Time renderPosition = _log->start() + begin;
  Eks::Time renderEnd = _log->start() + end;
  
  qDebug() << "Render timeline between" << begin.milliseconds() << "and" << end.milliseconds();

  while(renderPosition < renderEnd)
    {
    xForeach(const ImageCache* item, _cachedImages)
      {
      qDebug() << "Cache available between" << (item->begin - _log->start()).milliseconds() << 
        "and" << (item->end - _log->start()).milliseconds();
      }

    xForeach(const ImageCache* item, _cachedImages)
      {
      if(item->begin <= renderPosition &&
         item->end > renderPosition)
        {
        p->drawImage(_log->timeToX(item->begin), 0.0f, item->image);
        renderPosition = item->end;
        
        qDebug() << "Render cached image between" << (item->begin - _log->start()).milliseconds() << 
          "and" << (item->end - _log->start()).milliseconds();

        break;
        }
      }

    if(renderPosition >= renderEnd)
      {
      break;
      }

    qDebug() << "Cache between" << (renderPosition - _log->start()).milliseconds() << 
      "and" << ((renderPosition + _log->timeFromWidth(cachedImageWidth)) - _log->start()).milliseconds();

    Eks::UniquePointer<ImageCache> cache;
    if(_reservedImages.size())
      {
      cache = _reservedImages.popBack();
      }
    else
      {
      cache = _allocator->createUnique<ImageCache>();
      }
    xAssert(cache);

    cache->begin = renderPosition;
    cache->end = renderPosition = cache->begin + _log->timeFromWidth(cachedImageWidth);

    Eks::TemporaryAllocator alloc(Eks::Core::temporaryAllocator());
    std::map<
        Eks::Time,
        EventItem *,
        std::less<Eks::Time>,
        Eks::TypedAllocator<std::pair<const Eks::Time, EventItem *>>> events(&alloc);

    Eks::UnorderedMap<const DurationItem *, bool> found(&alloc);

    bool current = false;
    xForeach(const auto &cont, _containers)
      {
      bool eventsInContainer = false;
      xForeach(const auto &dur, cont->durationChildren())
        {
        if(dur->start() < cache->end &&
           dur->end() > cache->begin &&
           !found.contains(dur))
          {
          found.insert(dur, true);
          events[dur->start()] = dur.ptr();
          eventsInContainer = true;
          }
        }

      xForeach(const auto &mom, cont->momentChildren())
        {
        if((mom->time() > cache->begin && mom->time() < cache->end))
          {
          events[mom->time()] = mom.ptr();
          eventsInContainer = true;
          }
        }

      if(eventsInContainer && cont == _currentContainer)
        {
        current = true;
        }
      }

    if(current && events.size())
      {
      _currentEventCache << cache;
      }

    cache->image = QImage(
      cachedImageWidth,
      durationHeight *_maxDurationEvents,
      QImage::Format_ARGB32_Premultiplied);

    cache->image.fill(Qt::blue);

    Eks::Vector<Eks::Time> activeStack(&alloc);
    for(auto it = events.begin(), end = events.end(); it != end; ++it)
      {
      const auto &t = it->first;
      const auto &ev = it->second;

      if(activeStack.size() && t > activeStack.back())
        {
        activeStack.popBack();
        }

      ev->paint(this, p);

      Eks::Time endTime;
      if(ev->endTime(endTime))
        {
        activeStack << endTime;
        }
      }

    _cachedImages << std::move(cache);
    }
  }

void ThreadItem::clearCache()
  {
  qDebug() << "Clear all cached images";

  _currentEventCache.clear();
  _cachedImages.clear();
  }

void ThreadItem::clearCurrentContainerCache()
  {
  xForeach(auto *cache, _currentEventCache)
    {
    for(auto it = _cachedImages.begin(), end = _cachedImages.end();
        it != end;
        ++it)
      {
      if(cache == *it)
        {
        qDebug() << "Clear cached segment between" << (cache->begin - _log->start()).milliseconds() << 
          "and" << (cache->end - _log->start()).milliseconds();;
        it = _cachedImages.remove(it);
        end = _cachedImages.end();
        break;
        }
      }
    }

  _currentEventCache.clear();
  }

void ThreadItem::selectEvent(EventItem *item, const QPointF &pos)
  {
  log()->selectEvent(item, pos);
  }

QRectF ThreadItem::boundingRect() const
  {
  QRectF r;
  
  r.setTop(-durationHeight * _maxDurationEvents);
  r.setBottom(0);

  r.setLeft(timeToX(_log->start()));
  r.setRight(timeToX(currentTime()));

  r.adjust(-threadPad, -threadPad, threadPad, threadPad);

  return r;
  }

void ThreadItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *)
  {
  QRectF r(boundingRect());

  auto exposed = option->exposedRect;
  auto left = _log->timeFromX(exposed.left(), false);
  auto right = _log->timeFromX(exposed.right(), false);

  cacheAndRenderBetween(p, left, right);

  p->setPen(Qt::red);
  p->setBrush(Qt::white);
  p->drawRoundedRect(r, threadPad, threadPad);
  }

void ThreadItem::timeConversionChanged()
  {
  prepareGeometryChange();
  clearCache();
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

float ThreadsItem::getZeroX(QGraphicsItem *to)
  {
  if(!_threads.size())
    {
    return 0.0f;
    }

  QGraphicsItem *item = _threadList.front();
  return item->mapToItem(to, 0.0, 0.0).x();
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

  auto start = _threads->getZeroX(this);
  auto pos = startPosition;
  for(int i = 0; i < 10; ++i, pos += increment)
    {
    auto t = _log->timeFromX(pos, false);
    float x = start + pos;

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

Eks::Time LogView::timeFromWidth(float w) const
  {
  float scaledX = (w) / _scale;
  auto t = Eks::Time::fromMilliseconds(scaledX);
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
  auto duration = thread->addDuration(id, t);


  duration->setLocation(l);
  duration->setDisplay(disp);

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
  auto thread = _timelineRoot->threads()->getThreadItem(thr);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  thread->endDuration(id, t);

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


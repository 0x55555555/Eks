#include "logview.h"
#include "QtCore/QAbstractItemModel"
#include "XDebugLogger.h"
#include "QtWidgets/QGraphicsItem"
#include "QtWidgets/QGraphicsTextItem"
#include "QtWidgets/QGraphicsSceneMouseEvent"
#include "QtCore/QDebug"
#include "QtGui/QPen"

class ThreadItem;
static const float durationHeight = 30.0f;
static const float threadPad = 5;
static const float infoPad = 3;

class EventItem : public QGraphicsItem
  {
XProperties:
  XROProperty(ThreadItem *, thread);
  XByRefProperty(LogView::Location, location, setLocation);
  XByRefProperty(QString, display, setDisplay);

public:
  EventItem(QGraphicsItem *parent, ThreadItem *t) : QGraphicsItem(parent), _thread(t)
    {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    }

  void mousePressEvent(QGraphicsSceneMouseEvent *)
    {
    }

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

  float timeToX(const Eks::Time &t) const;
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

    QString txt = location +
                  "<b>" + i->display() + "</b>";
    return txt;
    }

  QRectF boundingRect()
    {
    return QGraphicsTextItem::boundingRect().adjusted(-infoPad-2, -infoPad-2, infoPad+2, infoPad+2);
    }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::lightGray);
    painter->drawRoundedRect(boundingRect(), infoPad, infoPad);

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
    prepareGeometryChange();
    }

  void setEnd(const Eks::Time &t)
    {
    _end = t;
    prepareGeometryChange();
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
    prepareGeometryChange();
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF(timeToX(time()), 0.0f, 1.0f, durationHeight);
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

class ThreadItem : public QGraphicsItem
  {
XProperties:
  XROProperty(LogView *, log);

public:
  ThreadItem(LogView *l) : _log(l)
    {
    }

  float timeToX(const Eks::Time &t) const
    {
    return _log->timeToX(t);
    }

  MomentItem *addMoment(const Eks::Time &t)
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

  DurationItem *addDuration(const Eks::Time &start)
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

  void endDuration(DurationItem *e, const Eks::Time &time)
    {
    e->setEnd(time);
    auto it = std::find(_openDurations.begin(), _openDurations.end(), e);
    if(it == _openDurations.end())
      {
      return;
      }

    _openDurations.erase(it);
    }

  void selectEvent(EventItem *item, const QPointF &pos)
    {
    log()->selectEvent(item, pos);
    }

  QRectF boundingRect() const X_OVERRIDE
    {
    QRectF r(childrenBoundingRect());
    r.adjust(-threadPad, -threadPad, threadPad, threadPad);
    return r;
    }

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    QRectF r(childrenBoundingRect());
    r.adjust(-threadPad, -threadPad, threadPad, threadPad);

    p->setPen(Qt::red);
    p->setBrush(Qt::white);
    p->drawRoundedRect(r, threadPad, threadPad);
    }

private:
  std::multimap<Eks::Time, DurationItem *> _durationItems;
  std::vector<DurationItem *> _openDurations;
  };

void EventItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
  {
  thread()->selectEvent(this, e->scenePos());
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
  : _threads(Eks::Core::defaultAllocator()),
    _threadList(Eks::Core::defaultAllocator()),
    _selected(0),
    _info(0)
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

  setScene(&_scene);
  }

float LogView::timeToX(const Eks::Time &t) const
  {
  return (t - _min).milliseconds();
  }

void LogView::layoutThreads()
  {
  int usedY = 0;

  xForeach(auto th, _threadList)
    {
    auto bnds = th->boundingRect();
    
    th->setY(usedY);

    usedY -= bnds.height();
    th->setX(0);
    }
  }

void LogView::addDuration(
    const QModelIndex &id,
    const Eks::Time &t,
    const QString &disp,
    const quint64 thr,
    const Location &l)
  {
  auto thread = getThreadItem(thr);
  auto duration = thread->addDuration(t);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  duration->setLocation(l);
  duration->setDisplay(disp);

  _openEvents[id] = duration;
  layoutThreads();
  }

void LogView::addMoment(
    const QModelIndex &,
    const Eks::Time &t,
    const QString &disp,
    const quint64 thr,
    const Location &l)
  {
  auto thread = getThreadItem(thr);
  auto moment = thread->addMoment(t);

  _min = xMin(_min, t);
  _max = xMax(_max, t);

  moment->setLocation(l);
  moment->setDisplay(disp);
  layoutThreads();
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
  layoutThreads();
  }

ThreadItem *LogView::getThreadItem(quint64 t)
  {
  auto item = _threads[t];
  if(!item)
    {
    item = new ThreadItem(this);
    _threads[t] = item;
    _threadList << item;
    _scene.addItem(item);
    }

  return item;
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
    }
  }


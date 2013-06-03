#include "logview.h"
#include "QtCore/QAbstractItemModel"
#include "XDebugLogger.h"
#include "QtWidgets/QGraphicsItem"

static const float durationHeight = 30.0f;

float timeToX(const Eks::Time &t)
  {
  return t.milliseconds();
  }

class DurationItem : public QGraphicsItem
  {
XProperties:
  XROByRefProperty(Eks::Time, start);
  XROByRefProperty(Eks::Time, end);
  XByRefProperty(LogView::Location, location, setLocation);
  XByRefProperty(QString, display, setDisplay);

public:
  DurationItem(QGraphicsItem *i) : QGraphicsItem(i) { }

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

    p->setPen(Qt::black);
    p->setBrush(Qt::white);
    p->drawRect(r);
    }
  };

class MomentItem : public QGraphicsItem
  {
XProperties:
  XROByRefProperty(Eks::Time, time);
  XByRefProperty(LogView::Location, location, setLocation);
  XByRefProperty(QString, display, setDisplay);

public:
  MomentItem(QGraphicsItem *i) : QGraphicsItem(i) { }

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
    p->setPen(Qt::black);
    p->setBrush(Qt::white);
    p->drawRect(r);
    }
  };

class ThreadItem : public QGraphicsItem
  {
public:
  ThreadItem(QGraphicsItem *i=0) : QGraphicsItem(i)
    {
    }

  MomentItem *addMoment(const Eks::Time &t)
    {
    QGraphicsItem *last = this;
    if(_openDurations.size())
      {
      last = _openDurations.back();
      }

    auto i = new MomentItem(last);
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

    auto d = new DurationItem(last);
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

  QRectF boundingRect() const X_OVERRIDE
    {
    return childrenBoundingRect();
    }

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    QRectF r(childrenBoundingRect());

    p->setPen(Qt::black);
    p->setBrush(Qt::white);
    p->drawRect(r);
    }

private:
  std::multimap<Eks::Time, DurationItem *> _durationItems;
  std::vector<DurationItem *> _openDurations;
  };

LogView::LogView(QAbstractItemModel *model)
  {
  setObjectName("Log");

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
        addMoment(idx, d.value<Eks::Time>(), display, thread, loc);
        }
      else
        {
        auto d = model->data(idx, Eks::DebugLogger::StartTime).value<Eks::Time>();
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

void LogView::addDuration(
    const QModelIndex &id,
    const Eks::Time &t,
    const QString &disp,
    const quint64 thr,
    const Location &l)
  {
  auto thread = getThreadItem(thr);
  auto duration = thread->addDuration(t);

  duration->setLocation(l);
  duration->setDisplay(disp);

  _openEvents[id] = duration;
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

  moment->setLocation(l);
  moment->setDisplay(disp);
  }

void LogView::updateEnd(
    const QModelIndex &id,
    const Eks::Time &t)
  {
  auto item = _openEvents[id];
  xAssert(item);

  auto thread = static_cast<ThreadItem*>(item->parentItem());
  thread->endDuration(item, t);

  _openEvents.remove(id);
  }

ThreadItem *LogView::getThreadItem(quint64 t)
  {
  auto item = _threads[t];
  if(!item)
    {
    item = new ThreadItem();
    _scene.addItem(item);
    }

  return item;
  }


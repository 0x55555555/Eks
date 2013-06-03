#include "logview.h"
#include "QtCore/QAbstractItemModel"
#include "XDebugLogger.h"
#include "QtWidgets/QGraphicsItem"

class ThreadItem : public QGraphicsItem
  {
public:
  ThreadItem(QGraphicsItem *i=0) : QGraphicsItem(i) { }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF();
    }

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    }
  };

class DurationItem : public QGraphicsItem
  {
XProperties:
  XByRefProperty(Eks::Time, start, setStart);
  XByRefProperty(Eks::Time, end, setEnd);
  XByRefProperty(LogView::Location, location, setLocation);
  XByRefProperty(QString, display, setDisplay);

public:
  DurationItem(QGraphicsItem *i) : QGraphicsItem(i) { }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF();
    }

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    }
  };

class MomentItem : public QGraphicsItem
  {
XProperties:
  XByRefProperty(Eks::Time, time, setTime);
  XByRefProperty(LogView::Location, location, setLocation);
  XByRefProperty(QString, display, setDisplay);

public:
  MomentItem(QGraphicsItem *i) : QGraphicsItem(i) { }

  QRectF boundingRect() const X_OVERRIDE
    {
    return QRectF();
    }

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE
    {
    }
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
  auto duration = new DurationItem(thread);
  duration->setStart(t);

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
  auto moment = new MomentItem(thread);

  moment->setTime(t);

  moment->setLocation(l);
  moment->setDisplay(disp);
  }

void LogView::updateEnd(
    const QModelIndex &id,
    const Eks::Time &t)
  {
  auto item = _openEvents[id];
  xAssert(item);

  item->setEnd(t);
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


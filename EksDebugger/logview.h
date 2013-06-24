#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "QtWidgets/QGraphicsView"
#include "QtWidgets/QGraphicsItem"
#include "XEventLogger"
#include "XDebugLogger.h"
#include "XUnorderedMap"
#include "QtCore/QPersistentModelIndex"
#include "XSharedPointer"
#include "XBucketAllocator"

class QGraphicsScene;
class QAbstractItemModel;

class EventItem;
class ThreadItem;
class MomentItem;
class DurationItem;
class InfoItem;
class TimelineItem;
class ThreadsItem;

class LogView : public QGraphicsView
  {
  Q_OBJECT

public:
  typedef Eks::DebugLogger::DebugLocationWithData Location;

  LogView(QObject *model);

  const Eks::Time &start() const;

  void selectEvent(EventItem *item, const QPointF &scenePos);
  float xOffset() const { return _offset; }
  float timeToX(const Eks::Time &t) const;
  float timeToXNoOffset(const Eks::Time &t) const;
  Eks::Time timeFromX(float x, bool offset) const;
  Eks::Time timeFromWidth(float x) const;

protected:
  void timerEvent(QTimerEvent *) X_OVERRIDE;

signals:
  void timeConversionChanged();

private:
  void addDuration(
      const xsize id,
      const Eks::Time &t,
      const QString &disp,
      const xuint64 thr,
      const Location *l);
  void addMoment(
      const Eks::Time &t,
      const QString &disp,
      const xuint64 thr,
      const Location *l);
  void updateEnd(
      const xsize id,
      const xuint64 thr,
      const Eks::Time &t);

  void wheelEvent(QWheelEvent *event) X_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) X_OVERRIDE;
  void mousePressEvent(QMouseEvent *event) X_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent *event) X_OVERRIDE;

  QGraphicsScene _scene;

  TimelineItem *_timelineRoot;

  EventItem *_selected;
  InfoItem *_info;

  float _scale;
  float _offset;

  float _lastDragX;
  bool _dragging;

  Eks::Time _min;
  Eks::Time _max;
  };

class TimelineItem : public QGraphicsObject
  {
  Q_OBJECT

public:
  TimelineItem(Eks::AllocatorBase *allocator, LogView *log);

  QRectF boundingRect() const X_OVERRIDE;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE;

  ThreadsItem *threads();
  void setCurrentTime(const Eks::Time &);

public slots:
  void timeConversionChanged();
  void layoutThreads();

protected:
  void timerEvent(QTimerEvent *event) X_OVERRIDE;

private:
  ThreadsItem *_threads;
  LogView *_log;
  bool _pendingLayoutThread;
  };

class EventContainer
  {
public:
  typedef Eks::Vector<Eks::SharedPointer<DurationItem>> DurationVector;
  typedef Eks::Vector<Eks::SharedPointer<MomentItem>> MomentVector;

XProperties:
  XProperty(Eks::Time, start, setStart);

  XROByRefProperty(DurationVector, durationChildren);
  XROByRefProperty(MomentVector, momentChildren);

public:
  EventContainer(Eks::AllocatorBase *alloc);

  void addMoment(const Eks::SharedPointer<MomentItem> &item);
  void addDuration(const Eks::SharedPointer<DurationItem> &item);
  };

class EventItem : public Eks::detail::SharedData
  {
XProperties:
  XProperty(const LogView::Location *, location, setLocation);
  XByRefProperty(QString, display, setDisplay);
  XProperty(bool, isSelected, setSelected);

public:
  EventItem();

  virtual QString formattedTime(const LogView *thr) = 0;
  virtual void paint(const ThreadItem *t, QPainter *p) = 0;

  virtual bool endTime(Eks::Time &) = 0;

  Eks::Time relativeTime(const LogView *thr, const Eks::Time &t) const;

private:
  X_DISABLE_COPY(EventItem);
  };

class ThreadItem : public QGraphicsObject
  {
  Q_OBJECT

XProperties:
  XROProperty(LogView *, log);
  XROByRefProperty(Eks::Time, currentTime);

public:
  ThreadItem(Eks::AllocatorBase *alloc, LogView *l, QGraphicsItem *parent);

  float timeToX(const Eks::Time &t) const;

  void setCurrentTime(const Eks::Time &t);

  MomentItem *addMoment(const Eks::Time &t);
  DurationItem *addDuration(xsize id, const Eks::Time &start);
  void endDuration(xsize id, const Eks::Time &time);

  void selectEvent(EventItem *item, const QPointF &pos);

  QRectF boundingRect() const X_OVERRIDE;

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE;

public slots:
  void timeConversionChanged();

private:
  EventContainer *getCurrentContainer(const Eks::Time &now);

  EventContainer *_currentContainer;

  void cacheAndRenderBetween(QPainter *p, const Eks::Time &begin, const Eks::Time &end);
  void clearCache();
  void clearCurrentContainerCache();

  struct ImageCache
    {
    Eks::Time begin;
    Eks::Time end;
    QImage image;
    };

  enum
    {
    MaxImages = 32
    };

  Eks::Vector<Eks::UniquePointer<ImageCache>, MaxImages> _cachedImages;
  Eks::Vector<Eks::UniquePointer<ImageCache>> _reservedImages;

  Eks::AllocatorBase *_allocator;
  Eks::FixedSizeBucketAllocator _momentAlloc;
  Eks::FixedSizeBucketAllocator _durationAlloc;

  xsize _maxDurationEvents;

  Eks::Vector <ImageCache *> _currentEventCache;
  Eks::Vector <Eks::UniquePointer<EventContainer>> _containers;


  struct DurationStruct
    {
    Eks::SharedPointer<DurationItem> item;
    xsize id;
    };

  Eks::Vector <DurationStruct, 64> _openDurations;
  };

#endif // LOGVIEW_H

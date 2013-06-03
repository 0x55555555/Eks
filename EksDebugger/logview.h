#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "QtWidgets/QGraphicsView"
#include "XEventLogger"
#include "XUnorderedMap"
#include "QtCore/QPersistentModelIndex"

class QGraphicsScene;
class QAbstractItemModel;

class ThreadItem;
class DurationItem;

class LogView : public QGraphicsView
  {
  Q_OBJECT

public:
  typedef Eks::ThreadEventLogger::EventData::Location Location;

  LogView(QAbstractItemModel *model);

private:
  void addDuration(
      const QModelIndex &id,
      const Eks::Time &t,
      const QString &disp,
      const quint64 thr,
      const Location &l);
  void addMoment(
      const QModelIndex &id,
      const Eks::Time &t,
      const QString &disp,
      const quint64 thr,
      const Location &l);
  void updateEnd(
      const QModelIndex &id,
      const Eks::Time &t);

  ThreadItem *getThreadItem(quint64);

  Eks::UnorderedMap<quint64, ThreadItem *> _threads;
  Eks::UnorderedMap<QPersistentModelIndex, DurationItem *> _openEvents;
  QGraphicsScene _scene;
  };

#endif // LOGVIEW_H

#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "QtWidgets/QTreeView"

class LogView : public QTreeView
  {
public:
  LogView(QAbstractItemModel *model);
  };

#endif // LOGVIEW_H

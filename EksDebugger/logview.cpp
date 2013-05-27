#include "logview.h"

LogView::LogView(QAbstractItemModel *model)
  {
  setObjectName("Log");
  setModel(model);
  }

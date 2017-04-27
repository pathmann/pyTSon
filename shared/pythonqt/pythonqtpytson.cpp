#include "pythonqtpytson.h"

#include <QLayout>

const QVariantList pytsondecorator::getContentsMargins(QLayout* layout) {
  int top, left, bottom, right;
  layout->getContentsMargins(&left, &top, &right, &bottom);

  return QVariantList() << left << top << right << bottom;
}

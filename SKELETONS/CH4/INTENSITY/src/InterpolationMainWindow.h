#ifndef _INTERPOLATIONMAINWINDOW_H_
#define _INTERPOLATIONMAINWINDOW_H_
#include "ui_InterpolationMainWindow.h"
class InterpolationMainWindow:public QMainWindow {

  Q_OBJECT

public:

  InterpolationMainWindow(QWidget *parent=NULL, const std::string & filename="");
  ~InterpolationMainWindow();

public slots:

  
  // The refresh function:
  void refresh();

private:

  Ui::InterpolationMainWindow ui;

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  InterpolationMainWindow(const InterpolationMainWindow &);
  InterpolationMainWindow & operator = (const InterpolationMainWindow &);

};
#endif

#include "mainview.h"

#include <QDebug>

// Triggered by pressing a key
void MainView::keyPressEvent(QKeyEvent* ev) {
  switch (ev->key()) {
  case 'R': {
    pitch = yaw = 0.0f;
    camera_distance = 2.0f;
  }
  default:
    break;
  }

  // Used to update the screen after changes
  update();
}

// Triggered by releasing a key
void MainView::keyReleaseEvent(QKeyEvent* ev) {
  switch (ev->key()) {
  default:
    qDebug() << ev->key() << "released";
    break;
  }

  update();
}

// Triggered by clicking two subsequent times on any mouse button
// It also fires two mousePress and mouseRelease events!
void MainView::mouseDoubleClickEvent(QMouseEvent* ev) {
  qDebug() << "Mouse double clicked:" << ev->button();

  update();
}

// Triggered when moving the mouse inside the window (only when the mouse is
// clicked!)
void MainView::mouseMoveEvent(QMouseEvent* ev) {
  if (ev->buttons() == Qt::LeftButton) {
    auto drag_end = ev->pos();
    auto delta = (drag_end - drag_start);

    pitch += 360.0f * delta.x() / QWidget::width();
    yaw += 360.0f * delta.y() / QWidget::height();
  }
  drag_start = ev->pos();

  update();
}

// Triggered when pressing any mouse button
void MainView::mousePressEvent(QMouseEvent* ev) {
  if (ev->button() == Qt::LeftButton) {
    drag_start = ev->pos();
  }

  update();
  // Do not remove the line below, clicking must focus on this widget!
  this->setFocus();
}

// Triggered when releasing any mouse button
void MainView::mouseReleaseEvent(QMouseEvent* ev) {
  Q_UNUSED(ev);
  update();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainView::wheelEvent(QWheelEvent* ev) {
  camera_distance -= ev->delta() / 360.0f;
  update();
}

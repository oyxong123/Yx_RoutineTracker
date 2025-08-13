#pragma once
#include <QWidget>

class WindowTracker {
public:
    static void setCurrent(QWidget *w) { current = w; }
    static QWidget* currentWindow() { return current; }
private:
    static QWidget *current;
};

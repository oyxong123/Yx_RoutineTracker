#ifndef ROUTINESTRUCT_H
#define ROUTINESTRUCT_H

#include <QString>
#include <QList>

struct Routine {
    int id;
    QString name;
    QString type;
    QString type_param;
    int priority;
};

struct RoutineGroup {
    QString name;
    QList<Routine*> content;

    RoutineGroup(const QString& title) : name(title) {}
};

#endif // ROUTINESTRUCT_H

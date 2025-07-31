PRAGMA foreign_keys = ON;

create table routine(
    id INTEGER PRIMARY KEY, 
    name TEXT NOT NULL, 
    type TEXT NOT NULL, 
    type_param TEXT NULL, 
    is_active INTEGER NOT NULL, 
    priority INTEGER NOT NULL
);

create table records(
    id INTEGER PRIMARY KEY, 
    date TEXT NOT NULL, 
    routine_id INTEGER NOT NULL REFERENCES routine(id) ON DELETE CASCADE ON UPDATE CASCADE
);

create index idx_records_routine_id ON records(routine_id);
create index idx_records_date ON records(date);
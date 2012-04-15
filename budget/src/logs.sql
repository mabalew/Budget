CREATE TABLE logs(
	id integer primary key autoincrement,
	date datetime DEFAULT CURRENT_TIMESTAMP,
	level integer DEFAULT 0,
	message varchar2(255)
);

CREATE TABLE log_levels(
	level_id integer not null unique,
	level varchar(10) not null unique
);

CREATE VIEW v_logs as
	select datetime(date, 'localtime') as date, ll.level, message from logs l, log_levels ll
	where l.level=ll.level_id
	order by date;

CREATE VIEW v_debug_logs as
	select * from v_logs where level='DEBUG'
	order by date;

CREATE VIEW v_trace_logs as
	select * from v_logs where level='TRACE'
	order by date;

CREATE VIEW v_info_logs as
	select * from v_logs where level='INFO'
	order by date;

CREATE VIEW v_warn_logs as
	select * from v_logs where level='WARN'
	order by date;

CREATE VIEW v_error_logs as
	select * from v_logs where level='ERROR'
	order by date;

INSERT INTO "log_levels" VALUES(0,'TRACE');
INSERT INTO "log_levels" VALUES(1,'DEBUG');
INSERT INTO "log_levels" VALUES(2,'INFO');
INSERT INTO "log_levels" VALUES(3,'WARN');
INSERT INTO "log_levels" VALUES(4,'ERROR');

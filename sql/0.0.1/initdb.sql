CREATE TABLE `ut4webadmin_chat` (
	`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	`time`	TEXT,
	`sender_name`	TEXT,
	`sender_uid`	TEXT,
	`sender_team_num`	INTEGER,
	`message`	TEXT
);
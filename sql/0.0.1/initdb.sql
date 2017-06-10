CREATE TABLE `ut4webadmin_chat` (
	`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	`time`	TEXT,
	`sender_name`	TEXT,
	`sender_uid`	TEXT,
	`sender_team_num`	INTEGER DEFAULT 255,
	`message`	TEXT
);

CREATE INDEX `ut4webadmin_chat_uid_idx` ON `ut4webadmin_chat` (`sender_uid` ASC);
CREATE INDEX `ut4webadmin_chat_name_idx` ON `ut4webadmin_chat` (`sender_name` ASC);
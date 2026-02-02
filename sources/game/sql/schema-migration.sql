---@proc schema_migration
CREATE TABLE IF NOT EXISTS games (
  id         INTEGER  PRIMARY KEY AUTOINCREMENT,
  name       TEXT     NOT NULL,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

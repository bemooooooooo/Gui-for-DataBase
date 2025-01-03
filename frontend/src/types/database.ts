export type RelationType = 'one-to-one' | 'one-to-many' | 'many-to-one' | 'many-to-many';

export interface Relation {
  sourceId: string;
  targetId: string;
  type: RelationType;
}

export interface Column {
  id: string;
  name: string;
  type: string;
  isPrimaryKey: boolean;
  isNotNull: boolean;
  defaultValue?: string;
  references?: {
    tableId: string;
    columnId: string;
    type: RelationType;
  };
}

export interface Table {
  id: string;
  name: string;
  columns: Column[];
  position: {
    x: number;
    y: number;
  };
}

export interface DatabaseConfig {
  name: string;
  tables: Table[];
}

export interface DeploymentConfig {
  config_id: string;
  serverConfig: ServerConfig;
  databaseConfig: DatabaseConfig;
}

export interface DeploymentStatus {
  status: string;
  message: string;
  details?: {
    host: string;
    port: number;
    database: string;
    timestamp: string;
  };
}

export interface ServerConfig {
  host: string;
  port: number;
  username: string;
  password: string;
  databaseType: 'PostgreSQL' | 'MySQL' | 'Redis';
  database: string;
  additionalConfig?: Record<string, string>;
}

export interface ValidationError {
  field: string;
  message: string;
}

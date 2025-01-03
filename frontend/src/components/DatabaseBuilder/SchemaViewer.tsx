import React from 'react';
import { Table } from '../../types/database';

interface SchemaViewerProps {
    tables: Table[];
}

const SchemaViewer: React.FC<SchemaViewerProps> = ({ tables }) => {
    return (
        <div className="space-y-6">
            {tables.map((table) => (
                <div
                    key={table.id}
                    className="bg-white p-4 rounded-lg shadow"
                >
                    <h3 className="text-lg font-semibold mb-2">{table.name}</h3>
                    <div className="space-y-2">
                        {table.columns.map((column) => (
                            <div
                                key={column.id}
                                className="flex items-center justify-between py-1"
                            >
                                <div className="flex-1">
                                    <span className="font-medium">{column.name}</span>
                                    <div className="flex items-center space-x-2 mt-1">
                                        <span className="text-gray-600">{column.type}</span>
                                        {column.isNotNull && (
                                            <span className="text-xs bg-red-100 text-red-800 px-2 py-1 rounded">
                                                NOT NULL
                                            </span>
                                        )}
                                        {column.isPrimaryKey && (
                                            <span className="text-xs bg-blue-100 text-blue-800 px-2 py-1 rounded">
                                                PRIMARY KEY
                                            </span>
                                        )}
                                        {column.defaultValue && (
                                            <span className="text-xs bg-gray-100 text-gray-800 px-2 py-1 rounded">
                                                DEFAULT: {column.defaultValue}
                                            </span>
                                        )}
                                    </div>
                                </div>
                            </div>
                        ))}
                    </div>
                </div>
            ))}
        </div>
    );
};

export default SchemaViewer;

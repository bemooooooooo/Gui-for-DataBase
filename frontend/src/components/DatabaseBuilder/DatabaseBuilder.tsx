import React, { useState, useRef, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import TableBlock from './TableBlock';
import Xarrow from 'react-xarrows';
import { Table, RelationType } from '../../types/database';
import { Button, Input } from 'antd';
import { PlusOutlined } from '@ant-design/icons';

const DatabaseBuilder: React.FC = () => {
    const navigate = useNavigate();
    const [tables, setTables] = useState<Table[]>([]);
    const [dbName, setDbName] = useState('');
    const [isMobile, setIsMobile] = useState(window.innerWidth <= 768);
    const [tempRelation, setTempRelation] = useState<{ source: string; mousePosition: { x: number; y: number } } | null>(null);
    const [relations, setRelations] = useState<Array<{ sourceId: string; targetId: string; type: RelationType }>>([]);
    const containerRef = useRef<HTMLDivElement>(null);
    const tempEndRef = useRef<HTMLDivElement>(null);
    const [containerPosition, setContainerPosition] = useState({ x: 0, y: 0 });
    const [isDragging, setIsDragging] = useState(false);
    const [startDragPosition, setStartDragPosition] = useState({ x: 0, y: 0 });

    useEffect(() => {
        const handleResize = () => {
            setIsMobile(window.innerWidth <= 768);
        };
        window.addEventListener('resize', handleResize);
        return () => window.removeEventListener('resize', handleResize);
    }, []);

    useEffect(() => {
        const handleMouseMove = (e: MouseEvent) => {
            if (tempRelation) {
                const rect = containerRef.current?.getBoundingClientRect();
                if (rect) {
                    setTempRelation({
                        ...tempRelation,
                        mousePosition: {
                            x: e.clientX - rect.left - containerPosition.x,
                            y: e.clientY - rect.top - containerPosition.y
                        }
                    });
                }
            }
            if (isDragging && containerRef.current) {
                const deltaX = e.clientX - startDragPosition.x;
                const deltaY = e.clientY - startDragPosition.y;
                setContainerPosition(prev => ({
                    x: prev.x + deltaX,
                    y: prev.y + deltaY
                }));
                setStartDragPosition({ x: e.clientX, y: e.clientY });
            }
        };

        const handleMouseUp = () => {
            setIsDragging(false);
        };

        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);
        return () => {
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
        };
    }, [tempRelation, isDragging, startDragPosition]);

    const handleStartRelation = (tableId: string, columnId: string) => {
        setTempRelation({
            source: `${tableId}-${columnId}`,
            mousePosition: { x: 0, y: 0 }
        });
    };

    const handleEndRelation = (targetTableId: string, targetColumnId: string) => {
        if (tempRelation) {
            const [sourceTableId, sourceColumnId] = tempRelation.source.split('-');
            
            // Add the new relation
            setRelations(prev => [...prev, {
                sourceId: tempRelation.source,
                targetId: `${targetTableId}-${targetColumnId}`,
                type: 'one-to-many' as RelationType
            }]);

            // Update the source column with the reference
            setTables(prev => prev.map(table => {
                if (table.id === sourceTableId) {
                    return {
                        ...table,
                        columns: table.columns.map(column => {
                            if (column.id === sourceColumnId) {
                                return {
                                    ...column,
                                    references: {
                                        tableId: targetTableId,
                                        columnId: targetColumnId,
                                        type: 'one-to-many'
                                    }
                                };
                            }
                            return column;
                        })
                    };
                }
                return table;
            }));

            setTempRelation(null);
        }
    };

    const handleAddTable = () => {
        const newId = `table-${Date.now()}`;
        const newTable: Table = {
            id: newId,
            name: `Table ${tables.length + 1}`,
            position: {
                x: Math.random() * 500,
                y: Math.random() * 500
            },
            columns: [{
                id: `column-${Date.now()}`,
                name: 'id',
                type: 'INT',
                isPrimaryKey: true,
                isNotNull: true
            }]
        };
        setTables([...tables, newTable]);
    };

    const handleMouseDown = (e: React.MouseEvent) => {
        if (e.target === containerRef.current) {
            setIsDragging(true);
            setStartDragPosition({ x: e.clientX, y: e.clientY });
        }
    };

    return (
        <div style={{ height: '100vh', display: 'flex', flexDirection: 'column' }}>
            <div className="header" style={{ 
                padding: '1rem',
                borderBottom: '1px solid #e5e7eb',
                backgroundColor: '#fff',
                zIndex: 1000,
                display: 'flex',
                justifyContent: 'space-between',
                alignItems: 'center'
            }}>
                <div style={{ display: 'flex', alignItems: 'center', gap: '1rem' }}>
                    <h1 style={{ margin: 0 }}>Database Builder</h1>
                    <Input
                        placeholder="Enter database name"
                        value={dbName}
                        onChange={(e) => setDbName(e.target.value)}
                        style={{ width: 200 }}
                    />
                </div>
                <Button type="primary">Save</Button>
            </div>

            <div style={{ 
                position: 'relative',
                flex: 1,
                overflow: 'hidden',
                backgroundColor: '#F7FAFC',
                cursor: isDragging ? 'grabbing' : 'grab'
            }}>
                <div
                    ref={containerRef}
                    onMouseDown={handleMouseDown}
                    style={{
                        position: 'absolute',
                        width: '5000px',
                        height: '5000px',
                        transform: `translate(${containerPosition.x}px, ${containerPosition.y}px)`,
                        backgroundImage: 'radial-gradient(#E2E8F0 1px, transparent 1px)',
                        backgroundSize: '20px 20px',
                    }}
                >
                    {tables.map(table => (
                        <TableBlock
                            key={table.id}
                            table={table}
                            onDelete={(id) => setTables(tables.filter(t => t.id !== id))}
                            onColumnAdd={(tableId) => {
                                setTables(tables.map(t => {
                                    if (t.id === tableId) {
                                        return {
                                            ...t,
                                            columns: [...t.columns, {
                                                id: `column-${Date.now()}`,
                                                name: `Column ${t.columns.length + 1}`,
                                                type: 'varchar',
                                                isPrimaryKey: false,
                                                isNotNull: false
                                            }]
                                        };
                                    }
                                    return t;
                                }));
                            }}
                            onColumnDelete={(tableId, columnId) => {
                                setTables(tables.map(t => {
                                    if (t.id === tableId) {
                                        return {
                                            ...t,
                                            columns: t.columns.filter(c => c.id !== columnId)
                                        };
                                    }
                                    return t;
                                }));
                            }}
                            onColumnUpdate={(tableId, columnId, updates) => {
                                setTables(tables.map(t => {
                                    if (t.id === tableId) {
                                        return {
                                            ...t,
                                            columns: t.columns.map(c => {
                                                if (c.id === columnId) {
                                                    return { ...c, ...updates };
                                                }
                                                return c;
                                            })
                                        };
                                    }
                                    return t;
                                }));
                            }}
                            onPositionChange={(tableId, newPosition) => {
                                setTables(tables.map(t => {
                                    if (t.id === tableId) {
                                        return { ...t, position: newPosition };
                                    }
                                    return t;
                                }));
                            }}
                            onStartRelation={handleStartRelation}
                            onEndRelation={handleEndRelation}
                            isMobile={isMobile}
                        />
                    ))}
                    {relations.map((relation, index) => (
                        <Xarrow
                            key={index}
                            start={relation.sourceId}
                            end={relation.targetId}
                            startAnchor="right"
                            endAnchor="left"
                            color="#4A5568"
                            strokeWidth={1}
                            path="straight"
                            showHead={true}
                            headSize={6}
                        />
                    ))}
                    {tempRelation && (
                        <div
                            ref={tempEndRef}
                            style={{
                                position: 'absolute',
                                left: tempRelation.mousePosition.x,
                                top: tempRelation.mousePosition.y,
                                width: 1,
                                height: 1
                            }}
                        />
                    )}
                    {tempRelation && (
                        <Xarrow
                            start={tempRelation.source}
                            end={tempEndRef}
                            startAnchor="right"
                            endAnchor="left"
                            color="#4A5568"
                            strokeWidth={1}
                            path="straight"
                            showHead={true}
                            headSize={6}
                        />
                    )}
                </div>

                <Button
                    type="primary"
                    onClick={handleAddTable}
                    icon={<PlusOutlined />}
                    style={{
                        position: 'fixed',
                        top: '80px',
                        right: '20px',
                        backgroundColor: '#2D3748',
                        borderColor: '#2D3748',
                        boxShadow: '0 2px 4px rgba(0, 0, 0, 0.1)',
                        zIndex: 1000
                    }}
                >
                    Add Table
                </Button>
            </div>
        </div>
    );
};

export default DatabaseBuilder;

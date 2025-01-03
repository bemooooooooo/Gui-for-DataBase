import React, { useState, useEffect, useRef } from 'react';
import { Button, Input, Select, Menu, Dropdown, Modal } from 'antd';
import { CloseOutlined, PlusOutlined, EllipsisOutlined } from '@ant-design/icons';
import Draggable from 'react-draggable';
import { Table, Column, RelationType } from '../../types/database';

interface TableBlockProps {
    table: Table;
    onDelete: (id: string) => void;
    onColumnAdd: (tableId: string) => void;
    onColumnDelete: (tableId: string, columnId: string) => void;
    onColumnUpdate: (tableId: string, columnId: string, updates: Partial<Column>) => void;
    onPositionChange: (tableId: string, newPosition: { x: number; y: number }) => void;
    onStartRelation: (tableId: string, columnId: string) => void;
    onEndRelation: (tableId: string, columnId: string) => void;
    isMobile: boolean;
}

const TableBlock: React.FC<TableBlockProps> = ({ 
    table, 
    onDelete, 
    onColumnAdd, 
    onColumnDelete, 
    onColumnUpdate, 
    onStartRelation,
    onEndRelation,
    onPositionChange,
    isMobile
}) => {
    const dimensions = {
        width: isMobile ? 200 : 250,
        headerHeight: isMobile ? 40 : 50,
        rowHeight: isMobile ? 25 : 30
    };

    const [isDraggingColumn, setIsDraggingColumn] = useState(false);
    const [draggedColumn, setDraggedColumn] = useState<Column | null>(null);
    const [mousePosition, setMousePosition] = useState({ x: 0, y: 0 });
    const [relationModalVisible, setRelationModalVisible] = useState(false);
    const [selectedRelationType, setSelectedRelationType] = useState<RelationType>('one-to-many');
    const [tempRelation, setTempRelation] = useState<{
        source: { columnId: string };
        target: { columnId: string };
    } | null>(null);
    const [isEditingTableName, setIsEditingTableName] = useState(false);
    const [editingColumnId, setEditingColumnId] = useState<string | null>(null);

    useEffect(() => {
        const handleMouseMove = (e: MouseEvent) => {
            if (isDraggingColumn) {
                setMousePosition({ x: e.clientX, y: e.clientY });
            }
        };

        document.addEventListener('mousemove', handleMouseMove);
        return () => {
            document.removeEventListener('mousemove', handleMouseMove);
        };
    }, [isDraggingColumn]);

    const handleColumnNameChange = (columnId: string, newName: string) => {
        onColumnUpdate(table.id, columnId, { name: newName });
    };

    const handleColumnTypeChange = (columnId: string, newType: string) => {
        onColumnUpdate(table.id, columnId, { type: newType });
    };

    const handleColumnDragStart = (column: Column) => {
        setIsDraggingColumn(true);
        setDraggedColumn(column);
    };

    const handleColumnDrop = (targetColumn: Column) => {
        if (draggedColumn && draggedColumn.id !== targetColumn.id) {
            setTempRelation({
                source: { columnId: draggedColumn.id },
                target: { columnId: targetColumn.id }
            });
            setRelationModalVisible(true);
        }
        setIsDraggingColumn(false);
        setDraggedColumn(null);
    };

    const handleRelationModalOk = () => {
        setRelationModalVisible(false);
        setTempRelation(null);
        setSelectedRelationType('one-to-many');
    };

    const getColumnMenu = (column: Column) => (
        <Menu>
            {column.id !== table.columns[0].id && (
                <Menu.Item 
                    key="delete" 
                    danger 
                    onClick={() => onColumnDelete(table.id, column.id)}
                >
                    Delete Column
                </Menu.Item>
            )}
            <Menu.Item
                key="primaryKey"
                onClick={() => onColumnUpdate(table.id, column.id, { isPrimaryKey: !column.isPrimaryKey })}
            >
                {column.isPrimaryKey ? 'Remove Primary Key' : 'Set as Primary Key'}
            </Menu.Item>
            <Menu.Item
                key="createRelation"
                onClick={() => onStartRelation(table.id, column.id)}
            >
                Create Relation
            </Menu.Item>
        </Menu>
    );

    return (
        <>
            <Draggable
                position={table.position}
                onDrag={(e, data) => onPositionChange(table.id, { x: data.x, y: data.y })}
                bounds="parent"
                handle=".table-header"
            >
                <div style={{ 
                    position: 'relative',
                    display: 'inline-block',
                    minWidth: isMobile ? '200px' : '250px',
                    maxWidth: isMobile ? '280px' : '350px',
                    backgroundColor: '#ffffff',
                    borderRadius: '4px',
                    boxShadow: '0 2px 4px rgba(0, 0, 0, 0.1)',
                    border: '1px solid #e0e0e0'
                }}>
                    <div className="table-header" style={{
                        padding: '8px 12px',
                        backgroundColor: '#2D3748',
                        color: '#ffffff',
                        borderTopLeftRadius: '4px',
                        borderTopRightRadius: '4px',
                        display: 'flex',
                        justifyContent: 'space-between',
                        alignItems: 'center',
                        height: '40px',
                        position: 'relative',
                        width: '100%',
                        cursor: 'move'
                    }}>
                        <div style={{ 
                            fontWeight: 'bold',
                            fontSize: '14px',
                            overflow: 'hidden',
                            textOverflow: 'ellipsis',
                            whiteSpace: 'nowrap',
                            maxWidth: '90%',
                            paddingRight: '30px'
                        }}>
                            {isEditingTableName ? (
                                <Input
                                    value={table.name}
                                    onChange={e => onColumnUpdate(table.id, table.columns[0].id, { name: e.target.value })}
                                    onBlur={() => setIsEditingTableName(false)}
                                    onPressEnter={() => setIsEditingTableName(false)}
                                    autoFocus
                                    style={{
                                        backgroundColor: 'transparent',
                                        color: '#ffffff',
                                        border: 'none',
                                        borderBottom: '1px solid #ffffff',
                                        borderRadius: 0
                                    }}
                                />
                            ) : (
                                <span
                                    onDoubleClick={() => setIsEditingTableName(true)}
                                    style={{ cursor: 'text' }}
                                >
                                    {table.name}
                                </span>
                            )}
                        </div>
                        <div style={{ position: 'absolute', right: '8px', display: 'flex', gap: '4px' }}>
                            <Button
                                type="text"
                                icon={<PlusOutlined style={{ color: '#ffffff', fontSize: '12px' }} />}
                                onClick={() => onColumnAdd(table.id)}
                                style={{
                                    padding: '4px',
                                    height: 'auto',
                                    minWidth: '24px',
                                    border: 'none'
                                }}
                            />
                            <Button
                                type="text"
                                danger
                                icon={<CloseOutlined style={{ color: '#ffffff', fontSize: '12px' }} />}
                                onClick={() => onDelete(table.id)}
                                style={{
                                    padding: '4px',
                                    height: 'auto',
                                    minWidth: '24px',
                                    border: 'none'
                                }}
                            />
                        </div>
                    </div>
                    <div style={{ 
                        padding: '0',
                        display: 'flex',
                        flexDirection: 'column',
                        width: '100%'
                    }}>
                        {table.columns.map((column, index) => (
                            <div
                                key={column.id}
                                style={{
                                    display: 'flex',
                                    alignItems: 'center',
                                    gap: '8px',
                                    padding: '8px 12px',
                                    backgroundColor: index % 2 === 0 ? '#f8f9fa' : '#ffffff',
                                    borderBottom: '1px solid #e0e0e0',
                                    height: '36px',
                                    fontSize: '13px',
                                    width: '100%'
                                }}
                                draggable
                                onDragStart={() => handleColumnDragStart(column)}
                                onDragOver={(e) => e.preventDefault()}
                                onDrop={() => handleColumnDrop(column)}
                            >
                                {editingColumnId === column.id ? (
                                    <Input
                                        value={column.name}
                                        onChange={e => handleColumnNameChange(column.id, e.target.value)}
                                        onBlur={() => setEditingColumnId(null)}
                                        onPressEnter={() => setEditingColumnId(null)}
                                        autoFocus
                                        style={{ 
                                            flex: '1 1 auto',
                                            minWidth: '120px',
                                            height: '28px',
                                            padding: '2px 8px',
                                            fontSize: '13px'
                                        }}
                                    />
                                ) : (
                                    <div
                                        id={`column-${column.id}`}
                                        onDoubleClick={() => setEditingColumnId(column.id)}
                                        style={{
                                            flex: '1 1 auto',
                                            minWidth: '120px',
                                            padding: '2px 8px',
                                            cursor: 'text',
                                            whiteSpace: 'nowrap',
                                            overflow: 'hidden',
                                            textOverflow: 'ellipsis',
                                            display: 'flex',
                                            alignItems: 'center',
                                            gap: '6px'
                                        }}
                                    >
                                        {column.isPrimaryKey && (
                                            <span style={{ 
                                                color: '#4A5568',
                                                fontSize: '11px',
                                                fontWeight: 'bold'
                                            }}>
                                                PK
                                            </span>
                                        )}
                                        {column.name}
                                    </div>
                                )}
                                <Select
                                    value={column.type}
                                    onChange={value => handleColumnTypeChange(column.id, value)}
                                    style={{ 
                                        width: '90px',
                                        fontSize: '13px',
                                        flex: '0 0 auto'
                                    }}
                                    size="small"
                                >
                                    <Select.Option value="varchar">VARCHAR</Select.Option>
                                    <Select.Option value="int">INT</Select.Option>
                                    <Select.Option value="boolean">BOOL</Select.Option>
                                    <Select.Option value="date">DATE</Select.Option>
                                </Select>
                                <Dropdown overlay={getColumnMenu(column)} trigger={['click']}>
                                    <Button
                                        type="text"
                                        icon={<EllipsisOutlined style={{ fontSize: '14px', color: '#4A5568' }} />}
                                        style={{ 
                                            padding: '2px 4px',
                                            height: '24px',
                                            minWidth: '24px'
                                        }}
                                    />
                                </Dropdown>
                            </div>
                        ))}
                    </div>
                </div>
            </Draggable>

            {isDraggingColumn && draggedColumn && (
                <div
                    style={{
                        position: 'fixed',
                        top: 0,
                        left: 0,
                        width: '100%',
                        height: '100%',
                        pointerEvents: 'none',
                        zIndex: 9999
                    }}
                >
                    <svg style={{ width: '100%', height: '100%' }}>
                        <line
                            x1={table.position.x + dimensions.width}
                            y1={table.position.y + dimensions.headerHeight / 2}
                            x2={mousePosition.x}
                            y2={mousePosition.y}
                            stroke="#6366f1"
                            strokeWidth="2"
                            strokeDasharray="5,5"
                        />
                    </svg>
                </div>
            )}

            <Modal
                title="Select Relation Type"
                open={relationModalVisible}
                onOk={handleRelationModalOk}
                onCancel={() => {
                    setRelationModalVisible(false);
                    setTempRelation(null);
                }}
            >
                <Select
                    value={selectedRelationType}
                    onChange={setSelectedRelationType}
                    style={{ width: '100%' }}
                >
                    <Select.Option value="one-to-many">One-to-Many</Select.Option>
                    <Select.Option value="many-to-one">Many-to-One</Select.Option>
                    <Select.Option value="many-to-many">Many-to-Many</Select.Option>
                </Select>
            </Modal>
        </>
    );
};

export default TableBlock;

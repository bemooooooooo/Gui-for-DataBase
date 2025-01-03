import React from 'react';
import { Form, Input, Button, message } from 'antd';
import { ServerConfig } from '../../types/database';

interface DeploymentFormProps {
    onDeploy: (serverConfig: ServerConfig) => Promise<void>;
}

const DeploymentForm: React.FC<DeploymentFormProps> = ({ onDeploy }) => {
    const [form] = Form.useForm();
    const [loading, setLoading] = React.useState(false);

    const handleSubmit = async (values: ServerConfig) => {
        try {
            setLoading(true);
            await onDeploy(values);
            message.success('Deployment started successfully');
            form.resetFields();
        } catch (error) {
            message.error('Failed to start deployment');
            console.error('Deployment error:', error);
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="p-4 max-w-2xl mx-auto">
            <h1 className="text-2xl font-bold mb-6">Deploy Database</h1>
            
            <Form
                form={form}
                layout="vertical"
                onFinish={handleSubmit}
                className="space-y-4"
            >
                <Form.Item
                    label="Host"
                    name="host"
                    rules={[{ required: true, message: 'Please enter the host' }]}
                >
                    <Input placeholder="localhost" />
                </Form.Item>

                <Form.Item
                    label="Port"
                    name="port"
                    rules={[{ required: true, message: 'Please enter the port' }]}
                >
                    <Input type="number" placeholder="5432" />
                </Form.Item>

                <Form.Item
                    label="Database"
                    name="database"
                    rules={[{ required: true, message: 'Please enter the database name' }]}
                >
                    <Input placeholder="my_database" />
                </Form.Item>

                <Form.Item
                    label="Username"
                    name="username"
                    rules={[{ required: true, message: 'Please enter the username' }]}
                >
                    <Input placeholder="postgres" />
                </Form.Item>

                <Form.Item
                    label="Password"
                    name="password"
                    rules={[{ required: true, message: 'Please enter the password' }]}
                >
                    <Input.Password />
                </Form.Item>

                <Form.Item>
                    <Button type="primary" htmlType="submit" loading={loading} block>
                        Deploy Database
                    </Button>
                </Form.Item>
            </Form>

            <div className="mt-8">
                <h2 className="text-lg font-semibold mb-4">Deployment Instructions</h2>
                <ol className="list-decimal list-inside space-y-2">
                    <li>Select a previously saved database configuration</li>
                    <li>Enter the target server's connection details</li>
                    <li>Make sure the server has SSH access enabled</li>
                    <li>Ensure the server has sufficient permissions to install and configure databases</li>
                    <li>Click "Deploy Database" to start the deployment process</li>
                </ol>
            </div>
        </div>
    );
};

export default DeploymentForm;

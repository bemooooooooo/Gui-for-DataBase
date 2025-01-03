import React, { useState } from 'react';
import { BrowserRouter as Router, Route, Routes, Navigate } from 'react-router-dom';
import DatabaseBuilder from './components/DatabaseBuilder/DatabaseBuilder';
import DeploymentForm from './components/DeploymentForm/DeploymentForm';
import { ServerConfig, DatabaseConfig } from './types/database';

function App() {
  const [databaseConfig] = useState<DatabaseConfig | null>(null);

  const handleDeploy = async (serverConfig: ServerConfig) => {
    if (!databaseConfig) return;

    const deploymentConfig = {
      database: databaseConfig,
      server: serverConfig
    };

    try {
      const response = await fetch('http://localhost:8080/api/database/deploy', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(deploymentConfig)
      });

      if (!response.ok) {
        throw new Error('Deployment failed');
      }

      const result = await response.json();
      console.log('Deployment result:', result);
    } catch (error) {
      console.error('Error deploying database:', error);
    }
  };

  return (
    <div style={{ height: '100%', width: '100%', overflow: 'hidden' }}>
      <Router>
        <Routes>
          <Route path="/deploy" element={<DeploymentForm onDeploy={handleDeploy} />} />
          <Route path="/builder" element={<DatabaseBuilder />} />
          <Route path="/" element={<Navigate to="/builder" replace />} />
        </Routes>
      </Router>
    </div>
  );
}

export default App;

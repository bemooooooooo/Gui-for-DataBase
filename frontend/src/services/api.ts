import axios from 'axios';
import { DatabaseConfig, DeploymentConfig } from '../types/database';

interface LoginResponse {
    token: string;
}

interface DeploymentResponse {
    status: string;
    message: string;
}

class Api {
    private baseURL: string;
    private api;

    constructor() {
        this.baseURL = process.env.REACT_APP_API_URL || 'http://localhost:8080';
        this.api = axios.create({
            baseURL: this.baseURL,
            headers: {
                'Content-Type': 'application/json'
            }
        });

        this.setupInterceptors();
    }

    private setupInterceptors() {
        this.api.interceptors.request.use(
            (config) => {
                const token = localStorage.getItem('token');
                if (token && config.headers) {
                    config.headers.Authorization = `Bearer ${token}`;
                }
                return config;
            },
            (error) => {
                return Promise.reject(error);
            }
        );

        this.api.interceptors.response.use(
            (response) => response,
            async (error) => {
                if (error.response?.status === 401) {
                    localStorage.removeItem('token');
                    window.location.href = '/login';
                }
                return Promise.reject(error);
            }
        );
    }

    async login(username: string, password: string): Promise<string> {
        const response = await this.api.post<LoginResponse>('/auth/login', { username, password });
        const { token } = response.data;
        localStorage.setItem('token', token);
        return token;
    }

    async register(username: string, email: string, password: string): Promise<void> {
        await this.api.post('/auth/register', { username, email, password });
    }

    async logout(): Promise<void> {
        localStorage.removeItem('token');
    }

    async saveDatabaseConfig(config: DatabaseConfig): Promise<DatabaseConfig> {
        const response = await this.api.post<DatabaseConfig>('/database/config', config);
        return response.data;
    }

    async getDatabaseConfigs(): Promise<DatabaseConfig[]> {
        const response = await this.api.get<DatabaseConfig[]>('/database/configs');
        return response.data;
    }

    async deployDatabase(config: DeploymentConfig): Promise<DeploymentResponse> {
        const response = await this.api.post<DeploymentResponse>('/database/deploy', config);
        return response.data;
    }
}

export const api = new Api();

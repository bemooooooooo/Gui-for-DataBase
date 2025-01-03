const { override } = require('customize-cra');

module.exports = override(
    (config, env) => {
        if (env === 'development') {
            config.devServer = {
                ...config.devServer,
                setupMiddlewares: (middlewares, devServer) => {
                    if (!devServer) {
                        throw new Error('webpack-dev-server is not defined');
                    }

                    // Здесь можно добавить любую дополнительную настройку middleware
                    
                    return middlewares;
                }
            };
        }
        return config;
    }
);

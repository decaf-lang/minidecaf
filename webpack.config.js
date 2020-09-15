const path = require('path');

module.exports = (env, argv) => {
    var config = {
        mode: "development",
        entry: './src/minidecaf.ts',
        output: {
            filename: 'dist/minidecaf.bundle.js',
            library: 'MiniDecaf',
            path: path.resolve(__dirname, 'web')
        },
        module: {
            rules: [{
                test: /\.ts$/,
                use: "ts-loader"
            }]
        },
        resolve: {
            extensions: ['.ts', '.js']
        },
    };
    if (argv.mode === 'development') {
        config.devtool = 'inline-source-map';
    }
    return config;
};

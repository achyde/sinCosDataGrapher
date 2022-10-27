process.env.DIST_ELECTRON = join(__dirname, '..')
process.env.DIST = join(process.env.DIST_ELECTRON, '../dist')
process.env.PUBLIC = app.isPackaged ? process.env.DIST : join(process.env.DIST_ELECTRON, '../public')

import {app, BrowserWindow} from 'electron'
import {join} from 'path'
import dgram from 'node:dgram';

const udp_port = dgram.createSocket('udp4');

udp_port.on('error', (err) => {
    console.log(`udp_port error:\n${err.stack}`);
    udp_port.close();
});

udp_port.on('listening', () => {
    const address = udp_port.address();
    console.log(`udp_port listening ${address.address}:${address.port}`);
    udp_port.setBroadcast(true);
});

udp_port.bind(9999);

let win: BrowserWindow | null = null
const url = process.env.VITE_DEV_SERVER_URL as string
const indexHtml = join(process.env.DIST, 'index.html')

async function createWindow() {
    win = new BrowserWindow({
        title: 'Main window',
        webPreferences: {preload: join(__dirname, '../preload/index.js')},
    });
    
    if (app.isPackaged) {
        win.loadFile(indexHtml)
    } else {
        win.loadURL(url)
        win.maximize();
    }
}

app.whenReady().then(createWindow).then(() => {
    udp_port.on('message', msg => win.webContents.send("udp_data", new Float64Array(msg.buffer)));
})

const { app, BrowserWindow, ipcMain, dialog, Menu, MenuItem } = require('electron');
const path = require('path');
const { execFile } = require('child_process');
const fs = require('fs');

function createWindow() {
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    title: "नव-सङ्गणक-शाला (Nava Studio) v2.0",
    icon: path.join(__dirname, 'public/favicon.ico'),
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    }
  });

  win.loadFile('src/index.html');
  
  // Set Context Menu
  const ctxMenu = new Menu();
  ctxMenu.append(new MenuItem({ label: 'पठतु (Copy)', role: 'copy' }));
  ctxMenu.append(new MenuItem({ label: 'लिखतु (Paste)', role: 'paste' }));
  ctxMenu.append(new MenuItem({ type: 'separator' }));
  ctxMenu.append(new MenuItem({ label: 'सर्वं वृणोतु (Select All)', role: 'selectAll' }));

  win.webContents.on('context-menu', (e, params) => {
    ctxMenu.popup({ window: win, x: params.x, y: params.y });
  });
}

app.whenReady().then(() => {
  createWindow();

  // Create Standard Application Menu (PURE SANSKRIT)
  const template = [
    {
      label: 'सम्पादनम् (Edit)',
      submenu: [
        { label: 'पूर्ववत् (Undo)', role: 'undo' },
        { label: 'पुनरावृत्तिः (Redo)', role: 'redo' },
        { type: 'separator' },
        { label: 'कर्तनम् (Cut)', role: 'cut' },
        { label: 'पठतु (Copy)', role: 'copy' },
        { label: 'लिखतु (Paste)', role: 'paste' },
        { label: 'सर्वं वृणोतु (Select All)', role: 'selectAll' }
      ]
    },
    {
      label: 'दृश्यम् (View)',
      submenu: [
        { label: 'पुनर्भारणम् (Reload)', role: 'reload' },
        { label: 'निरीक्षकः (DevTools)', role: 'toggleDevTools' }
      ]
    }
  ];

  const menu = Menu.buildFromTemplate(template);
  Menu.setApplicationMenu(menu);

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});

// IPC Handler to run Nava compiler
ipcMain.on('run-code', (event, code) => {
  const tempFilePath = path.join(app.getPath('temp'), 'temp.ns');
  const binPath = app.isPackaged 
      ? path.join(process.resourcesPath, 'bin', 'nvc.exe') 
      : path.join(__dirname, '..', 'bin', 'nvc.exe');
  
  fs.writeFileSync(tempFilePath, code);

  execFile(binPath, [tempFilePath, '--run'], (error, stdout, stderr) => {
    if (error) {
      event.reply('run-result', { success: false, output: stderr || error.message });
      return;
    }
    event.reply('run-result', { success: true, output: stdout });
  });
});

// IPC Handler to save file
ipcMain.on('save-file', (event, code) => {
  const result = dialog.showSaveDialogSync({
    title: 'नव-सङ्गणक-शाला - सञ्चिका-संग्रहः',
    defaultPath: path.join(app.getPath('documents'), 'file.ns'),
    filters: [
      { name: 'Nava Sanskritam', extensions: ['ns'] },
      { name: 'All Files', extensions: ['*'] }
    ]
  });

  if (result) {
    fs.writeFileSync(result, code);
  }
});

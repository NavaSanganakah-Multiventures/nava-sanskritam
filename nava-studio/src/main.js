const { app, BrowserWindow, ipcMain, dialog, Menu } = require('electron');
const path = require('path');
const { execFile } = require('child_process');
const fs = require('fs');

function createWindow() {
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    title: "Nava Studio v1.0",
    icon: path.join(__dirname, 'public/favicon.ico'),
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    }
  });

  win.loadFile('src/index.html');
  // win.webContents.openDevTools(); // Uncomment for debugging
}

app.whenReady().then(() => {
  createWindow();

  // Create Standard Application Menu for Copy/Paste
  const template = [
    {
      label: 'Edit',
      submenu: [
        { role: 'undo' },
        { role: 'redo' },
        { type: 'separator' },
        { role: 'cut' },
        { role: 'copy' },
        { role: 'paste' },
        { role: 'selectAll' }
      ]
    },
    {
      label: 'View',
      submenu: [
        { role: 'reload' },
        { role: 'toggleDevTools' }
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
    title: 'Nava Sanskritam - Save Code',
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

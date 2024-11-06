#define MyAppVersion "0.1.0.0"
#define MyAppName "FaceRecognition"
#define MyAppPublisher "Your Company Name"
#define MyAppDescription "Face Recognition Application"

#define MyAppVersion "0.1.0.0"
#define MyAppName "FaceRecognition"
#define MyAppPublisher "Your Company Name"
#define MyAppDescription "Face Recognition Application"
[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DefaultDirName={commonpf64}\{#MyAppName}
DefaultGroupName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppName}.exe
MinVersion=6.1sp1
OutputDir=..\..\
OutputBaseFilename={#MyAppName}_Setup_{#MyAppVersion}
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
SetupIconFile=..\UI\Resources\app.ico
[Files]
Source: "..\UI\Resources\app.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\bin\Debug\{#MyAppName}.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\bin\Debug\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\bin\Debug\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs
Source: "..\..\build\bin\Debug\models\*"; DestDir: "{app}\models"; Flags: ignoreversion recursesubdirs
Source: "..\..\build\bin\Debug\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs
Source: "..\..\build\bin\Debug\config.ini"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppName}.exe"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppName}.exe"
[Run]
Filename: "{app}\{#MyAppName}.exe"; Description: "Launch {#MyAppName}"; Flags: postinstall nowait 

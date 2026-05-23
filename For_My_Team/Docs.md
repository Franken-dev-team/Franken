# Panduan Pengembangan Cluxe Engine

## Cara Setup Awal
Buat folder build secara manual di root project.

Setelah compile, pastikan semua file di folder assets/ (atau file .bmp dan .ttf) di-copy ke folder tempat file executable (.exe atau binary Linux) berada.

Penting: Engine tidak akan jalan kalau file aset tidak sejajar dengan file binary.

Cross-Platform: Instruksi ini berlaku untuk semua OS. Cukup pastikan struktur folder output build kalian rapi.

## Cara Compile

### Windows:
1. Install Visual Studio 2022 dengan C++ development tools
2. Install CMake (versi 3.20 atau lebih baru)
   ```
   
   Catatan: ImGui sudah ada di folder include/, jadi tidak perlu install via vcpkg.
5. Buka Command Prompt atau PowerShell di root project
6. Jalankan perintah:
   ```
   mkdir build
   cd build
   cmake .."
   cmake --build .
   ```

### Linux:
1. Install dependencies:
   ```
   sudo apt update
   sudo apt install build-essential cmake libsdl3-dev
   ```
2. Buka terminal di root project
3. Jalankan perintah:
   ```
   mkdir build
   cd build
   cmake ..
   make -j$(nproc)
   ```

### Cross-Platform Note:
- Pengguna Linux biasanya bisa langsung pakai sudo apt install libsdl3-dev (atau distro-specific command), jadi mereka mungkin nggak butuh vcpkg. CMake akan otomatis nemu di /usr/lib/.
- Pengguna Windows (tim lu yang lain) sangat disarankan pakai vcpkg juga biar versinya sama dengan lu.

### Note:
- Untuk Windows, executable akan berada di build/Debug/
- Untuk Linux, executable akan berada di build/
- Pastikan folder assets/ di-copy ke lokasi executable setelah compile

## Instruksi Menjalankan
Jalankan CluxeEditor.exe dari folder build/Debug. Pastikan semua file pendukung (Player.bmp) berada di folder assets (folder sebelah .exe nya).

## Sistem UI
Kode untuk Editor (Engine UI) ada di src/ui/uen menggunakan Dear ImGui.

## Aturan Pathing
Dilarang keras menggunakan Absolute Path (contoh: C:\Users\...). Gunakan selalu Relative Path agar project bisa dijalankan di semua PC tanpa error.

Pathing: Tetap diingatkan untuk tidak menggunakan Absolute Path dalam kode. Gunakan path relatif terhadap lokasi binary.

## Rendering
Engine ini menggunakan SDL3 dengan sistem Logical Resolution (1280x720). Untuk UI Editor, sistem akan otomatis switcch ke Native Resolution agar teks tetap tajam.

## Sistem Docking UI (ImGui)

### Overview
Cluxe Engine menggunakan sistem docking window berbasis Dear ImGui yang memungkinkan pengaturan layout UI yang fleksibel.

### Fitur Utama
- **Multi-Window**: Properties, Main Viewport, Audio Mixer
- **Drag & Drop Docking**: Semua window bisa di-drag dan di-dock
- **Texture Click Detection**: Klik pada texture untuk mendapatkan koordinat X,Y
- **Window Management**: Show/hide window melalui menu

### Cara Penggunaan

#### 1. Window Management
- Buka menu **View** untuk toggle visibility window:
  - **Properties**: Window untuk kontrol dan informasi
  - **Main Viewport**: Tampilan utama game dengan texture
  - **Audio Mixer**: Kontrol audio dan musik
  - **Toggle Demo**: Tampilkan/hide ImGui demo window

#### 2. Docking Windows
- **Drag Tab**: Drag tab window ke posisi yang diinginkan
- **Auto-Dock**: Window akan otomatis dock saat drag dekat edge
- **Split Layout**: Window bisa di-split menjadi beberapa panel
- **Reset Layout**: Menu **Layout → Reset Layout** untuk reset ke default

#### 3. Texture Click Detection
- Klik pada texture Player.bmp di **Main Viewport**
- Koordinat klik akan muncul di window **Properties**
- Koordinat X,Y ditampilkan dalam format floating point
- Berguna untuk debugging posisi texture

#### 4. Audio Mixer Controls
- **Play/Pause/Stop**: Kontrol musik background
- **Volume Slider**: Atur volume master (0.0 - 1.0)
- **Mute Checkbox**: Mute/unmute audio
- **Status Display**: Tampilkan status music playing/stopped

### Struktur Kode

#### Variabel Docking System
```cpp
bool showProperties;
bool showMainViewport;
bool showAudioMixer;
ImGuiID dockspace_id;
bool dockspaceInitialized;
```

#### Texture Click Detection
```cpp
float clickedTextureX;
float clickedTextureY;
bool textureClicked;
```

### Tips & Tricks
1. **Layout Customization**: Sesuaikan layout sesuai kebutuhan workflow
2. **Reset Layout**: Jika layout berantakan, gunakan reset layout
3. **Texture Debugging**: Gunakan click detection untuk debugging posisi
4. **Audio Testing**: Test audio melalui Audio Mixer window

### Troubleshooting
- **Window Hilang**: Gunakan menu View untuk show kembali
- **Layout Rusak**: Reset layout melalui menu Layout
- **Texture Tidak Muncul**: Pastikan file Player.bmp ada di folder assets/
- **Audio Tidak Berfungsi**: Check file bg/bg.mp3 di folder assets/

## Kontribusi
Jika ada update pada bagian Editor, pastikan tidak merusak sistem koordinat mouse pada gameplay,dan jangan lupa kasih Dokumentasi untuk cara penggunaan sistem terbaru tersebut
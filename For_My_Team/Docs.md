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
3. Install vcpkg (jika belum ada)
4. Install packages yang dibutuhkan:
   ```
   [path_ke_vcpkg]\vcpkg install sdl3:x64-windows
   ```
   
   Catatan: ImGui sudah ada di folder include/, jadi tidak perlu install via vcpkg.
5. Buka Command Prompt atau PowerShell di root project
6. Jalankan perintah:
   ```
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE="[path_ke_vcpkg]/scripts/buildsystems/vcpkg.cmake"
   cmake --build . --config Debug
   ```
   
   Catatan: [path_ke_vcpkg] adalah lokasi di mana folder vcpkg di-install di PC masing-masing.

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

## Kontribusi
Jika ada update pada bagian Editor, pastikan tidak merusak sistem koordinat mouse pada gameplay,dan jangan lupa kasih Dokumentasi untuk cara penggunaan sistem terbaru tersebut